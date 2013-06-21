#ifndef STANDARDMODELUNIFVALIDITY_H
#define STANDARDMODELUNIFVALIDITY_H

#include <QStandardItemModel>
#include "uniform/storage/uniformvariable.h"

/**
 * @brief The StandardModelUnifValidity class Enhance standard model with uniform variables validity.
 */
class StandardModelUnifValidity : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit StandardModelUnifValidity(UniformTypes::UNIFORM_TYPES type, QObject *parent = 0);

    bool isValid(bool mark = false);
    void setType(UniformTypes::UNIFORM_TYPES type);

private:
    bool testValidity(QStandardItem* item);
    void setValid(QStandardItem* item, bool valid);
    
signals:
    
public slots:

private slots:
    void testItem(QStandardItem* item);

private:
    UniformTypes::UNIFORM_TYPES type;
    
};

#endif // STANDARDMODELUNIFVALIDITY_H
