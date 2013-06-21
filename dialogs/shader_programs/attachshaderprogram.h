#ifndef ATTACHSHADERPROGRAM_H
#define ATTACHSHADERPROGRAM_H

#include <QDialog>
#include <QLineEdit>
#include "infomanager.h"

namespace Ui {
class AttachShaderProgram;
}

class AttachShaderProgram : public QDialog
{
    Q_OBJECT
    
public:
    explicit AttachShaderProgram(QWidget *parent = 0);
    ~AttachShaderProgram();

    bool isNewNeeded();
    QString getProgramName();

private slots:
    void setActiveToolWindow(int index);
    void accept();

private:
    Ui::AttachShaderProgram *ui;
    InfoManager* info;
    bool isComboBox;
    QString progName;
    bool createNew;
};

#endif // ATTACHSHADERPROGRAM_H
