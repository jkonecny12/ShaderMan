#ifndef MODELATTACHMENTDIALOG_H
#define MODELATTACHMENTDIALOG_H

#include <QDialog>
#include "infomanager.h"

namespace Ui {
class ModelAttachmentDialog;
}

class ModelAttachmentDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModelAttachmentDialog(QWidget *parent = 0);
    ~ModelAttachmentDialog();

private:
    void initializeTree();
    void showShaderPrograms();
    QStandardItem* addTreeNode(ModelNode *node, QStandardItem *item);
    QPair<QList<QStandardItem*>, QList<QStandardItem*> > addMesh(Mesh *mesh);
    ModelNode* findNode(QModelIndex index);
    QList<QStandardItem *> getMeshItems();

private slots:
    void onDoubleClick(QModelIndex index);
    void showContextMenu(const QPoint &point);
    void setShaderProgDialog();
    
private:
    Ui::ModelAttachmentDialog *ui;
    InfoManager* info;
    const QString vertices;
    const QString normals;
    const QString colors;
    const QString texCoords;
    const QString indices;
};

#endif // MODELATTACHMENTDIALOG_H
