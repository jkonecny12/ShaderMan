#include "metaproject.h"
#include "tools/utility.h"
#include <QMessageBox>

/**
 * @brief MetaProject::MetaProject Create meta project class. Contains all information about projects.
 * @param name Name this project.
 * @param base Base directory of this project.
 * @param parent Parent of this object for QObject.
 */
MetaProject::MetaProject(QString name, QDir base, QObject *parent) : QObject(parent)
{
    projectName = name;
    baseDir = new QDir(base);
    activeModel = NULL;
    settings = new SettingsStorage(this);
    //MetaShaderProg* prog = new MetaShaderProg(QString("prog"),*baseDir);
    //programs.insert("prog",prog);

    connect(this,SIGNAL(shProgCreated(QString)),this,SLOT(setModelAttachment()));
    connect(this,SIGNAL(modelLoaded()),this,SLOT(setModelAttachment()));
}

/**
 * @brief MetaProject::MetaProject Create empty project class.
 * @param parent Parent of this object for QObject.
 */
MetaProject::MetaProject(QObject *parent) : QObject(parent)
{
    activeModel = NULL;
    baseDir = NULL;
    settings = NULL;
}

/**
 * @brief MetaProject::~MetaProject Destroy this project class and destroy all inside of this project.
 */
MetaProject::~MetaProject()
{
    delete baseDir;
    delete activeModel;
    qDeleteAll(vertexM);
    qDeleteAll(fragmentM);
    qDeleteAll(programs);
    qDeleteAll(uniformVariables);
}

/**
  Add vertex shader to project
  */
bool MetaProject::addVertex(QString path)
{
    //path = QDir::toNativeSeparators(path);
    //vertexS.append(path);

    // if path is not relative to project directory, then change it
    if(!QFileInfo(path).isRelative())
    {
        path = baseDir->relativeFilePath(QFileInfo(path).absoluteFilePath());
    }

    // return false if this shader is in array
    if(vertexM.contains(path))
    {
        qDebug("This key is already in array");
        return false;
    }

    if(fragmentM.contains(path))
    {
        qDebug("This key is already but in fragment shaders");
        return false;
    }

    vertexM[path] = new MetaShader(path,MetaShader::VERTEX,this);

    qDebug() << "Appending vertex shader to project " << path;
    emit shaderCreated(path);

    return true;
}

/**
  Add fragment shader to project
  */
bool MetaProject::addFragment(QString path)
{
    //path = QDir::toNativeSeparators(path);

    // if path is not relative to project directory, then change it
    if(!QFileInfo(path).isRelative())
    {
        path = baseDir->relativeFilePath(QFileInfo(path).absoluteFilePath());
    }

    // return false if this shader is in array
    if(fragmentM.contains(path))
    {
        qDebug("This key is already in array");
        return false;
    }

    if(vertexM.contains(path))
    {
        qDebug("This key is already but in vertex shaders");
        return false;
    }

    //fragmentS.append(path);
    fragmentM[path] = new MetaShader(path,MetaShader::FRAGMENT,this);

    qDebug() << "Appending fragment shader to project " << path;
    emit shaderCreated(path);

    return true;
}

/**
  Return project name
  */
QString MetaProject::getName()
{
    return projectName;
}

/**
  Return project base directory
  */
QDir MetaProject::getProjectDirectory()
{
    return *baseDir;
}

/**
  Return project absolute path base directory as QString
  */
QString MetaProject::getProjAbsolutePath()
{
    //return QDir::toNativeSeparators(baseDir->absolutePath());
    return baseDir->absolutePath();
}

/**
  Save MetaProject data to QDataStream
  */
void MetaProject::saveProject(QDataStream &out)
{
    out << projectName;

    out << settings;

    out << vertexM;
    out << fragmentM;

    removeProgramsNull();

    out << programs;
    out << defaultProgram;

    out << uniformVariables;

    //set project path for saving textures
    foreach(TextureStorage* tex, textures.values())
    {
        tex->setBasePath(this->baseDir->path());
    }

    out << textures;

    bool loaded = isModelLoaded();
    out << loaded;

    if(loaded)
    {
        QString path = activeModel->getFilePath();
        path = pathToRelative(path);
        out << path;
        out << activeModel->getShaderPrograms();
    }
}

/**
  Load MetaProject data to QDataStream
  */
