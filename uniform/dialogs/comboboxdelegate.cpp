#include "comboboxdelegate.h"
#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/*option*/,
                                        const QModelIndex &/*index*/) const
{
    QComboBox* box = new QComboBox(parent);

    box->addItem(QString("true"));
    box->addItem(QString("false"));

    return box;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool value = index.model()->data(index,Qt::EditRole).toBool();

    QComboBox* box = static_cast<QComboBox*>(editor);

    if(value)
        box->setCurrentIndex(0);
    else
        box->setCurrentIndex(1);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    QComboBox* box = static_cast<QComboBox*>(editor);

    if(box->currentIndex() == 0)
        model->setData(index,QVariant(true),Qt::EditRole);
    else
        model->setData(index,QVariant(false),Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
