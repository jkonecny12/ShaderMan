#include "pickshprogdialog.h"
#include "ui_pickshprogdialog.h"
#include "infomanager.h"

PickShProgDialog::PickShProgDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PickShProgDialog)
{
    ui->setupUi(this);

    setUpComboBox();

    adjustSize();
}

PickShProgDialog::~PickShProgDialog()
{
    delete ui;
}

/**
 * @brief PickShProgDialog::getShProgSet Return setted shader program
 * @return Name of the shader program
 */
const QString PickShProgDialog::getShProgSet()
{
    return ui->shProgChooser->currentText();
}

/**
 * @brief PickShProgDialog::setUpComboBox Create combo box, where user can pick existing shader program.
 */
void PickShProgDialog::setUpComboBox()
{
    InfoManager* info = InfoManager::getInstance();

    QStringList list = info->getActiveProject()->getProgramNames();

    ui->shProgChooser->addItems(list);
}
