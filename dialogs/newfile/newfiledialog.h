#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
#include <QFile>
#include <QDir>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "infomanager.h"

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewFileDialog(QWidget *parent = 0);
    explicit NewFileDialog(QString projectBaseDir,QWidget *parent = 0);
    ~NewFileDialog();
    QString getPath();
    InfoManager::SHADERTYPE getType();



private:
    void customConnect();
    void initialize();

private slots:
    void selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/);
    void openSaveManager();
    void pathEdited();
    void accept(); // overloaded function for accepting selection


private:
    Ui::NewFileDialog *ui;
    QString path;
    QString fName;
    QStandardItemModel *model;
    QItemSelectionModel *selectionM;
    InfoManager::SHADERTYPE fType;
    bool isDefault;
};

#endif // NEWFILEDIALOG_H
