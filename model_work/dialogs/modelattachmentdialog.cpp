#include "modelattachmentdialog.h"
#include "ui_modelattachmentdialog.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMessageBox>
#include <QMenu>
#include "model_work/storage/modelnode.h"
#include "model_work/dialogs/modelvaluesviewer.h"
#include "model_work/dialogs/pickshprogdialog.h"

#define NOTLEAF Qt::UserRole + 1
#define MESHROLE Qt::UserRole + 2
#define NODEROLE Qt::UserRole + 3


ModelAttachmentDialog::ModelAttachmentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelAttachmentDialog),
    vertices("Vertices"),
    normals("Normals"),
    colors("Colors"),
    texCoords("Texture Coordinates"),
    indices("Indices")
{
    ui->setupUi(this);

    info = InfoManager::getInstance();

    initializeTree();
    showShaderPrograms();

    ui->dataView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->dataView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleClick(QModelIndex)));
    connect(ui->dataView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));
}

ModelAttachmentDialog::~ModelAttachmentDialog()
{
    delete ui;
}

/**
 * @brief ModelAttachmentDialog::initializeTable Initialize data for attaching model to TreeView
 */
void ModelAttachmentDialog::initializeTree()
{
    /**
     * Model info
     * Qt::UserRole + 1 (NOTLEAF) = true if this node is non leaf
     * Qt::UserRole + 2 (MESHROLE) = index of model mesh
     * Qt::UserRole + 3 (NODEROLE) = index of model node
     */

    QStandardItemModel* model = new QStandardItemModel(this);

    //model->setColumnCount(3);
    QStringList header;
    header << "Type" << "Number" << "Program attached";

    model->setHorizontalHeaderLabels(header);

    QStandardItem* root = model->invisibleRootItem();
    ModelNode* rootNode = info->getActiveProject()->getModel()->getRootNode();

    addTreeNode(rootNode, root); // recursively add data to nodes

    root->setColumnCount(3);

    ui->dataView->setModel(model);
    ui->dataView->setColumnWidth(0,width()-250);
    ui->dataView->adjustSize();
}

/**
 * @brief ModelAttachmentDialog::showShaderPrograms Update shader programs attached to meshes in model (3. column in TreeView)
 */
void ModelAttachmentDialog::showShaderPrograms()
{
    ModelNode* node;
    Mesh* mesh;
    QList<QStandardItem*> meshes = getMeshItems();

    foreach(QStandardItem* meshItem, meshes)
    {
        mesh = info->getActiveProject()->getModel()->getMesh(meshItem->data(MESHROLE).toUInt());
        node = findNode(meshItem->index());

        if(node == NULL)
            continue;

        QString prog = node->getShaderProgram(mesh);
        /*
        MetaShaderProg* prog = node->getShaderProgram(mesh);

        if(prog == NULL)
            continue;
        */

        meshItem->parent()->setChild(meshItem->row(), 2, new QStandardItem(prog));
    }

}

/**
 * @brief ModelAttachmentDialog::addTreeNode Add new node from 3D model to TreeView model
 */
