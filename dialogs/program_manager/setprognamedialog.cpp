#include "setprognamedialog.h"
#include "ui_setprognamedialog.h"
#include "infomanager.h"
#include <QMessageBox>

/**
 * @brief SetProgNameDialog::SetProgNameDialog Create dialog for setting shader program name.
 * @param parent Parent of this dialog.
 */
SetProgNameDialog::SetProgNameDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetProgNameDialog)
{
    ui->setupUi(this);
}

/**
 * @brief SetProgNameDialog::~SetProgNameDialog Destroy this object.
 */
SetProgNameDialog::~SetProgNameDialog()
{
    delete ui;
}

/**
 * @brief SetProgNameDialog::getValue Get setted name.
 * @return Return name of the shader program.
 */
QString SetProgNameDialog::getValue()
{
    return ui->nameEdit->text();
}

/**
 * @brief SetProgNameDialog::accept Test setted name of this dialog. This method will be called, when user accept this dialog.
 */
void SetProgNameDialog::accept()
{
    QStringList list = InfoManager::getInstance()->getActiveProject()->getProgramNames();
    QString name = ui->nameEdit->text();

    if(list.contains(name))
    {
        QMessageBox box;
        box.setText(tr("This program name exists already."));
        box.setIcon(QMessageBox::Warning);
        box.exec();
        return;
    }
    else if(name.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("Program name canno't be empty."));
        box.setIcon(QMessageBox::Critical);
        box.exec();
        return;
    }
    else
    {
        setResult(QDialog::Accepted);
        hide();
    }
}