void MetaProject::loadProject(QDataStream &in, QString basePath, int /*versionMajor*/, int /*versionMinor*/)
{
    //basePath = QDir::toNativeSeparators(basePath);

    in >> projectName;

    if(basePath.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("This project canno't be loaded!"));
        box.setIcon(QMessageBox::Warning);
        box.exec();
        return;
    }

    baseDir = new QDir(basePath);

    in >> settings;
    settings->setParent(this);

    in >> vertexM;
    in >> fragmentM;

    foreach(MetaShader* shader, vertexM)
    {
        shader->setParent(this);
        emit shaderCreated(shader->getShader());
    }

    qDebug() << "Loaded vertex shaders from project file: " << getVertexFilePaths();

    foreach(MetaShader* shader, fragmentM)
    {
        shader->setParent(this);
        emit shaderCreated(shader->getShader());
    }

    qDebug() << "Loaded fragment shaders from project file: " << getFragmentFilePaths();

    in >> programs;

    foreach(MetaShaderProg* prog, programs)
    {
        prog->setProjectBaseDir(*baseDir);
        prog->setParent(this);
        connect(prog,SIGNAL(nameChanged(QString,QString)),this,SLOT(progNameChanged()));
    }

    in >> defaultProgram;

    if(defaultProgram.isEmpty() && !programs.isEmpty())
    {
        defaultProgram = programs.values().value(0)->getName();
    }

    qDebug() << "Loaded shader programs from project file" << programs.keys();


    in >> uniformVariables;

    in >> textures;

    foreach(TextureStorage* s, textures)
    {
        s->setParent(this);
        //s->setBasePath(basePath);
        if(!s->convertPathToAbsolute(basePath))
        {
            QMessageBox box;
            box.setText(tr("Texture file %1 for texture '%2' did not exists!").arg(s->getPath(),s->getName()));
            box.setIcon(QMessageBox::Warning);
            box.exec();

            textures.remove(s->getName());
        }
        else
            connect(s, SIGNAL(nameChanged(QString)),this,SLOT(texNameChanged()));
    }

    bool loaded;
    in >> loaded;

    if(loaded)
    {
        QString path;
        in >> path;

        path = pathToAbsolute(path);

        activeModel = new Model(this,this->getName());

        if(!activeModel->setFilePath(path))
        {
            QMessageBox box;
            box.setText(tr("Model file %1 did not exists!").arg(path));
            box.setIcon(QMessageBox::Warning);
            box.exec();

            delete activeModel;
            activeModel = NULL;
        }
        else
        {
            activeModel->loadModel();
            QHash<QPair<uint,uint>,QString> attachProgs;
            in >> attachProgs;

            if(!attachProgs.isEmpty())
                activeModel->setShaderPrograms(attachProgs, defaultProgram);
        }
    }

    qDebug() << QString("Project %1 loaded").arg(projectName);



    /*
    if(!programs.isEmpty())
    {
        QString key = programs.keys()[0];
        static_cast<MetaShaderProg*>(programs.value(key))->setProjectBaseDir(*baseDir);
        defaultProgram = programs.keys()[0];
    }
    */
}

/**
 * @brief MetaProject::postLoadProject Complete loading of project, when project is set to info manager.
 */
void MetaProject::postLoadProject()
{
    /*
    if(isModelLoaded())
        activeModel->loadModel();

    if(activeModel == NULL)
        return;

    if(!activeModel->isLoaded())
    {
        delete activeModel;
        activeModel = NULL;
    }
    */
}

/**
 * @brief MetaProject::getVertexFilesPath get vertex file paths from MetaShaders
 * @return file paths of all vertex shaders from this project
 */
QStringList MetaProject::getVertexFilePaths()
{
    QStringList ret;
    QHashIterator<QString,MetaShader*> it(vertexM);

    while(it.hasNext())
    {
        it.next();
        //ret.append(QDir::toNativeSeparators(baseDir->absolutePath()) + QDir::separator() + it.key());
        ret.append(baseDir->absolutePath() + '/' + it.key());
    }

    qDebug() << ret;

    return ret;
}

/**
 * @brief MetaProject::getFragmentFilesPath Get fragment file paths from MetaShaders.
 * @return File paths of all fragment shaders from this project.
 */
