#include "multiplyunifmodel.h"

/**
 * @brief MultiplyUnifModel::MultiplyUnifModel Model to multiply uniform table.
 * @param parent Parent of this model.
 */
MultiplyUnifModel::MultiplyUnifModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    setNames << "";
}

/**
 * @brief MultiplyUnifModel::rowCount Always only 1 row.
 * @return Number 1.
 */
int MultiplyUnifModel::rowCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

/**
 * @brief MultiplyUnifModel::columnCount Get column count of this table.
 * @return Column number.
 */
int MultiplyUnifModel::columnCount(const QModelIndex &/*parent*/) const
{
    return setNames.size();
}

/**
 * @brief MultiplyUnifModel::data Get data on given index, with given role.
 * @param index Index where we get data from model.
 * @param role What we want to get from model.
 * @return Data we want.
 */
QVariant MultiplyUnifModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.column() > (setNames.size() + 2) )
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        int column = index.column();

        if(setNames.size() <= column)
            return QVariant();
        else
            return QVariant(setNames.value(column));

    }

    return QVariant();
}

/**
 * @brief MultiplyUnifModel::setData Set data on given index, with given role.
 * @param index Index where we set data from model.
 * @param value Data we want set.
 * @return True if data was set.
 */
bool MultiplyUnifModel::setData(const QModelIndex &index, const QVariant &value, int /*role*/)
{
    if(!index.isValid())
        return false;

    int column = index.column();

    setNames.replace(column, value.toString());

    emit dataChanged(index, index);

    if((column+2) >= setNames.size()) // need to add new positions
    {
        insertColumns(setNames.size(), 2);
    }

    return true;
}

/**
 * @brief MultiplyUnifModel::insertColumns Isenrt columns to this model.
 * @param column Where we want insert columns.
 * @param count Number of columns for insertion.
 * @param parent Parent index.
 * @return True if column was inserted ok.
 */
bool MultiplyUnifModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);

    for(int i = 0; i < count; i += 2)
    {
        setNames.append("*");
        setNames.append("");
    }

    endInsertColumns();

    return true;
}

/**
 * @brief MultiplyUnifModel::flags Get flags of given index.
 * @param index Index of cell.
 * @return If column is even the cell is editable and selectable.
 */
Qt::ItemFlags MultiplyUnifModel::flags(const QModelIndex &index) const
{
    int column = index.column();

    if((column % 2) == 1)
        return Qt::ItemIsEnabled;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

/**
 * @brief MultiplyUnifModel::getNameList Get list of uniform variables names setted to this model.
 * @return List of items.
 */
QStringList MultiplyUnifModel::getNameList()
{
    QStringList ret;

    foreach(QString n, setNames)
    {
        if(n != "--" && n != "*" && !n.isEmpty())
            ret.append(n);
    }

    return ret;
}

/**
 * @brief MultiplyUnifModel::setNameList Set list of names.
 * @param list List of names.
 */
void MultiplyUnifModel::setNameList(QStringList list)
{
    int nameSize = getNameNumber();

    if(nameSize > list.size())
    {
        return;
    }

    int count = list.size() - (nameSize - 1);

    if(count < 0)
        return;

    insertColumns(setNames.size(),count*2);

    for(int i = 0; i < list.size();++i)
    {
        setNames.replace(i*2, list.value(i));
    }

    int lastIndex = setNames.size() - 1;
    QModelIndex last = createIndex(0, lastIndex, lastIndex);
    QModelIndex first = createIndex(0,0,(quintptr)0);

    dataChanged(first,last);
}

int MultiplyUnifModel::getNameNumber()
{
    int counter = 0;

    foreach(QString name, setNames)
    {
        if(name == "*")
            continue;

        ++counter;
    }

    return counter;
}
