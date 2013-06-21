#include "projecttreeitem.h"

#define FRAG "Fragments"
#define VERT "Vertexes"

/**
 * @brief ProjectTreeItem::ProjectTreeItem Create root item and start creating projects.
 */
ProjectTreeItem::ProjectTreeItem()
{
    info = InfoManager::getInstance();
    this->parent = NULL;

    QStringList list = info->getProjectNames();

    foreach(QString name, list)
    {
        int pos = sortNewItem(name, PROJECT);
        //childs.append(new ProjectTreeItem(info->getProject(name), this));
        childs.insert(pos, new ProjectTreeItem(info->getProject(name), this));
    }

    dirData = "";
    data = NULL;
    type = ROOT;
}

/**
 * @brief ProjectTreeItem::ProjectTreeItem Create project item and start creating category items (Fragments, Vertexes, ...)
 * @param proj Project object.
 * @param parent Parent of this item (root).
 */
ProjectTreeItem::ProjectTreeItem(MetaProject *proj, ProjectTreeItem *parent)
{
    info = InfoManager::getInstance();
    this->parent = parent;
    this->type = PROJECT;
    this->data = proj;
    this->dirData = proj->getName();

    childs.append(new ProjectTreeItem(FRAG,this));
    childs.append(new ProjectTreeItem(VERT,this));
}

/**
 * @brief ProjectTreeItem::ProjectTreeItem Create category item and start creating directory items.
 * @param data Name of the category.
 * @param parent Parent of this item (project).
 */
ProjectTreeItem::ProjectTreeItem(QString data, ProjectTreeItem *parent)
{
    info = InfoManager::getInstance();
    //QString* nData = new QString(data);
    this->data = NULL;
    //this->dirData = data;
    this->type = CATEGORY;
    this->parent = parent;

    MetaProject* proj = static_cast<MetaProject*>(parent->getData());
    //QDir base = proj->getProjectDirectory();
    QStringList paths;

    if(data == FRAG)
    {
        dirData = FRAG;
        paths = proj->getFragmentRelativePaths();
    }
    else
    {
        dirData = VERT;
        paths = proj->getVertexRelativePaths();
    }

    foreach(QString path, paths)
    {
        QString origin = path;

        if(data == FRAG)
            //path = QString(FRAG) + QDir::separator() + QDir::toNativeSeparators(path);
            path = QString(FRAG) + '/' + path;
        else if(data == VERT)
            //path = QString(VERT) + QDir::separator() + QDir::toNativeSeparators(path);
            path = QString(VERT) + '/' + path;

        //addPath(path.split(QDir::separator()), origin);
        addPath(path.split('/'), origin);
    }
}

/**
 * @brief ProjectTreeItem::ProjectTreeItem Create shader item. Tree leaf.
 * @param shader Shader object.
 * @param parent Parent of this item (directory or category).
 */
ProjectTreeItem::ProjectTreeItem(MetaShader *shader, ProjectTreeItem *parent)
{
    info = InfoManager::getInstance();
    this->data = shader;
    this->type = SHADER;
    this->parent = parent;
    this->dirData = shader->getShaderName();
}

/**
 * @brief ProjectTreeItem::ProjectTreeItem Create directory item and start crating new shader and directory items.
 * @param fromMe Path from this point to shader item.
 * @param origin Original string for shader creation.
 * @param parent Parent of this item (directory, shader, category).
 */
ProjectTreeItem::ProjectTreeItem(QStringList fromMe, QString origin, ProjectTreeItem *parent)
{
    info = InfoManager::getInstance();
    this->type = DIRECTORY;
    this->parent = parent;
    //this->dirData = data;
    this->data = NULL;
    this->dirData = fromMe.first();

    //QPair<QString,QString> p = getLeftmostPath(fromMe);

    addPath(fromMe, origin);
}

/**
 * @brief ProjectTreeItem::~ProjectTreeItem Destroy this item. Erase this item from parent.
 */
ProjectTreeItem::~ProjectTreeItem()
{
    qDeleteAll(childs);

    if(parent != NULL)
        parent->removeMe(this);
}