QStringList MetaProject::getFragmentFilePaths()
{
    QStringList ret;
    QHashIterator<QString,MetaShader*> it(fragmentM);

    while(it.hasNext())
    {
        it.next();
        //ret.append(QDir::toNativeSeparators(baseDir->absolutePath()) + QDir::separator() + it.key());
        ret.append(baseDir->absolutePath() + '/' + it.key());
    }

    return ret;
}

/**
 * @brief MetaProject::getVertexFilePath Get absolute vertex file path.
 * @param name Name of the vertex shader.
 * @return Return absolute file path.
 */
QString MetaProject::getVertexFilePath(QString name)
{
    if(!vertexM.contains(name))
        return QString("");

    //QString path = QDir::toNativeSeparators(baseDir->absolutePath()) + QDir::separator() + vertexM.value(name)->getShader();
    QString path = baseDir->absolutePath() + '/' + vertexM.value(name)->getShader();
    return path;
}

/**
 * @brief MetaProject::getFragmentFilePath Get absolute fragment file path.
 * @param name Name of the fragment shader.
 * @return Return absolute file path.
 */
QString MetaProject::getFragmentFilePath(QString name)
{
    if(!fragmentM.contains(name))
        return QString("");

    //QString path = QDir::toNativeSeparators(baseDir->absolutePath()) + QDir::separator() + fragmentM.value(name)->getShader();
    QString path = baseDir->absolutePath() + '/' + fragmentM.value(name)->getShader();
    return path;
}

/**
 * @brief MetaProject::getVertexRelativePaths Get paths of all vertex shaders in this project, relatively from base project directory.
 * @return List of vertex shader paths.
 */
QStringList MetaProject::getVertexRelativePaths()
{
    return vertexM.keys();
}

/**
 * @brief MetaProject::getFragmentRelativePaths Get paths of all fragment shaders in this project, relatively from base project directory.
 * @return List of fragment shader paths.
 */
QStringList MetaProject::getFragmentRelativePaths()
{
    return fragmentM.keys();
}

/**
 * @brief MetaProject::getVertexFileNames get vertex file names from MetaShaders
 * @return file names of all vertex shaders from this project
 */
QStringList MetaProject::getVertexFileNames()
{
    QStringList ret;
    QHashIterator<QString,MetaShader*> it(vertexM);

    while(it.hasNext())
    {
        it.next();
        ret.append(static_cast<MetaShader*>(it.value())->getShaderName());
    }

    return ret;
}

/**
 * @brief MetaProject::getFramgnetFileNames get fragment file names from MetaShaders
 * @return file names of all fragment shaders from this project
 */
QStringList MetaProject::getFragmentFileNames()
{
    QStringList ret;
    QHashIterator<QString,MetaShader*> it(fragmentM);

    while(it.hasNext())
    {
        it.next();
        ret.append(static_cast<MetaShader*>(it.value())->getShaderName());
    }

    return ret;
}

/**
 * @brief MetaProject::getVertexShader Get vertex shader with given name.
 * @param name Relative path to shader. Relative from project base directory.
 * @return Return vertex shader with given name.
 */
MetaShader *MetaProject::getVertexShader(QString name)
{
    return vertexM.value(name);
}

/**
 * @brief MetaProject::getVertexShaders Get vertex shaders.
 * @return All vertex shaders from this project.
 */
QList<MetaShader *> MetaProject::getVertexShaders()
{
    return vertexM.values();
}

/**
 * @brief MetaProject::getFragmentShader Get fragment shader with given name.
 * @param name Relative path to shadeer. Relative from project base directory.
 * @return Return fragment shader with given name.
 */
MetaShader *MetaProject::getFragmentShader(QString name)
{
    return fragmentM.value(name);
}

/**
 * @brief MetaProject::getFragmentShaders Get fragment shaders.
 * @return All fragment shaders from this project.
 */
QList<MetaShader *> MetaProject::getFragmentShaders()
{
    return fragmentM.values();
}

/**
 * @brief MetaProject::getShader Get shader with given name.
 * Vertex and fragment shader must be unique so we look for shader to these two categories.
 * @param name Name of the shader.
 * @return Return shader object or NULL, if given shader does not exists.
 */
MetaShader *MetaProject::getShader(QString name)
{
    MetaShader* sh = fragmentM.value(name,NULL);

    if(sh == NULL)
        sh = vertexM.value(name,NULL);

    return sh;
}

/**
 * @brief MetaProject::removeShader Remove shader from this project.
 * @param name Name of the shader (relative path from project).
 * @param removeFile If true remove shader file too, else remove only from project.
 * @return True if shader was removed, false otherwise.
 */
