#include "model.h"
#include "infomanager.h"
#include "model_work/tools/assimpprogresshandler.h"

/**
 * @brief Model::Model Create 3D model.
 * @param parent Parent of this object.
 */
Model::Model(QObject *parent) : QObject(parent)
{
    root = NULL;
}

/**
 * @brief Model::Model Create 3D model.
 * @param parent Parent of this object.
 * @param projectName Project name.
 */
Model::Model(QObject* parent, QString projectName) : QObject(parent)
{
    root = NULL;
    projectNames.append(projectName);
}

/**
 * @brief Model::~Model Destroy this object.
 */
Model::~Model()
{
    delete root;
}

/**
 * @brief Model::loadModel Load model from a file with given path. Be aware if file is not a model this class try to load it anyway.
 * @param path Absolute file path
 * @return True if model was properly loaded false otherwise
 */
bool Model::loadModel(const QString path)
{
    qDebug() << "Loading model" << path;

    QProgressDialog pd(tr("Loading model"), tr("Cancel"), 0, 200);
    pd.setWindowModality(Qt::WindowModal);

    AssimpProgressHandler *aProgress = new AssimpProgressHandler(&pd);

    Assimp::Importer import;
    import.SetProgressHandler(aProgress);

    const aiScene* scene = import.ReadFile(path.toLatin1().data(),aiProcess_JoinIdenticalVertices
                                           | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_SortByPType);
    pd.setValue(100);

    if(pd.wasCanceled())
        return false;

    if(!scene)
    {
        loadError = import.GetErrorString();
        return false;
    }

    if(!initModel(scene, pd))
    {
        loadError = tr("Problem with loading model data");
        return false;
    }

    this->path = path;

    pd.setValue(200);

    return true;
}

/**
 * @brief Model::setFilePath Set path to this model. If file on this path did not exists, then return false.
 * @param path Path to a 3D model file.
 * @return True if model exists, false otherwise.
 */
bool Model::setFilePath(QString path)
{
    QFile f(path);

    if(!f.exists())
        return false;


    this->path = path;

    return true;
}

/**
 * @brief Model::getFilePath Return path to loaded model
 * @return Path to model
 */
QString Model::getFilePath() const
{
    return path;
}

/**
 * @brief Model::getErrorLog Return error log if something go wrong when model was loaded
 * @return Error log
 */
const QString Model::getErrorLog()
{
    return loadError;
}

/**
 * @brief Model::getMeshesCount Return number of meshes in this model
 * @return Number of meshes
 */
unsigned int Model::getMeshesCount()
{
    return meshes.count();
}

/**
 * @brief Model::getMesh Return one mesh with given index
 * @param id Index of mesh, set when Mesh is created
 * @return Mesh object
 */
Mesh *Model::getMesh(const unsigned int id)
{
    return meshes.value(id);
}

/**
 * @brief Model::getMeshes Return list of all meshes in this model object
 * @return All meshes in this model
 */
QList<Mesh *> Model::getMeshes()
{
    return meshes;
}

/**
 * @brief Model::getRootNode Return root node for drawing model
 * @return Root node for recursive drawing
 */
ModelNode *Model::getRootNode()
{
    return root;
}

/**
 * @brief Model::getNode Return node with given id
 * @param id ID of returned node
 * @return If node with given id exist return node, else return NULL
 */
ModelNode *Model::getNode(const unsigned int id)
{
    if(root->getIndex() == id)
        return root;
    else
        return findNode(id, root);
}

/**
 * @brief Model::isInProject Question if this model is set to project with given name
 * @param name Name of project
 * @return True if this model is within given project false otherwise
 */
bool Model::isInProject(QString name)
{
    return projectNames.contains(name);
}

/**
 * @brief Model::getAttachedPrograms Get all shader programs attached to this model.
 * @return Return attached shader program names.
 */
