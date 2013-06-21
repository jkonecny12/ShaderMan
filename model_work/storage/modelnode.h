#ifndef MODELNODE_H
#define MODELNODE_H

#include <QList>
#include <QStringList>
#include <QMatrix4x4>
#include "model_work/storage/mesh.h"
//#include "meta_data/metashaderprog.h"

class ModelNode
{
public:
    ModelNode(ModelNode* node = NULL);
    ~ModelNode();

    static void setIndexCounter(unsigned int startIndex = 0);
    unsigned int getIndex();

    ModelNode* getParent();
    ModelNode* getChild(const unsigned int i);
    QList<ModelNode *> *getChilds();

    void addChild(ModelNode* node);
    unsigned int getChildCount();

    void addMeshToNode(Mesh* mesh);
    Mesh* getNodeMesh(unsigned int i);
    QList<Mesh *> *getNodeMeshes();
    unsigned int getMeshesCount();

    // node model transformation matrix
    QMatrix4x4 getNodeTransformation();
    void setNodeTransformation(QMatrix4x4 trans);

    // work with shader programs
    QString getShaderProgram(Mesh *mesh);
    QStringList getShaderPrograms();
    void setShProgramToMesh(QString program, Mesh *mesh);
    void setShProgramToAll(QString program);

private:
    unsigned int index;
    static unsigned int counter;
    ModelNode* parent;
    QList<ModelNode*> childs;
    QList<Mesh*> meshes;
    QMatrix4x4 modelTransformation;
    QHash<Mesh*, QString> shaderPrograms;
};

#endif // MODELNODE_H