bool MetaProject::removeShader(QString name, bool removeFile)
{
    MetaShader* sh = NULL;
    bool isVertex = false;

    if(vertexM.contains(name))
    {
        sh = vertexM.value(name);
        isVertex = true;
    }
    else if(fragmentM.contains(name))
    {
        sh = fragmentM.value(name);
        isVertex = false;
    }

    if(sh == NULL)
        return false;

    if(removeFile)
    {
        QFile f(pathToAbsolute(name));

        if(f.exists())
            f.remove();
    }

    if(isVertex)
    {
        vertexM.remove(name);
        emit shaderDestroyed(name, MetaShader::VERTEX);
    }
    else
    {
        fragmentM.remove(name);
        emit shaderDestroyed(name, MetaShader::FRAGMENT);
    }

    delete sh;
    return true;
}

/**
 * @brief MetaProject::addProgram Create shader program with standard name
 * @param name Name of the shader program
 * @return False if this name is used already, true if shader program was created.
 */
bool MetaProject::addProgram(QString name)
{
    if(programs.contains(name))
        return false;

    if(name.isEmpty())
    {
        QString name = QString("prog%1").arg(programs.count());
        programs[name] = new MetaShaderProg(name,*baseDir,this);
    }
    else
        programs[name] = new MetaShaderProg(name,*baseDir,this);

    if(defaultProgram.isEmpty())
        defaultProgram = name;

    connect(programs[name],SIGNAL(nameChanged(QString,QString)),this,SLOT(progNameChanged()));

    emit shProgCreated(name);

    return true;
}

/**
 * @brief MetaProject::removeProgram Remove shader program from this project.
 * @param name Name of the shader in this project.
 * @return False if shader program is not in this project, true otherwise.
 */
bool MetaProject::removeProgram(QString name)
{
    bool contains = programs.contains(name);

    programs.remove(name);

    if(contains)
        emit shProgDestroyed(name);

    return contains;
}

/**
 * @brief MetaProject::getProgramNames Get names for all shader programs in this project
 * @return Names of shader programs
 */
QStringList MetaProject::getProgramNames()
{
    QStringList ret;

    QHashIterator<QString,MetaShaderProg*> it(programs);

    while(it.hasNext())
    {
        it.next();
        ret << it.key();
    }

    return ret;
}

/**
 * @brief MetaProject::setShaderToProgram Set shader with given path to shader program
 * @param shaderPath Path to the given shader (unique id)
 * @param programName Program name (unique id)
 * @return True if shader and program exists, false otherwise
 */
bool MetaProject::setShaderToProgram(QString shaderPath, const QString programName)
{
    //program doesn't exists
    if(!programs.contains(programName))
        return false;

    shaderPath = pathToRelative(shaderPath);

    MetaShader* sh = NULL;

    if(vertexM.contains(shaderPath))
        sh = vertexM[shaderPath];
    else if(fragmentM.contains(shaderPath))
        sh = fragmentM[shaderPath];

    //shader doesn't exists
    if(sh == NULL)
        return false;

    //add shader to program
    if(sh->getType() == MetaShader::VERTEX)
        programs[programName]->setVertexShader(shaderPath);
    else if(sh->getType() == MetaShader::FRAGMENT)
        programs[programName]->setFragmentShader(shaderPath);

    return true;
}

/**
 * @brief MetaProject::getAttachedPrograms Get shader programs attached to this shader
 * @param shaderPath Relative path to shader
 * @return Names of shader programs attached to this shader
 */
QStringList MetaProject::getAttachedPrograms(QString shaderPath)
{
    QStringList ret;

    foreach(MetaShaderProg* prog, programs.values())
    {
        foreach(QString sh, prog->getShaders())
        {
            if(sh == shaderPath)
            {
                ret << prog->getName();
                break;
            }
        }
    }

    return ret;
}

/**
 * @brief MetaProject::attachedShaders Get attached shaders to program
 * @param programName Name of the shader program
 * @return Path of attached shaders
 */
QStringList MetaProject::getAttachedShaders(QString programName)
{
    if(programName.isEmpty())
        return static_cast<MetaShaderProg*>(programs.value(defaultProgram))->getShaders();

    return static_cast<MetaShaderProg*>(programs.value(programName))->getShaders();
}