/**
 * @brief ProjectTreeItem::getData Return data of this item. Returned value is varied from type of object.
 * @return if type is return:
 * SHADER -> MetaShader
 * PROJECT -> MetaProject
 * others -> NULL
 */
void *ProjectTreeItem::getData(int /*column*/)
{
    return data;
}

/**
 * @brief ProjectTreeItem::getRowCount Return number of rows in table the same as number of childs.
 * @return Number of childs.
 */
int ProjectTreeItem::getRowCount()
{
    return childs.size();
}

/**
 * @brief ProjectTreeItem::getColumnCount Return number of columns.
 * @return If type is SHADER return 2, otherwise return 1.
 */
int ProjectTreeItem::getColumnCount()
{
    if(type == SHADER)
        return 2;
    else
        return 1;
}

/**
 * @brief ProjectTreeItem::getRow Return on which row is this item in it's parent.
 * @return Return row number.
 */
int ProjectTreeItem::getRow()
{
    return parent->childs.indexOf(this);
}

/**
 * @brief ProjectTreeItem::getType Return type of this item.
 * @return Type of this item.
 */
ProjectTreeItem::TYPE ProjectTreeItem::getType()
{
    return type;
}

/**
 * @brief ProjectTreeItem::getDataString Return string of this item.
 * @param column Column of what data we want to get.
 * @return Return string what describe this item.
 */
QString ProjectTreeItem::getDataString(int column)
{
    switch(type)
    {
    case ROOT:
        return "";
        break;
    case PROJECT:
        return dirData;
        break;
    case CATEGORY:
    case DIRECTORY:
        return dirData;
        break;
    case SHADER:
        if(column == 0)
            //return static_cast<MetaShader*>(data)->getShaderName();
            return dirData;
        else if(column == 1)
        {
            MetaShader* sh = static_cast<MetaShader*>(data);
            MetaProject* p = findProject();
            return getPrograms(p->getPrograms(),sh->getShaderName()).join(",");
        }
    }

    return "";
}

/**
 * @brief ProjectTreeItem::getParent Get parent of this item.
 * @return Parent of this item.
 */
ProjectTreeItem *ProjectTreeItem::getParent()
{
    return parent;
}

/**
 * @brief ProjectTreeItem::getChild Get child on given row.
 * @param row Where to get child.
 * @return Return child on given row.
 */
ProjectTreeItem *ProjectTreeItem::getChild(int row)
{
    return childs.value(row);
}

/**
 * @brief ProjectTreeItem::addProject Add project to this item (calling as root).
 * @param project Project what we want to add.
 */
void ProjectTreeItem::addProject(MetaProject *project)
{
    int pos = sortNewItem(project->getName(), PROJECT);
    //childs.append(new ProjectTreeItem(project, this));
    childs.insert(pos, new ProjectTreeItem(project, this));
}

/**
 * @brief ProjectTreeItem::findLastCreated Find last created item when we go with given path in this tree.
 * @param path Path to shader item.
 * @param type Is vertex or fragment.
 * @return Return last created project item in path.
 */
ProjectTreeItem *ProjectTreeItem::findLastCreated(QStringList path, SHADER_TYPE type)
{
    if(path.isEmpty())
        return this;

    if(this->type == ROOT)
    {
        //QString jPath = path.join(QDir::separator());
        //QString jPath = path.join("/");

        QString actPart = path.takeFirst();

        foreach(ProjectTreeItem* item, childs)
        {
            if(!item->data)
                return this;

            /*
            MetaProject* p = static_cast<MetaProject*>(item->data);

            QStringList list = p->getFragmentRelativePaths();
            list += p->getVertexRelativePaths();

            if(list.contains(jPath))
            {
                return item->findLastCreated(path, type);
            }
            */

            if(actPart == item->getDataString())
                return item->findLastCreated(path, type);
        }
    }
    else if(this->type == PROJECT)
    {
        foreach(ProjectTreeItem* item, childs)
        {
            if(item->getDataString() == FRAG && type == FRAGMENT)
            {
                return item->findLastCreated(path,type);
            }
            else if(item->getDataString() == VERT && type == VERTEX)
            {
                return item->findLastCreated(path,type);
            }
        }
    }
    else if(this->type == CATEGORY || this->type == DIRECTORY)
    {
        QString first = path.takeFirst();

        foreach(ProjectTreeItem* item, childs)
        {
            if(item->getDataString() == first)
            {
                if(item->getType() == SHADER)
                    return item;

                return item->findLastCreated(path,type);
            }
        }
    }

    return this;
}

