#include "shprogvariablewidget.h"
#include <QHBoxLayout>
#include <QIntValidator>

/**
 * @brief ShProgVariableWidget::ShProgVariableWidget Create new row variable widget.
 * @param parent Parent of this widget.
 */
ShProgVariableWidget::ShProgVariableWidget(QString type, QWidget *parent) :
    ShProgWidgetAbstract(type,parent)
{
    edit = new QLineEdit(this);
    position = new QLineEdit(this);
    QIntValidator* validator = new QIntValidator(this);
    validator->setRange(0,100);
    position->setValidator(validator);

    QHBoxLayout* layout = static_cast<QHBoxLayout*>(this->layout());

    layout->addWidget(position);
    layout->addWidget(edit);

    connect(edit,SIGNAL(editingFinished()),this,SLOT(editingFinishedSlot()));
    connect(position,SIGNAL(editingFinished()),this,SLOT(positionChangedSlot()));
}

/**
 * @brief ShProgVariableWidget::getVariableValue Get variable name from combo box.
 * @return Return string with variable name.
 */
QString ShProgVariableWidget::getVariableValue()
{
    return edit->text();
}

/**
 * @brief ShProgVariableWidget::setVariable Set text to edit widget.
 * @param variable Name of the variable to set here.
 */
void ShProgVariableWidget::setVariable(const QString variable)
{
    edit->setText(variable);
    old = variable;
}

/**
 * @brief ShProgVariableWidget::getPosition Return position in buffer list.
 * @return Return position number.
 */
uint ShProgVariableWidget::getPosition()
{
    return position->text().toUInt();
}

/**
 * @brief ShProgVariableWidget::setPosition Set position value.
 * @param pos Position in buffer list.
 */
void ShProgVariableWidget::setPosition(uint pos)
{
    position->setText(QString::number(pos));
}

/**
 * @brief ShProgVariableWidget::isValid Test if setted position and value is setted.
 * @return True if position and value is not empty, false otherwise.
 */
bool ShProgVariableWidget::isValid()
{
    if(!edit->text().isEmpty() && !position->text().isEmpty())
        return true;

    return false;
}

/**
 * @brief ShProgVariableWidget::editingFinishedSlot Reemit editingFinished signal, but now with data.
 */
void ShProgVariableWidget::editingFinishedSlot()
{
    QString newValue = getVariableValue();

    emit editingFinished(old, newValue, position->text().toUInt());

    old =  newValue;
}

/**
 * @brief ShProgVariableWidget::positionChangedSlot Position in list of this attachemnt point was changed.
 */
void ShProgVariableWidget::positionChangedSlot()
{
    emit positionChanged(edit->text(), position->text().toUInt());
}