QStandardItem* ModelAttachmentDialog::addTreeNode(ModelNode* node, QStandardItem* item)
{
    QPair<QList<QStandardItem*>, QList<QStandardItem*> > pair;

    // work with meshes
    if(node->getMeshesCount() != 0)
    {
        QStandardItem* meshes = new QStandardItem("Meshes");
        meshes->setColumnCount(3);
        meshes->setData(qVariantFromValue(true), NOTLEAF);
        item->appendRow(meshes);
        QStandardItem* temp;
        unsigned int counter = 0;

        QListIterator<Mesh*> it(*(node->getNodeMeshes()));

        while(it.hasNext())
        {
            temp = new QStandardItem(QString("Mesh %1").arg(counter));
            temp->setData(qVariantFromValue(true), NOTLEAF);

            pair = addMesh(it.peekNext());
            temp->appendRows(pair.first);
            temp->appendColumn(pair.second);
            temp->setData(qVariantFromValue(it.next()->getIndex()), MESHROLE);
            temp->setColumnCount(3);

            meshes->appendRow(temp);

            counter++;
        }

    }

    // work with nodes
    if(node->getChildCount() != 0)
    {
        QList<QStandardItem*> nodes;

        QStandardItem* newNode;
        unsigned int counter = 0;
        QListIterator<ModelNode*> it(*(node->getChilds()));

        while(it.hasNext())
        {
            newNode = new QStandardItem(QString("Node %1").arg(counter));
            newNode->setColumnCount(3);
            newNode->setData(qVariantFromValue(true), NOTLEAF);
            newNode->setData(qVariantFromValue(it.peekNext()->getIndex()), NODEROLE);
            addTreeNode(it.next(), newNode);
            counter++;
            nodes.append(newNode);
        }

        QStandardItem* temp = new QStandardItem("Nodes");
        temp->setColumnCount(3);
        temp->setData(qVariantFromValue(true), NOTLEAF);
        temp->appendRows(nodes);
        item->appendRow(temp);

        // appending node count
        item->setChild(temp->row(), 1, new QStandardItem(QString("%1").arg(node->getChildCount())));
    }

    return item;
}

/**
 * @brief ModelAttachmentDialog::addMesh Return QStandardItems for given mesh
 * @param mesh Mesh from 3D model node
 * @return Return pair with name column (if exists vertices etc.) and number of values there
 */
QPair<QList<QStandardItem *>, QList<QStandardItem *> > ModelAttachmentDialog::addMesh(Mesh *mesh)
{
    QList<QStandardItem*> type;
    QList<QStandardItem*> count;

    if(mesh->hasVertices())
    {
        type.append(new QStandardItem(vertices));
        count.append(new QStandardItem(QString("%1").arg(mesh->getNumberVertices())));
    }

    if(mesh->hasNormals())
    {
        type.append(new QStandardItem(normals));
        count.append(new QStandardItem(QString("%1").arg(mesh->getNumberNormals())));
    }

    if(mesh->hasColors())
    {
        for(unsigned int i = 0; i < mesh->getColorBuffersCount(); ++i)
        {
            type.append(new QStandardItem(QString(colors + "%1").arg(i)));
            count.append(new QStandardItem(QString("%1").arg(mesh->getNumberColors(i))));
        }
    }

    if(mesh->hasTexCoords())
    {
        for(unsigned int i = 0; i < mesh->getTexCoordBuffersCount(); ++i)
        {
            type.append(new QStandardItem(QString(texCoords + "%1").arg(i)));
            count.append(new QStandardItem(QString("%1").arg(mesh->getNumberTexCoords(i))));
        }
    }

    if(mesh->hasIndices())
    {
        type.append(new QStandardItem(indices));
        count.append(new QStandardItem(QString("%1").arg(mesh->getNumberIndices())));
    }

    return qMakePair(type,count);
}

/**
 * @brief ModelAttachmentDialog::findNode Find ModelNode object from TreeView item with given index. Search in parents of given item.
 * @param index Index of QStandardItem
 * @return Found model node for given item
 */
ModelNode *ModelAttachmentDialog::findNode(QModelIndex index)
{
    while(index.data(NODEROLE).isNull())
    {
        index = index.parent();
    }

    unsigned int id = index.data(NODEROLE).toUInt();

    return info->getActiveProject()->getModel()->getNode(id);
}

/**
 * @brief ModelAttachmentDialog::getMeshItems Return all QStandardItem which coresponds to meshes in TreeView
 * @return Return list of QStandardItems of meshes items
 */
QList<QStandardItem*> ModelAttachmentDialog::getMeshItems()
{
    QList<QStandardItem*> ret;
    const QStandardItemModel* model = static_cast<const QStandardItemModel*>(ui->dataView->model());
    QList<QStandardItem*> items = model->findItems(vertices, Qt::MatchRecursive | Qt::MatchFixedString);
    QSet<QStandardItem*> itemsSet;

    // remove duplicates and get mesh (not vertices)
    foreach(QStandardItem* item, items)
    {
        if(!item->parent()->data(MESHROLE).isNull())
        {
            itemsSet.insert(item->parent());
        }
    }

    ret = itemsSet.toList();

    return ret;

}

