#ifndef LINEEDITDELEGATE_H
#define LINEEDITDELEGATE_H

#include <QItemDelegate>

class LineEditDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    enum TYPE {UINT, INT, FLOAT, DOUBLE};
    explicit LineEditDelegate(QObject *parent = 0);
    explicit LineEditDelegate(TYPE variableType, QObject *parent = 0);

    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    
private:
    TYPE type;

signals:
    
public slots:
    
};

#endif // LINEEDITDELEGATE_H
