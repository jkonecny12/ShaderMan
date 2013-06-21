#include "projectmanagertreemodel.h"

#define FRAG "Fragments"
#define VERT "Vertexes"

/**
 * @brief ProjectManagerTreeModel::ProjectManagerTreeModel Create new model for project manager tree.
 * @param parent Parent of this item.
 */
ProjectManagerTreeModel::ProjectManagerTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    root = new ProjectTreeItem();
    info = InfoManager::getInstance();

}

/**
 * @brief ProjectManagerTreeModel::~ProjectManagerTreeModel Destroy this model and created tree.
 */
ProjectManagerTreeModel::~ProjectManagerTreeModel()
{
    delete root;
}

/**
 * @brief ProjectManagerTreeModel::data Get data of this item on index with given role.
 * @param index Index what item we want.
 * @param role What data role we want to get.
 * @return QVariant data.
 */
QVariant ProjectManagerTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();


    ProjectTreeItem* item = static_cast<ProjectTreeItem*>(index.internalPointer());

    // Display role
    if(role == Qt::DisplayRole) // get name string
        return QVariant(item->getDataString());
    else if(role == Qt::UserRole) // get type of this item - User roles
        return QVariant(item->getType());
    else if(role == Qt::UserRole + 1) // get name of item (program name, so shader relative path for shader, name of project etc.
    {
        ProjectTreeItem::TYPE type = item->getType();

        if(type == ProjectTreeItem::SHADER)
        {
            MetaShader* sh = static_cast<MetaShader*>(item->getData());
            return QVariant(sh->getShader());
        }
        else if(type == ProjectTreeItem::PROJECT)
        {
            MetaProject* proj = static_cast<MetaProject*>(item->getData());
            return QVariant(proj->getName());
        }
    }
    else if(role == Qt::UserRole + 2 && item->getType() == ProjectTreeItem::SHADER) // get project of this shader item
    {
        MetaProject* proj = static_cast<MetaProject*>(item->findProject());
        return QVariant(proj->getName());
    }
    // font role
    else if(role == Qt::FontRole)
    {
        if(item->isActiveProject())
        {
            QFont font;
            font.setBold(true);
            return QVariant(font);
        }
        else
            return QVariant();
    }
    else if(role == Qt::DecorationRole) // return icons for items
    {
        QIcon icon;
        MetaShader* sh;

        switch(item->getType())
        {
        case ProjectTreeItem::DIRECTORY:
            icon.addFile(":/folder_blue.png");
            break;
        case ProjectTreeItem::SHADER:
            sh = static_cast<MetaShader*>(item->getData());

            if(sh->getType() == MetaShader::FRAGMENT)
            {
                icon.addFile(":/document_frag.png");
            }
            else if(sh->getType() == MetaShader::VERTEX)
            {
                icon.addFile(":/document_vert.png");
            }
            else
                return QVariant();
            break;
        default:
            return QVariant();
        }

        return QVariant(icon);
    }

    return QVariant();
}

/**
 * @brief ProjectManagerTreeModel::flags This model is not editable so the flags are always the same.
 * @param index Index of item.
 * @return Flags of item.
 */
Qt::ItemFlags ProjectManagerTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/**
 * @brief ProjectManagerTreeModel::headerData Get header data for view.
 * @param section In what section we get name.
 * @param orientation Orientation of header.
 * @param role Role what data we want show.
 * @return Name for first column and Programs for second.
 */
QVariant ProjectManagerTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
            return QVariant("Name");
        else if(section == 1)
            return QVariant("Programs");
    }

    return QVariant();
}

/**
 * @brief ProjectManagerTreeModel::index Get index of item on row and column with given parent.
 * @param row Row number.
 * @param column Column number.
 * @param parent Parent of item we want to get.
 * @return Index of our item.
 */
QModelIndex ProjectManagerTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    ProjectTreeItem* pItem;

    if(!parent.isValid())
        pItem = root;
    else
        pItem = static_cast<ProjectTreeItem*>(parent.internalPointer());

    ProjectTreeItem* cItem = pItem->getChild(row);

    if(cItem)
        return createIndex(row,column, cItem);
    else
        return QModelIndex();
}