/**
 * @brief MetaProject::getProgram Return shader program object
 * @param name Name of the shader program, if not set default program is used instead
 * @return Shader program
 */
MetaShaderProg* MetaProject::getProgram(QString name)
{
    if(programs.isEmpty())
        return NULL;

    if(name.isEmpty())
        return programs[defaultProgram];

    return programs[name];
}

/**
 * @brief MetaProject::addUniformVariable Add uniform variable to project
 * @param uVar Uniform variable
 */
bool MetaProject::addUniformVariable(UniformVariable* uVar)
{
    if(uniformVariables.contains(uVar->getName()))
        return false;

    uniformVariables.insert(uVar->getName(),uVar);
    emit uniformCreated(uVar->getName());
    return true;
}

/**
 * @brief MetaProject::removeUniformVariable Remove uniform variable from project
 * @param name Name of the uniform variable
 */
void MetaProject::removeUniformVariable(QString name)
{
    UniformVariable* var = uniformVariables.value(name, NULL);

    if(var == NULL)
        return;

    uniformVariables.remove(name);

    delete var;

    emit uniformDestroyed(name);
}

/**
 * @brief MetaProject::getUniformVariable Return uniform variable with given name
 * @param name Name of the variable
 * @return Uniform variable or if the name exists return NULL
 */
UniformVariable* MetaProject::getUniformVariable(const QString name)
{
    return uniformVariables.value(name);
}

/**
 * @brief MetaProject::getUniformNames Return names of all uniform variables in this project
 * @return Uniform variable names
 */
QStringList MetaProject::getUniformNames()
{
    QStringList list;

    QHashIterator<QString,UniformVariable*> it(uniformVariables);

    while(it.hasNext())
    {
        it.next();

        list.append(it.key());
    }

    return list;
}

/**
 * @brief MetaProject::getUniformVariables Return all uniform variables in this project
 * @return List of uniform variable object
 */
QList<UniformVariable *> MetaProject::getUniformVariables()
{
    QList<UniformVariable*> ret;

    QHashIterator<QString,UniformVariable*> it(uniformVariables);

    while(it.hasNext())
    {
        it.next();
        ret.append(it.value());
    }

    return ret;
}

/**
 * @brief MetaProject::getUniformVariables Return list of uniform variables attached to a given program
 * @param programName Name of the shader program
 * @return List of UniformVariable classes
 */
QList<UniformVariable *> MetaProject::getUniformVariables(QString programName)
{
    QList<UniformVariable*> ret;

    QHashIterator<QString,UniformVariable*> it(uniformVariables);

    while(it.hasNext())
    {
        it.next();
        if(!(it.value()->getAttachedVariables(programName).isEmpty()))
            ret.append(it.value());
    }

    return ret;
}

/**
 * @brief MetaProject::setModel Set loaded model to this project as active
 * @param model Object with all information about 3d model
 */
void MetaProject::setModel(Model *model)
{
    if(!model)
        return;

    if(activeModel != NULL)
    {
        qDebug() << "Deleting active model" << activeModel->getFilePath();
        delete activeModel;
    }

    activeModel = model;

    emit modelLoaded();
}

/**
 * @brief MetaProject::getModel Get model for this project
 * @return Model object
 */
Model *MetaProject::getModel()
{
    return activeModel;
}

/**
 * @brief MetaProject::isModelLoaded Test if any model is loaded to this project
 * @return True if model is loaded otherwise false
 */
bool MetaProject::isModelLoaded()
{
    return (activeModel == NULL) ? false : true;
}

/**
 * @brief MetaProject::getTextureNameList Get names of all textures
 * @return String list of all texture names
 */
QStringList MetaProject::getTextureNameList()
{
    QList<QString> list = textures.keys();
    QStringList strList;

    foreach(QString name, list)
        strList.append(name);

    return strList;
}

/**
 * @brief MetaProject::getTexture Get texture with given name
 * @param name Name of the texture
 * @return Return texture
 */
TextureStorage *MetaProject::getTexture(QString name)
{
    return textures.value(name, NULL);
}

/**
 * @brief MetaProject::addTexture Add new texture to texture list. Emit signal textureCreated.
 * @param texture Texture object to add
 * @return True if texture was added or false if texture with this name exists
 */
bool MetaProject::addTexture(TextureStorage *texture)
{
    if(textures.contains(texture->getName()))
        return false;

    texture->setParent(this);

    textures.insert(texture->getName(), texture);
    connect(texture, SIGNAL(nameChanged(QString)),this,SLOT(texNameChanged()));

    emit textureCreated(texture->getName());
    return true;
}

