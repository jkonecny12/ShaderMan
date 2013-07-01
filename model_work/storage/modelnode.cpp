#include "modelnode.h"
#include <QtAlgorithms>

unsigned int ModelNode::counter = 0;

/**
 * @brief ModelNode::ModelNode Create new node for this model.
 * @param node Parent of this node.
 */
ModelNode::ModelNode(ModelNode *node)
{
    parent = node;

    index = counter;
    ++counter;
}

/**
 * @brief ModelNode::~ModelNode Destroy this node and all it's childrens.
 */
ModelNode::~ModelNode()
{
    qDeleteAll(childs);
}

/**
 * @brief ModelNode::setIndex Set new index for numbering new nodes
 * @param startIndex Starting index for countering
 */
void ModelNode::setIndexCounter(unsigned int startIndex)
{
    counter = startIndex;
}

/**
 * @brief ModelNode::getIndex Return index of this node. Index are generated automaticaly when object is created
 * @return Index of this node
 */
unsigned int ModelNode::getIndex()
{
    return index;
}

/**
 * @brief ModelNode::getParent Return parent of this node
 * @return Parent of this node or NULL if it is not set (root)
 */
ModelNode *ModelNode::getParent()
{
    return parent;
}

/**
 * @brief ModelNode::getChild Return child of this node with given id
 * @param i Id of the child
 * @return Child node
 */
ModelNode *ModelNode::getChild(const unsigned int i)
{
    return childs.value(i);
}

/**
 * @brief ModelNode::getChilds Return list of all child attached to this node
 * @return List of child nodes
 */
QList<ModelNode *>* ModelNode::getChilds()
{
    return &childs;
}

/**
 * @brief ModelNode::addChild Add new child to this node
 * @param node Child node
 */
void ModelNode::addChild(ModelNode *node)
{
    childs.append(node);
}

/**
 * @brief ModelNode::getChildCount Return number of childs attached to this node
 * @return Number of childs
 */
unsigned int ModelNode::getChildCount()
{
    return childs.count();
}

/**
 * @brief ModelNode::addMeshToNode Add mesh to this node, it is used when we drawing object
 * @param mesh Mesh attached to this node
 */
void ModelNode::addMeshToNode(Mesh *mesh)
{
    meshes.append(mesh);
}

/**
 * @brief ModelNode::getNodeMesh Return mesh from this node with given index, need for drawing this node
 * @param i Index of mesh in this node (meshes are indexed internaly in node)
 * @return Mesh object
 */
Mesh *ModelNode::getNodeMesh(unsigned int i)
{
    return meshes.value(i);
}

/**
 * @brief ModelNode::getNodeMeshes Return list of meshes attached to this node
 * @return All meshes in this node
 */
QList<Mesh *>* ModelNode::getNodeMeshes()
{
    return &meshes;
}

/**
 * @brief ModelNode::getMeshesCount Return number of meshes attached to this node
 * @return Number of meshes
 */
unsigned int ModelNode::getMeshesCount()
{
    return meshes.count();
}

/**
 * @brief ModelNode::getNodeTransformation Return transformation in for meshes in this node
 * @return Transformation matrix
 */
QMatrix4x4 ModelNode::getNodeTransformation()
{
    return modelTransformation;
}

/**
 * @brief ModelNode::setNodeTransformation Set transformation for this node
 * @param trans Transformation matrix
 */
void ModelNode::setNodeTransformation(QMatrix4x4 trans)
{
    modelTransformation = trans;
}

/**
 * @brief ModelNode::getShaderProgram Return shader program for given mesh in this node
 * @param mesh Model mesh attached to this node
 * @return If given mesh exists in this node return shader program, if not return null.
 * If no shader program is attached, return NULL.
 */
QString ModelNode::getShaderProgram(Mesh *mesh)
{
    return shaderPrograms.value(mesh, QString());
}

/**
 * @brief ModelNode::getShaderPrograms Return all shader programs attached to this node meshes.
 * @return Return list of shader programs.
 */
QStringList ModelNode::getShaderPrograms()
{
    return shaderPrograms.values();
}

/**
 * @brief ModelNode::setShProgramToMesh Set shader program to this node to given mesh
 * @param program Shader program which will be used for render this mesh in this node
 * @param mesh Mesh in this node
 */
void ModelNode::setShProgramToMesh(QString program, Mesh *mesh)
{
    bool testMesh = false;

    foreach(Mesh* hMesh, meshes)
    {
        if(hMesh == mesh)
        {
            testMesh = true;
            break;
        }
    }

    if(!testMesh)
        return;

    shaderPrograms.insert(mesh, program);
}

/**
 * @brief ModelNode::setShProgramToAll Set program shade to all meshes in this node.
 * @param program Shader program what we want to set
 */
void ModelNode::setShProgramToAll(QString program)
{
    foreach(Mesh* mesh, meshes)
    {
        shaderPrograms.insert(mesh, program);
    }
}