/**
 * @brief ProjectManagerTreeModel::parent Get parent of item with index.
 * @param child Child where we want get parent item.
 * @return Parent of given child.
 */
QModelIndex ProjectManagerTreeModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    ProjectTreeItem* cItem = static_cast<ProjectTreeItem*>(child.internalPointer());
    ProjectTreeItem* pItem = cItem->getParent();

    if(pItem == root)
        return QModelIndex();

    return createIndex(pItem->getRow(), 0, pItem);
}

/**
 * @brief ProjectManagerTreeModel::rowCount Get number of rows in given parent.
 * @param parent Where we looking for row count.
 * @return Number of rows.
 */
int ProjectManagerTreeModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return root->getRowCount();

    return static_cast<ProjectTreeItem*>(parent.internalPointer())->getRowCount();
}

/**
 * @brief ProjectManagerTreeModel::columnCount Get number of columns in given parent.
 * @param parent Where we looking for column count.
 * @return Number of columns.
 */
int ProjectManagerTreeModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return root->getColumnCount();

    return static_cast<ProjectTreeItem*>(parent.internalPointer())->getColumnCount();
}

/**
 * @brief ProjectManagerTreeModel::insertRows Insert rows to this model.
 * @param row Row number where we add items.
 * @param count How many items we want to add.
 * @param parent Parent of items we want to add.
 * @return True if we added items, false otherwise.
 */
bool ProjectManagerTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent,row,row+count);

    if(insertType == ProjectTreeItem::PROJECT)
    {
        MetaProject* proj = info->getProject(insertData);
        root->addProject(proj);
    }
    else if(insertType == ProjectTreeItem::SHADER)
    {
        Q_ASSERT(parent.isValid());
        /*
        MetaProject* p = static_cast<MetaProject*>(sender());

        if(p->getFragmentRelativePaths().contains(insertData))
            temp->createPath(insertData.split(QDir::separator()), ProjectTreeItem::FRAGMENT);
        else
            temp->createPath(insertData.split(QDir::separator()), ProjectTreeItem::VERTEX);
        */
        static_cast<ProjectTreeItem*>(parent.internalPointer())->createPath(insertData);
    }
    endInsertRows();

    return true;
}

/**
 * @brief ProjectManagerTreeModel::removeRows Remove rows from this model.
 * @param row Row number where we start removing items.
 * @param count How many items we want to remove.
 * @param parent Parent of items we want to remove.
 * @return True if we removed items, false otherwise.
 */
bool ProjectManagerTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count);

    if(insertType == ProjectTreeItem::PROJECT)
    {
        ProjectTreeItem* item = root->getChild(row);

        delete item;
    }
    else if(insertType == ProjectTreeItem::SHADER)
    {
        ProjectTreeItem* parentItem = static_cast<ProjectTreeItem*>(parent.internalPointer());
        ProjectTreeItem* item = parentItem->getChild(row);

        delete item;
    }

    endRemoveRows();

    return true;
}

/**
 * @brief ProjectManagerTreeModel::projectCreated Call this method if project with given name was created.
 * @param projName Name of the new project.
 */
void ProjectManagerTreeModel::projectCreated(QString projName)
{
    //int row = root->getRowCount();
    int row = root->getNewItemPosition(projName, ProjectTreeItem::PROJECT);

    insertData = projName;
    insertType = ProjectTreeItem::PROJECT;
    insertRows(row,1);

    MetaProject* proj = info->getProject(projName);

    connect(proj,SIGNAL(shaderCreated(QString)),this,SLOT(shaderCreated(QString)));
    connect(proj,SIGNAL(shaderDestroyed(QString, MetaShader::SHADERTYPE)),
            this,SLOT(shaderRemoved(QString, MetaShader::SHADERTYPE)));
}

/**
 * @brief ProjectManagerTreeModel::projectRemoved Call this method if project with given name was removed.
 * @param projName Name of the old project.
 */
