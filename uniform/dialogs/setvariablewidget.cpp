#include "setvariablewidget.h"
#include "infomanager.h"
#include <QStringList>

SetVariableWidget::SetVariableWidget(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    //QWidget* widget = new QWidget(parent);
    this->setLayout(layout);
    varTypeLabel = new QLabel(tr(""),this);
    uniformNameBox = new QComboBox(this);
    programNameBox = new QComboBox(this);
    varNameLineEdit = new QLineEdit(this);
    deleteButton = new QPushButton("X",this);

    layout->addWidget(varTypeLabel);
    layout->addWidget(uniformNameBox);
    layout->addWidget(programNameBox);
    layout->addWidget(varNameLineEdit);
    layout->addWidget(deleteButton);

    initialize();
}

/**
 * @brief SetVariableWidget::getWidget Return widget in this line
 * @param widget Enum what widget we want return
 * @return Returned widget (QComboBox, QPushButton or QLabel)
 */
QWidget *SetVariableWidget::getWidget(SetVariableWidget::WIDGETS widget)
{
    switch(widget)
    {
    case Uniform_label:
        return varTypeLabel;

    case Uniform_comboBox:
        return uniformNameBox;

    case Program_name:
        return programNameBox;

    case ShaderName_LineEdit:
        return varNameLineEdit;

    case Delete_PushButton:
        return deleteButton;
    }

    return NULL;
}

/**
 * @brief SetVariableWidget::addLine Add this widget as new line to dialog. Set data to this widget
 * @param uniformName Name of the uniform variable
 * @param programName Name of the shader program
 * @param variableName Name of the variable in shader
 */
void SetVariableWidget::addLine(QString uniformName, QString programName, QString variableName)
{
    int max = uniformNameBox->maxCount();

    for(int i = 0; i < max; ++i)
    {
        if(uniformName == uniformNameBox->itemText(i))
        {
            uniformNameBox->setCurrentIndex(i);
            break;
        }
    }

    max = programNameBox->maxCount();

    for(int i = 0; i < max; ++i)
    {
        if(programName == programNameBox->itemText(i))
        {
            programNameBox->setCurrentIndex(i);
            break;
        }
    }

    varNameLineEdit->setText(variableName);

    QString type = InfoManager::getInstance()->getActiveProject()->
            getUniformVariable(uniformName)->getUniformTypeAsString();
    varTypeLabel->setText(type);
}

/**
 * @brief SetVariableWidget::getUniformName Return name of the set uniform variable.
 * @return Uniform variable name
 */
QString SetVariableWidget::getUniformName()
{
    return uniformNameBox->currentText();
}

/**
 * @brief SetVariableWidget::getProgramName Return name of the set shader program.
 * @return Shader program name
 */
QString SetVariableWidget::getProgramName()
{
    return programNameBox->currentText();
}

/**
 * @brief SetVariableWidget::getVariableName Return name of the shader variable.
 * @return Shader variable name
 */
QString SetVariableWidget::getVariableName()
{
    return varNameLineEdit->text();
}

/**
 * @brief SetVariableWidget::initialize Initialize comboBoxes and label
 */
void SetVariableWidget::initialize()
{
    InfoManager* info = InfoManager::getInstance();

    QStringList programs = info->getProgramNames();
    QStringList uniforms = info->getActiveProject()->getUniformNames();

    programNameBox->addItems(programs);
    uniformNameBox->addItems(uniforms);

    programNameBox->setCurrentIndex(-1);
    uniformNameBox->setCurrentIndex(-1);

    varTypeLabel->setText(tr("NONE"));

    connect(deleteButton,SIGNAL(clicked()),this,SLOT(deletePush()));
    connect(uniformNameBox,SIGNAL(activated(QString)),this,SLOT(setUniformType(QString)));
}

/** SLOTS **/

/**
 * @brief SetVariableWidget::deletePush Emmiting signal when user want to destroy this widget - user pushed button
 *  as parameter gives this widget
 */
void SetVariableWidget::deletePush()
{
    emit deleteWidget(this);
}

/**
 * @brief SetVariableWidget::setUniformType Set label for uniform type
 * @param name Name of the uniform variable
 */
void SetVariableWidget::setUniformType(QString name)
{
    QString str = InfoManager::getInstance()->getActiveProject()->getUniformVariable(name)->getUniformTypeAsString();

    varTypeLabel->setText(str);
}
