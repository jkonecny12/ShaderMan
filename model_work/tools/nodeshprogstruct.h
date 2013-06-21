#ifndef NODESHPROGSTRUCT_H
#define NODESHPROGSTRUCT_H

#include <QString>
#include <QList>
#include <QHash>

class NodeShProgStruct
{
public:
    NodeShProgStruct();

private:
    NodeShProgStruct* parent;
    QList<NodeShProgStruct*> childs;

};

#endif // NODESHPROGSTRUCT_H