void ProjectManagerTreeModel::projectRemoved(QString projName)
{
    int row = root->getRowCount();

    for(int i = 0; i < row; ++i)
    {
        ProjectTreeItem* item = root->getChild(i);
        if(item->getDataString() == projName)
        {
            insertData = projName;
            insertType = ProjectTreeItem::PROJECT;
            removeRows(i, 1);
            return;
        }
    }
}

/**
 * @brief ProjectManagerTreeModel::shaderCreated Call this method when new shader was created.
 * @param name Name of the new shader.
 */
void ProjectManagerTreeModel::shaderCreated(QString name)
{
    ProjectTreeItem* item;
    MetaProject* p = static_cast<MetaProject*>(sender());
    QStringList treePath;

    treePath <<  p->getName();

    if(p->getFragmentRelativePaths().contains(name))
        treePath << FRAG;
    else
        treePath << VERT;

    //treePath += name.split(QDir::separator());
    treePath += name.split('/');

    item = root->findLastCreatedFromStart(treePath);

    //int row = item->getRowCount();
    //QStringList offset = item->getNewPart(name.split(QDir::separator()));
    QStringList offset = item->getNewPart(name.split('/'));
    int row = 0;
    insertData = name;
    insertType = ProjectTreeItem::SHADER; // insert new shader
    temp = item;

    if(offset.size() == 1)
    {
        row = item->getNewItemPosition(offset.first(),ProjectTreeItem::SHADER);
    }
    else if(offset.size() == 0)
    {
        row = item->getNewItemPosition("",ProjectTreeItem::DIRECTORY);
    }
    else
    {
        row = item->getNewItemPosition(offset.first(),ProjectTreeItem::DIRECTORY);
    }

    QModelIndex index = createIndex(item->getRow(),0,item);
    insertRows(row, 1, index);

    emit dataChanged(index,index);
}

/**
 * @brief ProjectManagerTreeModel::shaderRemoved Call this method when shader was removed.
 * @param name Name of the removed shader.
 */
void ProjectManagerTreeModel::shaderRemoved(QString name, MetaShader::SHADERTYPE type)
{
    ProjectTreeItem* item;

    MetaProject* proj = dynamic_cast<MetaProject*>(sender());

    if(proj == NULL)
        return;

    //convert path to -> projectName '/' path
    QString projName = proj->getName();
    name.insert(0,projName + '/');

    if(type == MetaShader::FRAGMENT)
        //item = root->findLastCreated(name.split(QDir::separator()),ProjectTreeItem::FRAGMENT);
        item = root->findLastCreated(name.split('/'),ProjectTreeItem::FRAGMENT);
    else
        //item = root->findLastCreated(name.split(QDir::separator()),ProjectTreeItem::VERTEX);
        item = root->findLastCreated(name.split('/'),ProjectTreeItem::VERTEX);


    ProjectTreeItem* candidate = item->getRemoveCandidate();

    int row = candidate->getRow();
    insertData = name;
    insertType = ProjectTreeItem::SHADER;

    QModelIndex index = createIndex(candidate->getParent()->getRow(), 0, candidate->getParent());
    removeRows(row,1,index);



    emit dataChanged(index,index);
}

/**
 * @brief ProjectManagerTreeModel::projectActiveChanged Call this method if active project was changed.
 * @param newProject New active project.
 * @param oldProject Old active project.
 */
void ProjectManagerTreeModel::projectActiveChanged(QString newProject, QString oldProject)
{    
    // new project is set

    int rowNumber = root->getRowCount();

    ProjectTreeItem* oldP = NULL;
    ProjectTreeItem* newP = NULL;

    int oldRow = 0;
    int newRow = 0;

    for(int i = 0; i < rowNumber; ++i)
    {
        ProjectTreeItem* item = root->getChild(i);

        if(item->getDataString() == newProject)
        {
            newP = item;
            newRow = i;
        }
        else if(item->getDataString() == oldProject)
        {
            oldP = item;
            oldRow = i;
        }
    }

    if(oldP != NULL)
    {
        QModelIndex index = createIndex(oldRow, 0, oldP);

        dataChanged(index,index);
    }

    if(newP != NULL)
    {
        QModelIndex indexNew = createIndex(newRow, 0, newP);

        dataChanged(indexNew,indexNew);
    }
}
