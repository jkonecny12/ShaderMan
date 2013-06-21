#include "lineeditdelegate.h"
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>

/**
 * @brief LineEditDelegate::LineEditDelegate Create new delegate.
 * @param parent Paren of this delegate.
 */
LineEditDelegate::LineEditDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

/**
 * @brief LineEditDelegate::LineEditDelegate Create new delegate with setted type.
 * @param variableType Type of variable we want to use here.
 * @param parent Parent of this object.
 */
LineEditDelegate::LineEditDelegate(LineEditDelegate::TYPE variableType, QObject *parent) :
    QItemDelegate(parent)
{
    type = variableType;
}

/**
 * @brief LineEditDelegate::createEditor Create editor window.
 * @param parent Parent of this editor.
 * @return Editor QLineEdit.
 */
QWidget *LineEditDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/*option*/,
                                        const QModelIndex &/*index*/) const
{
    QLineEdit* edit = new QLineEdit(parent);

    /*
    if(type == INT)
        edit->setValidator(new QIntValidator());
    else if(type == UINT)
    {
        QIntValidator* validator = new QIntValidator();
        validator->setBottom(0);
        edit->setValidator(validator);
    }
    else
        edit->setValidator(new QDoubleValidator());
    */

    return edit;
}

/**
 * @brief LineEditDelegate::setEditorData Set data to editor.
 * @param editor Editor where we set our data.
 * @param index Index from model where we get needed data.
 */
void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit* edit = static_cast<QLineEdit*>(editor);

    QVariant value = index.model()->data(index,Qt::EditRole);
    edit->setText(value.toString());

    /*
    switch(type)
    {
    case UINT:
        value = index.model()->data(index,Qt::EditRole).toUInt();
        edit->setText(value.toString());
        break;
    case INT:
        value = index.model()->data(index,Qt::EditRole).toInt();
        edit->setText(value.toString());
        break;
    case FLOAT:
        value = index.model()->data(index,Qt::EditRole).toFloat();
        edit->setText(value.toString());
        break;
    case DOUBLE:
        value = index.model()->data(index,Qt::EditRole).toDouble();
        edit->setText(value.toString());
        break;
    }
    */

}

/**
 * @brief LineEditDelegate::setModelData Set data from editor window to model.
 * @param editor Editor we using for editing this cell.
 * @param model Model where we set our new data.
 * @param index Index to model where we set our data.
 */
void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QLineEdit* edit = static_cast<QLineEdit*>(editor);

    QVariant var = edit->text();

    /*
    switch(type)
    {
    case UINT:
        if(!var.canConvert<uint>())
            var = 0;
        break;
    case INT:
        if(!var.canConvert<int>())
            var = 0;
        break;
    case FLOAT:
        if(!var.canConvert<float>())
            var = 0;
        break;
    case DOUBLE:
        if(!var.canConvert<double>())
            var = 0;
        break;
    }
    */

    model->setData(index,var,Qt::EditRole);
}

/**
 * @brief LineEditDelegate::updateEditorGeometry Update geometry for this editor.
 * @param editor Editor which geometry we want to update.
 * @param option Options for needed style.
 */
void LineEditDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