/*** SLOTS ***/
/**
 * @brief ModelAttachmentDialog::onDoubleClick Activate when user double clicked on TreeView item
 * @param index Index of double clicked item
 */
void ModelAttachmentDialog::onDoubleClick(QModelIndex index)
{
    unsigned int id = 0;
    bool isGood = false;

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(tr("Problem when getting mesh."));

    if(!index.data(MESHROLE).isNull())
    {
        id = index.data(MESHROLE).toUInt();
        isGood = true;
    }
    else if(    index.data().toString() == vertices ||
                index.data().toString() == normals ||
                index.data().toString() == colors ||
                index.data().toString() == texCoords ||
                index.data().toString() == indices)
    {
        if(!index.parent().data(MESHROLE).isNull())
        {
            id = index.parent().data(MESHROLE).toUInt();
            isGood = true;
        }
        else
        {
            msgBox.exec();
            return;
        }

    }

    if(isGood)
    {
        Mesh* mesh = info->getActiveProject()->getModel()->getMesh(id);

        ModelValuesViewer viewer(mesh,tr("Show model mesh"),this);

        viewer.exec();
    }

}

/**
 * @brief ModelAttachmentDialog::showContextMenu Show context menu on right click, if item is not leaf item in model
 * @param point Point where we create this context menu
 */
void ModelAttachmentDialog::showContextMenu(const QPoint &point)
{
    QModelIndex index = ui->dataView->indexAt(point);

    if(!index.isValid())
        return;

    if(index.parent().child(index.row(), 0).data(NOTLEAF).isNull())
        return;

    QMenu menu(this);
    QAction* attachProg = new QAction(tr("Set shader program"), &menu);
    attachProg->setData(qVariantFromValue(point));

    connect(attachProg,SIGNAL(triggered()),this,SLOT(setShaderProgDialog()));

    menu.addAction(attachProg);
    menu.exec(mapToGlobal(point));
}

/**
 * @brief ModelAttachmentDialog::setShaderProgDialog Create dialog for pick shader program
 *to attach to mesh and set this shader program to node.
 */
void ModelAttachmentDialog::setShaderProgDialog()
{
    QPoint point = static_cast<QAction*>(sender())->data().toPoint();

    QModelIndex index = ui->dataView->indexAt(point);

    PickShProgDialog dialog;

    dialog.exec();

    // user clicked cancel button
    if(dialog.result() == dialog.Rejected)
        return;

    QString progName = dialog.getShProgSet();
    MetaShaderProg* prog = info->getActiveProject()->getProgram(progName);
    ModelNode* node = findNode(index);

    if(!index.data(MESHROLE).isNull())
    {
        Mesh* mesh = info->getActiveProject()->getModel()->getMesh(index.data(MESHROLE).toUInt());
        node->setShProgramToMesh(prog->getName(), mesh);
    }
    else
    {
        QList<ModelNode*> nodeList(*node->getChilds());
        QList<ModelNode*> forWork;
        node->setShProgramToAll(prog->getName());
        unsigned int count = 0;

        // find all node
        do
        {
            nodeList.append(forWork);
            forWork.clear();

            for(int i = count; i < nodeList.size(); ++i)
            {
                if(nodeList.value(i)->getChildCount() != 0)
                {
                    forWork.append(*nodeList.value(i)->getChilds());
                }
            }
            count = nodeList.size() - 1;

        } while(!forWork.isEmpty());

        // set shader program to all nodes
        foreach(ModelNode* nNode, nodeList)
        {
            nNode->setShProgramToAll(prog->getName());
        }
    }

    // refresh new settings in tree
    showShaderPrograms();

}
