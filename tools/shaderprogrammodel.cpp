#include "shaderprogrammodel.h"

/**
 * @brief ShaderProgramModel::ShaderProgramModel Create this shader program model, get all shader program names and connect needed signals.
 * @param parent Parent of this model.
 */
ShaderProgramModel::ShaderProgramModel(QObject *parent) :
    QAbstractListModel(parent)
{
    info = InfoManager::getInstance();
    names = info->getActiveProject()->getProgramNames();
    names.sort();

    // connect all shader program to this model
    QList<MetaShaderProg*> programs = info->getActiveProject()->getPrograms();
    foreach(MetaShaderProg* prog, programs)
    {
        connect(prog,SIGNAL(nameChanged(QString,QString)),this,SLOT(programNameChanged(QString,QString)));
    }

    connect(info->getActiveProject(),SIGNAL(shProgCreated(QString)),this,SLOT(programAdded(QString)));
    connect(info->getActiveProject(),SIGNAL(shProgDestroyed(QString)),this,SLOT(programRemoved(QString)));
}

/**
 * @brief ShaderProgramModel::rowCount Get number of rows.
 * @return Return row count.
 */
int ShaderProgramModel::rowCount(const QModelIndex &/*parent*/) const
{
    return names.size();
}

/**
 * @brief ShaderProgramModel::data Get model data.
 * @param index What data to return.
 * @param role Role of data what to display.
 * @return Return data.
 */
QVariant ShaderProgramModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= names.size())
        return QVariant();

    if(role == Qt::DisplayRole)
        return QVariant(names.at(index.row()));
    else
        return QVariant();
}

/**
 * @brief ShaderProgramModel::headerData Return header data.
 * @param role Orientation of program (horizontal or vertical).
 * @return Return header data.
 */
QVariant ShaderProgramModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    return QVariant(tr("Programs"));
}

/**
 * @brief ShaderProgramModel::insertRows Insert rows to model.
 * @param row Row number where to insert.
 * @param count Number of rows we want to insert.
 * @param parent Insert items to this parent.
 * @return True if we inserted some rows, false otherwise.
 */
bool ShaderProgramModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent,row, row+count-1);

    names = info->getActiveProject()->getProgramNames();
    names.sort();

    endInsertRows();

    return true;
}

/**
 * @brief ShaderProgramModel::removeRows Remove rows from model.
 * @param row Row number where to remove.
 * @param count Number of rows we want to remove.
 * @param parent Remove items of this parent.
 * @return True if we removed some rows, false otherwise.
 */
bool ShaderProgramModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count-1);

    names = info->getActiveProject()->getProgramNames();
    names.sort();

    endRemoveRows();

    return true;
}

/**
 * @brief ShaderProgramModel::findIndex Return index pointing to given shader program.
 * @param prog Shader program what we want to find.
 * @return Return index point to shader program.
 */
const QModelIndex ShaderProgramModel::findIndex(MetaShaderProg *prog)
{
    int pos = names.indexOf(prog->getName());

    if(pos < 0)
        return QModelIndex();

    QModelIndex id = this->createIndex(pos,0);

    return id;
}

/**
 * @brief ShaderProgramModel::programAdded Add shader program to this model.
 * @param addName Name of the new shader program.
 */
void ShaderProgramModel::programAdded(QString addName)
{
    QStringList nameList = info->getActiveProject()->getProgramNames();
    nameList.sort();

    int position = nameList.indexOf(addName);

    insertRow(position);
}

/**
 * @brief ShaderProgramModel::programRemoved Remove shader program from this model.
 * @param rmName Name of the shader program what we want to remove.
 */
void ShaderProgramModel::programRemoved(QString rmName)
{
    int position = names.indexOf(rmName);

    if(position != -1)
    {
        removeRow(position);
    }
}

/**
 * @brief ShaderProgramModel::programNameChanged Name of some shader program was changed.
 * @param oldName Old name of the shader program. This name is not exists now.
 * @param newName New shader program name.
 */
void ShaderProgramModel::programNameChanged(QString oldName, QString newName)
{
    int posOld = names.indexOf(oldName);

    names.replace(posOld, newName);
    names.sort();
    int posNew = names.indexOf(newName);

    QModelIndex index0;
    QModelIndex index1;

    if(posOld < posNew)
    {
        index0 = this->index(posOld);
        index1 = this->index(posNew);
    }
    else
    {
        index0 = this->index(posNew);
        index1 = this->index(posOld);
    }


    emit dataChanged(index0, index1);
}

