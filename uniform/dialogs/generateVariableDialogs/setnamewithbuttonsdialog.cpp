#include "setnamewithbuttonsdialog.h"
#include "ui_setnamewithbuttonsdialog.h"
#include "infomanager.h"
#include <QMessageBox>

/**
 * @brief SetNameWithButtonsDialog::SetNameWithButtonsDialog Create this dialog window. Set default values.
 * @param parent Parent of this object.
 */
SetNameWithButtonsDialog::SetNameWithButtonsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetNameWithButtonsDialog)
{
    ui->setupUi(this);

    ui->minusBtnId->setMaximum(14);
    ui->minusBtnId->setMinimum(0);
    ui->minusBtnId->setValue(0);

    ui->plusBtnId->setMaximum(14);
    ui->plusBtnId->setMinimum(0);
    ui->plusBtnId->setValue(1);
}

/**
 * @brief SetNameWithButtonsDialog::~SetNameWithButtonsDialog Destroy this dialog and free resources.
 */
SetNameWithButtonsDialog::~SetNameWithButtonsDialog()
{
    delete ui;
}

/**
 * @brief SetNameWithButtonsDialog::getVariableName Get setted uniform variable name.
 * @return Uniform variable name.
 */
QString SetNameWithButtonsDialog::getVariableName()
{
    return name;
}

/**
 * @brief SetNameWithButtonsDialog::getButtonMinus Get id of minus button.
 * @return Id of button.
 */
int SetNameWithButtonsDialog::getButtonMinus()
{
    return buttonMinus;
}

/**
 * @brief SetNameWithButtonsDialog::getButtonPlus Get id of plus button.
 * @return Id of button.
 */
int SetNameWithButtonsDialog::getButtonPlus()
{
    return buttonPlus;
}

/**
 * @brief SetNameWithButtonsDialog::accept If user press Ok button then test all setted information.
 */
void SetNameWithButtonsDialog::accept()
{
    MetaProject* proj = InfoManager::getInstance()->getActiveProject();

    name = ui->nameEdit->text();

    if(name.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("Name canno't be empty!"));
        box.setIcon(QMessageBox::Warning);

        box.exec();

        setResult(false);
        return;
    }

    QStringList names = proj->getUniformNames();

    if(names.contains(name))
    {
        QMessageBox box;
        box.setText(tr("This name is used already!"));
        box.setIcon(QMessageBox::Warning);

        box.exec();

        setResult(false);
        return;
    }

    buttonMinus = ui->minusBtnId->value();
    buttonPlus = ui->plusBtnId->value();

    setResult(true);
    hide();
}