/**
 * @brief ProjectTreeItem::findLastCreatedFromStart Find last created part in the tree from the path.
 * This method need path in the same structure as tree. So Project - Category - Directories - Shader.
 * @param path Path in the tree.
 * @return Last created item in tree compare to a path.
 */
ProjectTreeItem *ProjectTreeItem::findLastCreatedFromStart(QStringList path)
{
    if(path.isEmpty())
        return this;

    QString actual = path.takeFirst();

    foreach(ProjectTreeItem* item, childs)
    {
        if(item->getDataString() == actual)
        {
            return item->findLastCreatedFromStart(path);
        }
    }

    return this;
}

/**
 * @brief ProjectTreeItem::createPath Create path from this item.
 * @param path Path for creation.
 */
void ProjectTreeItem::createPath(QString path)
{
    ProjectTreeItem* item = this;
    //QStringList from = path.split(QDir::separator());
    QStringList from = path.split('/');

    while(item->getType() != CATEGORY)
    {
        //list.append(item->getDataString());
        item = item->getParent();
        from.removeFirst();
    }

    // get project
    Q_ASSERT(item->getParent()->getType() == PROJECT);

    if(from.size() == 1)
    {
        MetaShader* sh = getShader(path);
        int pos = sortNewItem(sh->getShaderName(), SHADER);
        //childs.append(new ProjectTreeItem(sh, this));
        childs.insert(pos, new ProjectTreeItem(sh, this));
    }
    else
    {
        int pos = sortNewItem(from.value(0,""), DIRECTORY);
        //childs.append(new ProjectTreeItem(from, path, this));
        childs.insert(pos,new ProjectTreeItem(from, path, this));
    }

    //item->addPath(from,path.join(QDir::separator()));

}

/**
 * @brief ProjectTreeItem::removeMe Remove given child from this item.
 * @param child Child to remove.
 */
void ProjectTreeItem::removeMe(ProjectTreeItem *child)
{
    if(!childs.contains(child))
        return;

    childs.removeOne(child);
    //delete child;
}

/**
 * @brief ProjectTreeItem::addPath Add path to this item.
 * @param fromMe Next path from this item.
 * @param origin Original string to shader.
 */
void ProjectTreeItem::addPath(QStringList fromMe, QString origin)
{
    QString first = fromMe.takeFirst();

    Q_ASSERT_X(first == dirData,"Adding Project tree data",QString("%1 == %2").arg(first, dirData).toLatin1());

    if(fromMe.size() == 1)
    {
        MetaShader* sh = getShader(origin);
        int pos = sortNewItem(sh->getShaderName(), SHADER);
        //childs.append(new ProjectTreeItem(sh,this));
        childs.insert(pos, new ProjectTreeItem(sh,this));
        return;
    }

    QString next = fromMe.first();

    foreach(ProjectTreeItem* child, childs)
    {
        if(next == child->getDataString())
        {
            child->addPath(fromMe, origin);
            return;
        }
    }

    int pos = sortNewItem(fromMe.first(), DIRECTORY);
    //childs.append(new ProjectTreeItem(fromMe, origin, this));
    childs.insert(pos, new ProjectTreeItem(fromMe, origin, this));
}

/**
 * @brief ProjectTreeItem::findProject Find project item for this item.
 * @return Project item.
 */
MetaProject *ProjectTreeItem::findProject()
{
    ProjectTreeItem* item = parent;

    while(item->getType() != PROJECT)
        item = item->getParent();

    return static_cast<MetaProject*>(item->getData());
}

/**
 * @brief ProjectTreeItem::getRemoveCandidate Get candidate for remove.
 * Candidates are these which will have no childs when this child will be removed.
 * @return Return cadidate to remove.
 */
