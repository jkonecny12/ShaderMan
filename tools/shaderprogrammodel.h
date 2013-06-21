#ifndef SHADERPROGRAMMODEL_H
#define SHADERPROGRAMMODEL_H

#include <QAbstractListModel>
#include "infomanager.h"

class ShaderProgramModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ShaderProgramModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    const QModelIndex findIndex(MetaShaderProg *prog);
    
signals:
    
public slots:
    void programAdded(QString addName);
    void programRemoved(QString rmName);
    void programNameChanged(QString oldName, QString newName);

private:
    InfoManager* info;
    QStringList names;
};

#endif // SHADERPROGRAMMODEL_H
