#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <QString>
#include <QDebug>
#include <QList>
#include <QSet>
#include <QStringList>
#include <QMatrix4x4>
#include <QProgressDialog>
#include "model_work/storage/mesh.h"
#include "model_work/storage/modelnode.h"
#include "meta_data/metashaderprog.h"

class Model : QObject
{
    Q_OBJECT

public:
    Model(QObject * parent);
    Model(QObject* parent, QString projectName);
    ~Model();
    void loadModel();
    bool loadModel(const QString path);

    bool setFilePath(QString path);
    QString getFilePath() const;
    const QString getErrorLog();

    //work with meshes
    unsigned int getMeshesCount();
    Mesh *getMesh(const unsigned int id);
    QList<Mesh *> getMeshes();

    //work with nodes
    ModelNode *getRootNode();
    ModelNode *getNode(const unsigned int id);
    QSet<ModelNode*> getNodes(ModelNode *node = NULL);

    bool isInProject(QString name);

    QStringList getAttachedPrograms();

    QHash<QPair<uint, uint>, QString> getShaderPrograms();
    void setShaderPrograms(QHash<QPair<uint,uint>,QString> names, QString defaultProgram);
    void setShaderPrograms(QString shaderProgram);

    bool isLoaded();

    friend QDataStream & operator<< (QDataStream& stream, const Model& model);
    friend QDataStream & operator>> (QDataStream& stream, Model& model);

private:
    bool initModel(const aiScene *scene, QProgressDialog &pd);
    float* copyVecToFloat(aiVector3D* vec, unsigned int num);
    float* copyColToFloat(aiColor4D* vec, unsigned int num);
    unsigned int* copyIdxToUInt(aiFace* face, unsigned int num);
    float* copyTexCoordsToFloat(aiVector3D* vec, unsigned int num);
    void copyNodes(aiNode* node, ModelNode* myNode);
    ModelNode* findNode(const unsigned int id, ModelNode* node);

private:
    QString path;
    QString loadError;
    QStringList projectNames;
    QList<Mesh*> meshes;
    ModelNode* root;
};

QDataStream & operator<< (QDataStream& stream, const Model& model);
QDataStream & operator>> (QDataStream& stream, Model& model);

#endif // MODEL_H
