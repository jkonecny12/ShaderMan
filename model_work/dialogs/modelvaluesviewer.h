#ifndef MODELVALUESVIEWER_H
#define MODELVALUESVIEWER_H

#include <QDialog>
#include <QStandardItemModel>
#include "model_work/storage/mesh.h"

namespace Ui {
class ModelValuesViewer;
}

class ModelValuesViewer : public QDialog
{
    Q_OBJECT
    
public:
    explicit ModelValuesViewer(Mesh* mesh, QString name, QWidget *parent);
    ~ModelValuesViewer();

private:
    void initTable();
    
private:
    Ui::ModelValuesViewer *ui;
    Mesh* mesh;
    QString name;
};

#endif // MODELVALUESVIEWER_H