/**
 * @brief MetaProject::hasTexture Test if this texture exists
 * @param name Name of the texture we are looking for
 * @return True if texture with this name exists, false otherwise
 */
bool MetaProject::hasTexture(QString name)
{
    return textures.contains(name);
}

/**
 * @brief MetaProject::hasFragmentShader Test if given fragment shader is in this project.
 * @param name Name of the tested fragment shader.
 * @return True if shader exists here, false otherwise.
 */
bool MetaProject::hasFragmentShader(QString name) const
{
    return fragmentM.contains(name);
}

/**
 * @brief MetaProject::hasVertexShader Test if given vertex shader is in this project.
 * @param name Name of the tested vertex shader.
 * @return True if shader exists here, false otherwise
 */
bool MetaProject::hasVertexShader(QString name) const
{
    return vertexM.contains(name);
}

/**
 * @brief MetaProject::hasUniformVariable Test if given uniform variable is in this project.
 * @param name Name of the tested uniform variable.
 * @return True if uniform variable exists here, false otherwise.
 */
bool MetaProject::hasUniformVariable(QString name) const
{
    return uniformVariables.contains(name);
}

/**
 * @brief MetaProject::getSettings Get settings for OpenGL.
 * @return Return settings storage.
 */
SettingsStorage *MetaProject::getSettings() const
{
    return settings;
}

/**
 * @brief MetaProject::setSettings Set OpenGL settings.
 * @param settings New settings for OpenGL.
 */
void MetaProject::setSettings(SettingsStorage *settings)
{
    if(settings != NULL)
        delete settings;

    this->settings = settings;
}

/**
 * @brief MetaProject::pathToRelative Change path to relative to project base directory, if it is not already.
 * @param path Path to a shader
 * @return Relative path to a shader (from project base directory)
 */
QString MetaProject::pathToRelative(QString path)
{
    // if path is not relative to project directory, then change it
    if(!QFileInfo(path).isRelative())
    {
        path = baseDir->relativeFilePath(QFileInfo(path).absoluteFilePath());
    }

    return path;
}

/**
 * @brief MetaProject::pathToAbsolute Get absolute path from given relative path against this project.
 * @param path Relative path from base project.
 * @return Return absolute path of the given path.
 */
QString MetaProject::pathToAbsolute(QString path)
{
    if(path.isEmpty())
        return path;

    qDebug() << path;

    if(QFileInfo(path).isRelative())
    {
        //path = baseDir->absolutePath() + QDir::separator() + path;
        //QDir(path).absoluteFilePath();
        path = baseDir->absoluteFilePath(path);
    }

    return path;
}

/**
 * @brief MetaProject::removeProgramsNull Remove null pointer items before save to project file.
 */
void MetaProject::removeProgramsNull()
{
    QHash<QString,MetaShaderProg*> newProgs;

    foreach(MetaShaderProg* p, programs)
    {
        if(p != NULL)
            newProgs.insert(p->getName(),p);
    }

    programs.clear();
    programs = newProgs;
}

/**
 * @brief MetaProject::texNameChanged Refresh texture hash names
 */
void MetaProject::texNameChanged()
{
    QHash<QString,TextureStorage*> newHash;

    foreach(TextureStorage* tex, textures.values())
    {
        newHash.insert(tex->getName(),tex);
    }

    textures = newHash;
}

/**
 * @brief MetaProject::progNameChanged Refresh shader program hash names
 */
void MetaProject::progNameChanged()
{
    QHash<QString,MetaShaderProg*> newHash;

    foreach(MetaShaderProg* prog, programs.values())
    {
        newHash.insert(prog->getName(), prog);
    }

    programs = newHash;
}

/**
 * @brief MetaProject::setModelAttachment Attach shader programs to model.
 */
void MetaProject::setModelAttachment()
{
    if(!isModelLoaded())
        return;

    if(programs.isEmpty())
        return;

    if(activeModel->getAttachedPrograms().isEmpty())
    {
        MetaShaderProg* prog = programs.values().value(0);
        QSet<ModelNode*> nodes = activeModel->getNodes();
        QString progName = prog->getName();

        foreach(ModelNode* node, nodes.values())
        {
            node->setShProgramToAll(progName);
        }
    }
}
