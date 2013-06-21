#include "attachshaderprogram.h"
#include "ui_attachshaderprogram.h"
#include <QStringList>
#include <QMessageBox>

AttachShaderProgram::AttachShaderProgram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AttachShaderProgram)
{
    ui->setupUi(this);
    info = InfoManager::getInstance();

    ui->comboBox->addItems(info->getProgramNames());
    isComboBox = true;
    createNew = false;

    connect(ui->toolBox,SIGNAL(currentChanged(int)),this,SLOT(setActiveToolWindow(int)));
}

AttachShaderProgram::~AttachShaderProgram()
{
    delete ui;
}

/**
 * @brief AttachShaderProgram::createNew We need to create new shader program
 * @return True if new program must be created
 */
bool AttachShaderProgram::isNewNeeded()
{
    return createNew;
}

/**
 * @brief AttachShaderProgram::getProgramName Get program name, attach shader to this program
 * @return Program name
 */
QString AttachShaderProgram::getProgramName()
{
    return progName;
}

/** SLOTS **/

/**
 * @brief AttachShaderProgram::setActiveToolWindow To know which window is active
 */
void AttachShaderProgram::setActiveToolWindow(int index)
{
    if(index == 0)
        isComboBox = true;
    else
        isComboBox = false;
}

/**
 * @brief AttachShaderProgram::accept Are variables valid?
 */
void AttachShaderProgram::accept()
{
    if(isComboBox)
    {
        if(info->getProgramNames().isEmpty())
        {
            QMessageBox::warning(this,tr("Canno't set shader program"),
                                 tr("Canno't set empty shader program. Please create new shader program."));
            return;
        }
        else
        {
            progName = ui->comboBox->currentText();
        }
    }
    else
    {
        if(ui->lineEdit->text().isEmpty())
        {
            QMessageBox::warning(this,tr("Canno't create shader program"),
                                 tr("You must set name of this shader program."));
            return;
        }
        else
        {
            progName = ui->lineEdit->text();
            createNew = true;
        }
    }

    setResult(Accepted);
    hide();
}