ProjectTreeItem *ProjectTreeItem::getRemoveCandidate()
{
    ProjectTreeItem* item = this;

    qDebug() << this->getDataString();

    while(item->getParent()->getRowCount() == 1)
    {
        if(item->getParent()->type == DIRECTORY)
            item = item->getParent();
    }

    return item;
}

/**
 * @brief ProjectTreeItem::getNewItemPosition Get position in this item for new item. It will not create this new item.
 * @param name Name what we want to add.
 * @param type Type of new item.
 * @return Return position where this item will be created.
 */
int ProjectTreeItem::getNewItemPosition(QString name, TYPE type)
{
    return sortNewItem(name,type);
}

/**
 * @brief ProjectTreeItem::getNewPart Remove old parts from path and return only new parts from this item.
 * @param path Path to shader.
 * @return Get new part of path.
 */
QStringList ProjectTreeItem::getNewPart(QStringList path)
{
    ProjectTreeItem* item = this;
    QStringList from = path;

    while(item->getType() != CATEGORY)
    {
        //list.append(item->getDataString());
        item = item->getParent();
        from.removeFirst();
    }

    return from;
}

/**
 * @brief ProjectTreeItem::isActiveProject Test if this project item is set as active.
 * @return If this is project and is active return true, false otherwise.
 */
bool ProjectTreeItem::isActiveProject()
{
    if(type != PROJECT)
        return false;

    MetaProject* proj = static_cast<MetaProject*>(data);
    MetaProject* active = info->getActiveProject();

    if(proj == active)
        return true;

    return false;
}

/**
 * @brief ProjectTreeItem::getShader Get shader item with given path.
 * @param path Path of the shader item.
 * @return Shader object.
 */
MetaShader *ProjectTreeItem::getShader(QString path)
{
    MetaProject* proj = findProject();

    if(proj->getFragmentRelativePaths().contains(path))
        return proj->getFragmentShader(path);
    else
        return proj->getVertexShader(path);
}

/**
 * @brief ProjectTreeItem::sortNewItem Get position of new item to add. Sort childs.
 * @param name Name of the new item.
 * @param type Type of the new item.
 * @return Position where will be new item.
 */
int ProjectTreeItem::sortNewItem(QString name, TYPE type)
{
    if(childs.isEmpty())
        return 0;

    QStringList listMain;
    QStringList listShader;

    foreach(ProjectTreeItem* p, childs)
    {
        if(p->getType() == SHADER)
            listShader.append(p->getDataString());
        else
            listMain.append(p->getDataString());
    }

    if(type == SHADER)
    {
        listShader.append(name);
        listShader.sort();
        return listMain.size() + listShader.indexOf(name);
    }

    listMain.append(name);
    listMain.sort();

    return listMain.indexOf(name);

    /*
    foreach(ProjectTreeItem* p, childs)
    {
        if(itemType == PROJECT)
            listProjects.append(p->getDataString());
        else if(itemType == DIRECTORY)
            listDirectory.append(p->getDataString());
        else if(itemType == SHADER)
            listShader.append(p->getDataString());
    }

    if(itemType == PROJECT)
    {
        listProjects.append(name);
        listProjects.sort();
        return listProjects.indexOf(name);
    }
    else if(itemType == DIRECTORY)
    {
        listDirectory.append(name);
        listDirectory.sort();
        return listDirectory.indexOf(name);
    }
    else if(itemType == SHADER)
    {

    }

    return list.indexOf(name);
    */

/*
    for(int i = 0; i < childs.size(); ++i)
    {
        QString chName = childs.value(i);

        if(chName == name)
            return i;
    }
*/
    //return childs.size();
}

/**
 * @brief ProjectTreeItem::getPrograms Get list of shader program names where this shader is set.
 * @param list List of shader programs.
 * @param shaderName Name of the shader.
 * @return List of shader program names.
 */
QStringList ProjectTreeItem::getPrograms(QList<MetaShaderProg *> list, QString shaderName)
{
    QStringList ret;

    foreach(MetaShaderProg* p, list)
    {
        if(p->getShaders().contains(shaderName))
        {
            ret.append(p->getName());
            continue;
        }
    }

    return ret;
}

