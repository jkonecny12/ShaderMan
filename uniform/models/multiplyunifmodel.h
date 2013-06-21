#ifndef MULTIPLYUNIFMODEL_H
#define MULTIPLYUNIFMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

/**
 * @brief The MultiplyUnifModel class Model for modify multiply uniform variable.
 */
class MultiplyUnifModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MultiplyUnifModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    bool setData(const QModelIndex &index, const QVariant &value, int role);

    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex());

    Qt::ItemFlags flags(const QModelIndex &index) const;

    QStringList getNameList();
    void setNameList(QStringList list);

private:
    int getNameNumber();
    
signals:
    
public slots:

private:
    QStringList setNames;
    
};

#endif // MULTIPLYUNIFMODEL_H
