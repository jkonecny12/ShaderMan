#ifndef COMBOUNIFNAMESDELEGATE_H
#define COMBOUNIFNAMESDELEGATE_H

#include "uniform/dialogs/comboboxdelegate.h"
#include <QStringList>

/**
 * @brief The ComboUnifNamesDelegate class Delegate with list of strings given to constructor.
 */
class ComboUnifNamesDelegate : public ComboBoxDelegate
{
    Q_OBJECT
public:
    explicit ComboUnifNamesDelegate(QStringList list, QObject *parent = 0);
    
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;


signals:
    
public slots:

private:
    QStringList list;
    
};

#endif // COMBOUNIFNAMESDELEGATE_H
