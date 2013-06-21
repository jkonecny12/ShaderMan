#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();
    void customConnect();
    QString getBaseDir();
    QString getProjectName();
    QString getFragName();
    QString getVertName();

private slots:
    void browseDir();
    virtual void accept();
    
private:
    Ui::NewProjectDialog *ui;
    QString dirPath;
    QString projName;
    QString vertexName;
    QString fragName;
};

#endif // NEWPROJECTDIALOG_H
