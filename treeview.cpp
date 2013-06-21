#include "treeview.h"
#include <QMenu>
#include <QHeaderView>
#include <QMessageBox>
#include "dialogs/shader_programs/attachshaderprogram.h"
#include "storage/projectmanagertreemodel.h"

/**
 * @brief TreeView::TreeView Create tree view with model for project management.
 * @param parent Parent of this item.
 */
TreeView::TreeView(QWidget *parent) :
    QTreeView(parent)
{
    infoM = InfoManager::getInstance();

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    //create actions for contex menus
    attachAction = new QAction(tr("Attach to program"),this);
    closeProjectAction = new QAction(tr("Close project"), this);
    removeShaderAction = new QAction(tr("Remove"), this);
    setActivAction = new QAction(tr("Set as active"), this);

    connect(attachAction,SIGNAL(triggered()),this,SLOT(attachToShaderProg()));
    connect(closeProjectAction,SIGNAL(triggered()),this,SLOT(closeProject()));
    connect(removeShaderAction,SIGNAL(triggered()),this,SLOT(removeShader()));
    connect(setActivAction,SIGNAL(triggered()),this,SLOT(setActiveProject()));

    ProjectManagerTreeModel* model = new ProjectManagerTreeModel(this);
    connect(infoM,SIGNAL(projectCreated(QString)),model,SLOT(projectCreated(QString)));
    connect(infoM,SIGNAL(projectRemoved(QString)),model,SLOT(projectRemoved(QString)));
    connect(infoM,SIGNAL(defaultProjectChanged(QString,QString)),model,SLOT(projectActiveChanged(QString,QString)));
    setModel(model);
}

/**
 * @brief TreeView::~TreeView Destroy this tree view.
 */
TreeView::~TreeView()
{

}

/**
  Return root item of the model
  */
QStandardItem* TreeView::getRootItem()
{
    return(model->invisibleRootItem());
}

/**
 * @brief TreeView::shaderMenu When we want show context menu for shader.
 * @param point Point where we want show menu.
 */
void TreeView::shaderMenu(const QPoint point, QModelIndex /*index*/)
{
    QMenu menu(this); //create menu

    //add actions to menu
    //menu.addAction(attachAction);
    menu.addAction(removeShaderAction);

    //show menu
    menu.exec(mapToGlobal(point));
}

/**
 * @brief TreeView::directoryMenu When we want show context menu for directory.
 */
void TreeView::directoryMenu(const QPoint /*point*/, QModelIndex /*index*/)
{
}

/**
 * @brief TreeView::categoryMenu When we want show context menu for category.
 */
void TreeView::categoryMenu(const QPoint /*point*/, QModelIndex /*index*/)
{
}

/**
 * @brief TreeView::projectMenu When we want show context menu for project.
 * @param point Point where we want show menu.
 */
void TreeView::projectMenu(const QPoint point, QModelIndex /*index*/)
{
    QMenu menu(this);

    menu.addAction(setActivAction);
    menu.addAction(closeProjectAction);
    menu.exec(mapToGlobal(point));
}

/** SLOTS **/

/**
 * @brief TreeView::showContextMenu Show context menu, this method will call methods for exact context menu creation.
 * @param point Point where user clicked.
 */
void TreeView::showContextMenu(const QPoint& point)
{
    QModelIndex index = indexAt(point);

    //user clicked to empty space
    if(!index.isValid())
        return;

    if(!index.data(Qt::UserRole).isValid())
        return;

    switch(index.data(Qt::UserRole).toInt())
    {
    case ProjectTreeItem::SHADER:
        qDebug("Showing context menu -- shader");
        shaderMenu(point, index);
        break;
    case ProjectTreeItem::DIRECTORY:
        qDebug("Showing context menu -- directory");
        directoryMenu(point, index);
        break;
    case ProjectTreeItem::CATEGORY:
        qDebug("Showing context menu -- category");
        categoryMenu(point, index);
        break;
    case ProjectTreeItem::PROJECT:
        qDebug("Showing context menu -- project");
        projectMenu(point, index);
        break;
    default:
        return;
    }
}

/**
 * @brief TreeView::attachToShaderProg Create and attach or only attach shader to shader program.
 *Open Dialog window for creating or/and attaching shader to program.
 */
void TreeView::attachToShaderProg()
{
    //will be implemented in future

    /*
    QModelIndex index = currentIndex();

    AttachShaderProgram prog;

    prog.exec();

    AttachShaderProgram prog;

    prog.exec();

    if(prog.result() == AttachShaderProgram::Accepted)
    {
        QModelIndex index = currentIndex();
        QModelIndex root = model->invisibleRootItem()->index();
        QModelIndex temp = index.parent();

        // find project name
        while(temp != root)
        {
            index = temp;
            temp = index.parent();
        }

        if(prog.isNewNeeded()) //we need to create new shader program
        {
            qDebug() << "Creating new shader program " << prog.getProgramName();
            infoM->createProgram(prog.getProgramName(),index.data().toString());
        }

        qDebug() << "Attaching shader " << currentIndex().data(Qt::UserRole).toString() <<
                    " to program " << prog.getProgramName() << " in project " << index.data().toString();
        infoM->attachShaderToProgram(currentIndex().data(Qt::UserRole).toString(),
            prog.getProgramName(),index.data().toString());

        QModelIndex cur = currentIndex();

        model->setData(cur.parent().child(cur.row(),1), prog.getProgramName());
    }
    */
}

/**
 * @brief TreeView::closeProject Close selected project. If project was active automatically set other as active.
 */
void TreeView::closeProject()
{
    QModelIndex index = currentIndex();

    QMessageBox box;
    box.setText(tr("Do you really want to close this project?"));
    box.setInformativeText(tr("Nothing in this project will be removed from harddrive."));
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Cancel);
    box.setIcon(QMessageBox::Question);

    int ret = box.exec();

    if(ret == QMessageBox::Yes)
    {
        QVariant name = index.data();

        infoM->closeProject(name.toString());
    }
}

/**
 * @brief TreeView::setActiveProject Set selected project as active project. Active project can be only one.
 */
void TreeView::setActiveProject()
{
    QModelIndex index = currentIndex();

    if(index.data(Qt::UserRole).toInt() != ProjectTreeItem::PROJECT)
        return;

    infoM->setActiveProject(index.data().toString());
}

/**
 * @brief TreeView::removeShader Remove selected shader. Ask user for shader deletion and after ask user for file deletion.
 */
void TreeView::removeShader()
{
    QModelIndex index = currentIndex();

    QMessageBox box;
    box.setText(tr("Do you really want to remove %1 shader from project?").arg(index.data().toString()));
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Cancel);
    box.setIcon(QMessageBox::Question);

    int ret = box.exec();

    if(ret == QMessageBox::Yes)
    {
        QVariant projName = index.data(Qt::UserRole + 2);
        MetaProject* proj = infoM->getProject(projName.toString());
        QVariant shName = index.data(Qt::UserRole + 1);
        MetaShader* sh = proj->getShader(shName.toString());

        QMessageBox boxRmFile;
        boxRmFile.setText(tr("Do you want remove file?"));
        boxRmFile.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        boxRmFile.setDefaultButton(QMessageBox::Cancel);
        boxRmFile.setIcon(QMessageBox::Question);

        int retRm = boxRmFile.exec();

        if(retRm == QMessageBox::Yes)
        {
            proj->removeShader(sh->getShader(), true);
        }
        else if(retRm == QMessageBox::No)
        {
            proj->removeShader(sh->getShader(), false);
        }
    }
}
