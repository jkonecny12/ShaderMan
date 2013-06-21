#ifndef FILETYPEDIALOG_H
#define FILETYPEDIALOG_H

#include <QDialog>
#include "infomanager.h"

namespace Ui {
class FileTypeDialog;
}

class FileTypeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileTypeDialog(QWidget *parent = 0);
    ~FileTypeDialog();
    InfoManager::SHADERTYPE getFileType();

protected:
    void customConnect();
    
protected:
    InfoManager::SHADERTYPE fileType;

private:
    Ui::FileTypeDialog *ui;

public slots:
    void vertexPressed();
    void fragmentPressed();
    void textPressed();

};

#endif // FILETYPEDIALOG_H
