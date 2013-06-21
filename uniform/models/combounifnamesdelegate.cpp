#include "combounifnamesdelegate.h"
#include <QComboBox>

/**
 * @brief ComboUnifNamesDelegate::ComboUnifNamesDelegate Create this delegate.
 * @param list List of uniform variable names, can be any strings.
 * @param parent Parent of this object.
 */
ComboUnifNamesDelegate::ComboUnifNamesDelegate(QStringList list, QObject *parent) :
    ComboBoxDelegate(parent)
{
    this->list = list;
    this->list.sort();
    this->list.prepend("--");
}

/**
 * @brief ComboUnifNamesDelegate::createEditor Create ComboBox editor for editing variables in view.
 * @param parent Parent of created editor.
 * @return Created editor.
 */
QWidget *ComboUnifNamesDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
                                              const QModelIndex &/*index*/) const
{
    QComboBox* box = new QComboBox(parent);
    box->addItems(list);

    return box;
}

/**
 * @brief ComboUnifNamesDelegate::setEditorData Set data from model to editor.
 * @param editor Where we set data.
 * @param index Where we get data to set to editor.
 */
void ComboUnifNamesDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString name = index.data().toString();
    QComboBox* box = static_cast<QComboBox*>(editor);

    int id = list.indexOf(name);

    if(id == -1)
    {
        box->setCurrentIndex(0);
    }
    else
        box->setCurrentIndex(id);
}

/**
 * @brief ComboUnifNamesDelegate::setModelData Set data from editor to model.
 * @param editor Editor used for editing view.
 * @param model Model where we set data.
 * @param index Index where to set data.
 */
void ComboUnifNamesDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);

    QString name = box->currentText();

    model->setData(index, QVariant(name));
}

