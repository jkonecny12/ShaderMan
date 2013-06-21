#ifndef PROJECTMANAGERTREEMODEL_H
#define PROJECTMANAGERTREEMODEL_H

#include <QAbstractItemModel>
#include <QStyle>
#include "projecttreeitem.h"

/**
 * @brief The ProjectManagerTreeModel class Implementing model for QTree project manager in MainWindow.
 * Represents projects in program and shaders loaded in this projects.
 */
class ProjectManagerTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ProjectManagerTreeModel(QObject *parent = 0);

    ~ProjectManagerTreeModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

signals:
    
public slots:
    void projectCreated(QString projName);
    void projectRemoved(QString projName);

    void shaderCreated(QString name);
    void shaderRemoved(QString name, MetaShader::SHADERTYPE type);

    void projectActiveChanged(QString newProject, QString oldProject);

private:
    ProjectTreeItem* root;
    InfoManager* info;
    QString insertData;
    ProjectTreeItem::TYPE insertType;
    ProjectTreeItem* temp;
};

#endif // PROJECTMANAGERTREEMODEL_H