QStringList Model::getAttachedPrograms()
{
    QSet<ModelNode*> nodes = getNodes();
    QStringList progs;

    foreach(ModelNode* node, nodes)
    {
        progs.append(node->getShaderPrograms());
    }

    progs.removeDuplicates();
    progs.removeAll("");
    /*
    QSet<MetaShaderProg*> temp = progs.toSet();
    QSet<MetaShaderProg*> ret;

    foreach(MetaShaderProg* prog, temp)
    {
        if(prog != NULL)
            ret.insert(prog);
    }
    */

    return progs;
}

/**
 * @brief Model::getShaderPrograms Get attached shader programs to all nodes, with identifier QPair<NodeId,MeshId>.
 * @return Name of shader programs with identifiers.
 */
QHash<QPair<uint,uint>, QString> Model::getShaderPrograms()
{
    QSet<ModelNode*> nodes = getNodes();
    QHash<QPair<uint,uint>,QString> result;

    foreach(ModelNode* node, nodes)
    {
        QList<Mesh*> *list = node->getNodeMeshes();

        foreach(Mesh* mesh, *list)
        {
            QPair<uint,uint> id = qMakePair(node->getIndex(),mesh->getIndex());
            QString prog = node->getShaderProgram(mesh);

            result.insert(id, prog);
        }
    }

    return result;
}

/**
 * @brief Model::setShaderPrograms Set shader programs to nodes.
 * @param names Identify node where to set shader programs with shader programs what to set.
 * Structure <QHash<QPair<nodeId,meshId>, shaderProgram>.
 * @param defaultProgram When no shader program is set for any mesh, use this one.
 */
void Model::setShaderPrograms(QHash<QPair<uint, uint>, QString> names, QString defaultProgram)
{
    QSet<ModelNode*> nodes = getNodes();

    foreach(ModelNode* node, nodes)
    {
        QList<Mesh*> *list = node->getNodeMeshes();

        foreach(Mesh* mesh, *list)
        {
            QPair<uint,uint> id = qMakePair(node->getIndex(),mesh->getIndex());

            if(names.contains(id))
                node->setShProgramToMesh(names.value(id), mesh);
            else
                node->setShProgramToMesh(defaultProgram, mesh);
        }
    }
}

/**
 * @brief Model::setShaderPrograms Set shader program to all meshes.
 * @param shaderProgram What shader program we want set.
 */
void Model::setShaderPrograms(QString shaderProgram)
{
    QHash<QPair<uint,uint>,QString> empty;

    setShaderPrograms(empty, shaderProgram);
}

/**
 * @brief Model::isLoaded Test if model is loaded correctly.
 * @return True if model is loaded correctly, false otherwise.
 */
bool Model::isLoaded()
{
    bool out = true;

    if(path.isEmpty())
        out = false;

    else if(!loadError.isEmpty())
        out = false;

    else if(root == NULL)
        out = false;

    return out;
}

/**
 * @brief Model::loadModel Hidden class for loading model with class member path. Used when loading Model from project file.
 */
void Model::loadModel()
{
    if(!path.isEmpty())
        loadModel(path);
}

/**
 * @brief Model::initModel Initialize private class members when loading new model
 * @return True if model is loaded properly false otherwise
 * @param scene Assimp scene model, all data can be mined from this class
 * @param pd Progress dialog to inform user.
 */
