#ifndef PROJECTTREEITEM_H
#define PROJECTTREEITEM_H

#include <QList>
#include "infomanager.h"

class ProjectTreeItem
{
public:
    enum TYPE {ROOT, PROJECT, DIRECTORY, CATEGORY, SHADER};
    enum SHADER_TYPE {FRAGMENT, VERTEX};
    explicit ProjectTreeItem();
    explicit ProjectTreeItem(MetaProject *proj, ProjectTreeItem* parent);
    explicit ProjectTreeItem(QString data, ProjectTreeItem* parent);
    explicit ProjectTreeItem(MetaShader* shader, ProjectTreeItem* parent);
    explicit ProjectTreeItem(QStringList fromMe, QString origin, ProjectTreeItem* parent);

    ~ProjectTreeItem();

    void* getData(int column = 0);
    int getRowCount();
    int getColumnCount();
    int getRow();
    TYPE getType();
    QString getDataString(int column = 0);
    ProjectTreeItem* getParent();
    ProjectTreeItem* getChild(int row);

    //modify model
    void addProject(MetaProject* project);

    ProjectTreeItem* findLastCreated(QStringList path, SHADER_TYPE type);
    ProjectTreeItem* findLastCreatedFromStart(QStringList path);
    void createPath(QString path);

    MetaProject* findProject();
    ProjectTreeItem* getRemoveCandidate();

    int getNewItemPosition(QString name, TYPE type);
    QStringList getNewPart(QStringList path);

    //only for project
    bool isActiveProject();

protected:
    void removeMe(ProjectTreeItem* child);
    void addPath(QStringList fromMe, QString origin);
    MetaShader* getShader(QString path);

    int sortNewItem(QString name, TYPE type);

private:
    QStringList getPrograms(QList<MetaShaderProg*> list, QString shaderName);

private:
    ProjectTreeItem* parent;
    QList<ProjectTreeItem*> childs;
    InfoManager* info;
    void* data;
    QString dirData;
    TYPE type;
};

#endif // PROJECTTREEITEM_H
