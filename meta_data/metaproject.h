#ifndef METAPROJECT_H
#define METAPROJECT_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QHash>
#include <QDebug>
#include <QDataStream>
#include "meta_data/metashader.h"
#include "meta_data/metashaderprog.h"
#include "uniform/storage/uniformvariable.h"
#include "model_work/storage/model.h"
#include "texture/texturestorage.h"
#include "project_settings/settingsstorage.h"

class QFile;
class QFileInfo;

/**
 * @brief The MetaProject class Class for storing information about projects.
 * Contains shaders, shader programs, loaded model, loaded textures ...
 */
class MetaProject : public QObject
{
    Q_OBJECT

public:
    explicit MetaProject(QString name, QDir base, QObject* parent = NULL);
    explicit MetaProject(QObject* parent = NULL);
    ~MetaProject();

    QString getName();
    QDir getProjectDirectory();
    QString getProjAbsolutePath();
    void saveProject(QDataStream& out);
    void loadProject(QDataStream& in, QString basePath, int, int);
    void postLoadProject();

    //work with shaders
    bool addVertex(QString path);
    bool addFragment(QString path);
    QStringList getVertexFilePaths();
    QStringList getFragmentFilePaths();
    QString getVertexFilePath(QString name);
    QString getFragmentFilePath(QString name);
    QStringList getVertexRelativePaths();
    QStringList getFragmentRelativePaths();

    QStringList getVertexFileNames();
    QStringList getFragmentFileNames();

    MetaShader* getVertexShader(QString name);
    QList<MetaShader*> getVertexShaders();
    MetaShader* getFragmentShader(QString name);
    QList<MetaShader*> getFragmentShaders();

    MetaShader* getShader(QString name);
    bool removeShader(QString name, bool removeFile = false);

    //work with shader programs
    bool addProgram(QString name = "");
    bool removeProgram(QString name);
    QStringList getProgramNames();
    bool setShaderToProgram(QString shaderPath, const QString programName);
    QStringList getAttachedPrograms(QString shaderPath);
    QStringList getAttachedShaders(QString programName = "");
    MetaShaderProg *getProgram(QString name = "");
    /**
     * @brief getPrograms Return all shader programs attached to this project.
     * @return All shader programs in this project.
     */
    inline QList<MetaShaderProg*> getPrograms() {return programs.values();}


    //work with uniform variables
    bool addUniformVariable(UniformVariable *uVar);
    void removeUniformVariable(QString name);
    UniformVariable *getUniformVariable(const QString name);
    QStringList getUniformNames();
    QList<UniformVariable*> getUniformVariables();
    QList<UniformVariable*> getUniformVariables(QString programName);

    //work with models
    void setModel(Model* model);
    Model* getModel();
    bool isModelLoaded();

    //work with textures
    QStringList getTextureNameList();
    TextureStorage* getTexture(QString name);
    bool addTexture(TextureStorage* texture);
    /**
     * @brief removeTexture Remove texture from this project. Emit signal textureDestroyed.
     * @param name Name of the texture we want to remove.
     */
    inline void removeTexture(QString name) {textures.remove(name); emit textureDestroyed(name);}
    bool hasTexture(QString name);

    bool hasFragmentShader(QString name) const;
    bool hasVertexShader(QString name) const;
    bool hasUniformVariable(QString name) const;

    SettingsStorage *getSettings() const;
    void setSettings(SettingsStorage* settings);

    //MetaShader findShader(QString file);
    QString pathToRelative(QString path);
    QString pathToAbsolute(QString path);

private:
    void removeProgramsNull();

private slots:
    void texNameChanged();
    void progNameChanged();
    void setModelAttachment();

signals:
    void shProgCreated(QString name);
    void shProgDestroyed(QString name);
    void shaderCreated(QString path);
    void shaderDestroyed(QString path, MetaShader::SHADERTYPE type);
    void textureCreated(QString name);
    void textureDestroyed(QString name);
    void uniformCreated(QString name);
    void uniformDestroyed(QString name);
    void modelLoaded();

private:
    QString projectName;
    QDir* baseDir;
    //QList<QString> vertexS;
    //QList<QString> fragmentS;
    QHash<QString,MetaShader*> vertexM;
    QHash<QString,MetaShader*> fragmentM;
    QHash<QString,MetaShaderProg*> programs;
    QHash<QString,UniformVariable*> uniformVariables;
    QHash<QString,TextureStorage*> textures;
    QString defaultProgram;
    Model* activeModel;
    QString modelPath;
    SettingsStorage* settings;

};

#endif // METAPROJECT_H
