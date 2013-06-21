#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include "infomanager.h"

/**
 * @brief The TreeView class Project manager of this program.
 */
class TreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit TreeView(QWidget *parent = 0);

    ~TreeView();

    QStandardItem* getRootItem();

protected:
    //void initialize();

private:
    void shaderMenu(const QPoint point, QModelIndex index);
    void directoryMenu(const QPoint point, QModelIndex index);
    void categoryMenu(const QPoint point, QModelIndex index);
    void projectMenu(const QPoint point, QModelIndex index);

private slots:
    void showContextMenu(const QPoint& point);
    void attachToShaderProg();
    void closeProject();
    void setActiveProject();
    void removeShader();

private:
    QStandardItemModel *model;
    InfoManager* infoM;

    QAction* attachAction;
    QAction* closeProjectAction;
    QAction* removeShaderAction;
    QAction* setActivAction;
    
signals:
    
public slots:

};

#endif // TREEVIEW_H