bool Model::initModel(const aiScene *scene, QProgressDialog& pd)
{
    if(!(scene->HasMeshes()))
        return false;

    float* vertices = NULL;
    //float* colors = NULL;
    float* normals = NULL;
    unsigned int* indices = NULL;

    Mesh::setIndexCounter(0);
    ModelNode::setIndexCounter(0);

    // for progress dialog
    float progress = 100;
    float appendProg = scene->mNumMeshes / 100.f;

    for(unsigned int i = 0; i < scene->mNumMeshes; ++i)
    {
        // for progress dialog
        progress += appendProg;
        pd.setValue(static_cast<int>(progress));

        aiMesh* mesh = scene->mMeshes[i];

        if(mesh->HasPositions())
        {
            //qDebug() << "Vertices:";
            vertices = copyVecToFloat(mesh->mVertices,mesh->mNumVertices);
        }

        if(mesh->HasNormals())
        {
            //qDebug() << "Normals:";
            normals = copyVecToFloat(mesh->mNormals, mesh->mNumVertices);
        }

        QList<unsigned int> numColors;
        QList<float*> colors;
        for(unsigned int y = 0; y < mesh->GetNumColorChannels(); ++y)
        {
            if(mesh->HasVertexColors(y))
            {
                //qDebug() << "Colors:";
                colors.append(copyColToFloat(mesh->mColors[y], mesh->mNumVertices));
                numColors.append(mesh->mNumVertices*4);
            }
        }

        QList<unsigned int> numTexCoords;
        QList<float*> texCoords;

        for(unsigned int i = 0; i < mesh->GetNumUVChannels(); ++i)
        {
            if(mesh->HasTextureCoords(i))
            {
                numTexCoords.append(mesh->mNumVertices*2);
                texCoords.append(copyTexCoordsToFloat(mesh->mTextureCoords[i], mesh->mNumVertices));
            }
        }

        if(mesh->HasFaces())
            indices = copyIdxToUInt(mesh->mFaces,mesh->mNumFaces);

        meshes.append(new Mesh(
                             mesh->mNumVertices*3, vertices,
                             mesh->mNumVertices*3, normals,
                             numColors, colors,
                             numTexCoords, texCoords,
                             mesh->mNumFaces*3, indices
                             ));

        vertices = NULL;
        normals = NULL;
        //colors = NULL;
        indices = NULL;
    }

    // copy nodes
    aiNode* node = scene->mRootNode;

    if(root != NULL)
        delete root;

    root = new ModelNode();
    copyNodes(node, root);

    return true;
}

/**
 * @brief Model::copyVecToFloat Internal method for copying aiVector3D to float*
 * @param vec Input vector from Assimp library
 * @param num Vector count
 * @return Array of floats
 */
float* Model::copyVecToFloat(aiVector3D* vec, unsigned int num)
{
    qDebug() << "Loading vertices numbers: " << num;

    float* out = new float[num*3];

    memcpy(out,vec,sizeof(float)*3*num);

/*
    for(unsigned int i = 0; i < 3*num; ++i)
        qDebug() << out[i];
*/
    return out;
}

/**
 * @brief Model::copyColToFloat Internal method for copying aiColor4D to float*
 * @param vec Input color from Assimp library
 * @param num Color count
 * @return Array of floats
 */
float *Model::copyColToFloat(aiColor4D *vec, unsigned int num)
{
    qDebug() << "Loading colors number: " << num;

    float* out = new float[num*4];

    memcpy(out,vec,sizeof(float)*4*num);

    return out;
}

/**
 * @brief Model::copyIdxToUInt Internal method for converting aiFace to unsigned int* for storing indices
 * @param face Input faces from Assimp library
 * @param num Face count
 * @return Array of unsigned ints
 */
unsigned int *Model::copyIdxToUInt(aiFace *face, unsigned int num)
{
    qDebug() << "Loading faces number: " << num;

    unsigned int* out = new unsigned int[num * 3];

    unsigned int id = 0;

    for(unsigned int i = 0; i < num;++i)
    {
        memcpy(&out[id],face[i].mIndices,sizeof(unsigned int)*3);
        id += 3;
    }
/*
    qDebug() << "Indices:";
    for(unsigned int i = 0; i < 3*num; ++i)
        qDebug() << out[i];
*/
    return out;
}

/**
 * @brief Model::copyTexCoordsToFloat Copy texture coordinates from model assimp objects to internal program structure.
 * @param vec Vector of one assimp texture coordinate object.
 * @param num Number of vertices (texture coordinates).
 * @return Return array of float texture coordinates.
 */
float *Model::copyTexCoordsToFloat(aiVector3D *vec, unsigned int num)
{
    float *ret = new float[num*2];

    for(unsigned int i = 0; i < num; ++i)
    {
        ret[i*2] = vec[i].x;
        ret[i*2 + 1] = vec[i].y;
    }

    return ret;
}

/**
 * @brief Model::copyNodes Recursive method for creating my own nodes from Assimp nodes
 * @param node Node of the model, may contain more nodes and meshes
 * @param myNode Own implementation of this assimp nodes
 */
void Model::copyNodes(aiNode *node, ModelNode *myNode)
{
    for(unsigned int i = 0; i < node->mNumMeshes; ++i)
        myNode->addMeshToNode(meshes.value(node->mMeshes[i]));

    qDebug() << "Copy nodes (recursive):" << myNode->getIndex() << "have meshes count:" << myNode->getMeshesCount();
    qDebug() << "Original node children count" << node->mNumChildren << "meshes count:" << node->mNumMeshes;

    // copy node transformation matrix
    aiMatrix4x4 trans = node->mTransformation;
    trans.Transpose();

    float temp[16];
    memcpy(temp,&trans,sizeof(float)*16);

    //float rtemp[16];

    //for(int i = 0; i < 16; ++i)
    //    rtemp[i] = temp[i];

    QMatrix4x4 myTrans(temp);

    myNode->setNodeTransformation(myTrans);

    for(unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ModelNode* myNewNode = new ModelNode(myNode);

        myNode->addChild(myNewNode);
        copyNodes(node->mChildren[i],myNewNode);
    }


    /*
    MetaProject* proj = InfoManager::getInstance()->getActiveProject();
    if(proj == NULL)
        return;

    // set shader program for drawing
    MetaShaderProg* program = InfoManager::getInstance()->getActiveProject()->getProgram();

    if(program != NULL)
        myNode->setShProgramToAll(program);
    */
}

/**
 * @brief Model::findNode Recursive finding node with given id
 * @param id ID of returned node
 * @param node Input model node to test
 * @return NULL if node with given id does not exist else given node
 */
ModelNode *Model::findNode(const unsigned int id, ModelNode *node)
{
    ModelNode* found;

    if(node->getIndex() == id)
        return node;
    else if(node->getChildCount() != 0)
    {
        for(unsigned int i = 0; i < node->getChildCount(); ++i)
        {
            found = findNode(id, node->getChild(i));

            if(found != NULL)
                return found;

        }
    }

    return NULL;
}

/**
 * @brief Model::getNodes Get all nodes from starting node in this model as set.
 * @param node Node of this model, from where we start search.
 * @return Return all nodes from starting node in this model.
 */
QSet<ModelNode *> Model::getNodes(ModelNode *node)
{
    QSet<ModelNode*> ret;

    if(node == NULL)
        node = root;

    if(node == NULL)
        return ret;

    if(node->getChildCount() != 0)
    {
        QList<ModelNode*> list = *root->getChilds();

        foreach(ModelNode* nNode, list)
        {
            ret.unite(getNodes(nNode));
        }

        ret.insert(node);
    }
    else
    {
        ret.insert(node);
    }

    return ret;
}

/**
 * @brief operator << Save model object.
 * @param stream Where to serialize.
 * @param model What to save.
 * @return Return stream.
 */
QDataStream &operator <<(QDataStream &stream, const Model &model)
{    
    stream << model.path;

    return stream;
}

/**
 * @brief operator >> Load model object.
 * @param stream From where to serialize.
 * @param model Where to save.
 * @return Return stream.
 */
QDataStream &operator >>(QDataStream &stream, Model &model)
{
    stream >> model.path;

    //model.loadModel(model.path);

    return stream;
}

