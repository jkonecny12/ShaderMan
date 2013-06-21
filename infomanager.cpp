#include "infomanager.h"

/**
  Defines static parts of class
  */
InfoManager* InfoManager::m_Instance;
QMutex InfoManager::mutex;

//----------------------

/** SINGLETON PART **/

/**
  Private constructor
  */
InfoManager::InfoManager() : QObject()
{
    activeProject = "";

    connect(this,SIGNAL(projectCreated(QString)),this,SLOT(blockProjectSignals(QString)));
}

/**
  Create or return instance of singleton
  */
InfoManager* InfoManager::getInstance()
{
    if(!m_Instance)
    {
        mutex.lock();

        if(!m_Instance)
            m_Instance = new InfoManager();

        mutex.unlock();
    }

    return m_Instance;
}

/**
  Drop singleton
  */
void InfoManager::dropInstance()
{
    mutex.lock();
    delete m_Instance;
    m_Instance = NULL;
    mutex.unlock();
}

/**
 * @brief InfoManager::isActiveProject Is there active project
 * @return True if active project is set, false otherwise
 */
bool InfoManager::isActiveProject()
{
    return(!activeProject.isEmpty());
}


/** WORKING PART **/

/**
 * @brief InfoManager::addProject Create project in this manager
 * @param name Name of the project
 * @param baseDir Path to given project
 * @return Return false if manager contains this project, true otherwise
 */
bool InfoManager::addProject(QString name, QDir baseDir)
{
    if(projects.contains(name))
        return false;

    MetaProject* proj = new MetaProject(name, baseDir);

    projects[name] = proj;
    emit projectCreated(name);

    if(activeProject.isEmpty())
        setActiveProject(name);

    connect(proj,SIGNAL(shaderCreated(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(shaderDestroyed(QString,MetaShader::SHADERTYPE)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(shProgCreated(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(shaderDestroyed(QString,MetaShader::SHADERTYPE)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(textureCreated(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(textureDestroyed(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(uniformCreated(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(uniformDestroyed(QString)),this,SLOT(projectChanged(QString)));
    connect(proj,SIGNAL(modelLoaded()),this,SLOT(projectChanged()));

    return true;
}

/**
 * @brief InfoManager::addProject Add project to this manager
 * @param project Pointer to given project class (manager will destroy this project in destructor)
 * @return Return false if manager contains this project, true otherwise
 */
bool InfoManager::addProject(MetaProject *project)
{
    if(projects.contains(project->getName()))
        return false;

    projects.insert(project->getName(), project);

    emit projectCreated(project->getName());

    if(activeProject.isEmpty())
        setActiveProject(project->getName());
    return true;
}

/**
 * @brief InfoManager::setActiveProject Set other project as active and emit signal about this change.
 * @param name Name of new active project.
 */
void InfoManager::setActiveProject(QString name)
{
    if(activeProject.isEmpty())
    {
        activeProject = name;
        emit defaultProjectChanged(name,QString());
    }
    else if(activeProject != name)
    {
        QString old = activeProject;

        activeProject = name;
        emit defaultProjectChanged(name, old);
    }
}

/**
  Add shaders meta information to this manager
  */
void InfoManager::addShader(QString path, InfoManager::SHADERTYPE type, QString projectName)
{
    QString project;

    if(projectName.isEmpty())
        project = activeProject;
    else
        project = projectName;

    Q_ASSERT(projects.contains(project));

    if(!projects.contains(project))
        return;

    switch(type)
    {
    case InfoManager::VERTEX:
        projects[project]->addVertex(path);
        break;

    case InfoManager::FRAGMENT:
        projects[project]->addFragment(path);
        break;

    default:
        break;
    }

    // save changes to project file
    saveProject(projects[project]->getName());

}

/**
  Remove all shaders from program
  */
void InfoManager::deleteAll()
{
    qDeleteAll(projects);
}

/**
  Save active project to .sm file in project base directory
  projectName - if set save project with given name
                if not set save active project
  */
bool InfoManager::saveProject(QString projectName)
{
    if(projectName.isEmpty())
        projectName = activeProject;

    //QFile file(projects[projectName]->getProjAbsolutePath() + QDir::separator() +
    //           QDir::separator() + projectName + ".sm");

    QFile file(projects[projectName]->getProjAbsolutePath() + '/' +
               '/' + projectName + ".sm");

    if(!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);

    out.setVersion(QDataStream::Qt_4_8);

    out << magicNumber;
    // identify program with his version
    out << versionMajorNumber;
    out << versionMinorNumber;

    projects[projectName]->saveProject(out);

    file.close();

    qDebug() << "Project" << projectName << "saved";

    return true;
}

/**
  Load active project from .sm file in project base directory
  projectPath - path to project file
  */
bool InfoManager::loadProject(QString projectPath)
{
    QFile file(projectPath);

    if(!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream in(&file);

    in.setVersion(QDataStream::Qt_4_8);

    quint32 magicN;

    in >> magicN;

    if(magicN != magicNumber)
    {
        file.close();
        return false;
    }

    qint32 versionMaj;
    qint32 versionMin;
    in >> versionMaj;
    in >> versionMin;

    // project part
    MetaProject* proj = new MetaProject();

    QDir dir(projectPath);
    dir.cdUp();
    proj->loadProject(in, dir.absolutePath(), versionMaj, versionMin);

    addProject(proj);

    proj->postLoadProject();

    /*
    if(projects.contains(proj->getProjName()))
    {
        file.close();
        return false;
    }

    projects[proj->getProjName()] = proj;
    */

    file.close();

    qDebug() << "Project" << proj->getName() << "loaded";

    return true;
}

/**
 * @brief InfoManager::getProjectBaseDir Get project base directory, if no name set then get active project base directory.
 * @param projectName Name of the project, if not set active project will be used.
 * @return Project base directory.
 */
QString InfoManager::getProjectBaseDir(QString projectName)
{
    if(projectName.isEmpty())
        return(projects[activeProject]->getProjectDirectory().absolutePath());
    else
        return(projects.value(projectName)->getProjectDirectory().absolutePath());
}

/**
 * @brief InfoManager::getProgramNames Return names of project programs
 * @param project Name of project
 * @return Program names
 */
QStringList InfoManager::getProgramNames(QString project)
{
    QString proj;

    if(project.isEmpty())
        proj = activeProject;

    return projects[proj]->getProgramNames();
}

/**
 * @brief InfoManager::getProgram Return shader program with given name
 * @param name Name of the shader program
 * @return Return shader program
 */
MetaShaderProg* InfoManager::getProgram(QString name)
{
    if(projects.isEmpty())
        return NULL;

    return projects[activeProject]->getProgram(name);
}

/**
 * @brief InfoManager::createProgram Create shader program with given shader program name to given project.
 * @param name Name of the shader program
 * @param project Name of the project
 */
void InfoManager::createProgram(QString name, QString project)
{
    if(project.isEmpty())
        project = activeProject;

    projects[project]->addProgram(name);

    saveProject();
}

/**
 * @brief InfoManager::attachShaderToProgram Attach shader to shader program within given project
 * @param shader Name of the shader
 * @param programName Name of the shader program
 * @param project Name of the project. If project is not set then use active project.
 */
void InfoManager::attachShaderToProgram(QString shader, QString programName, QString project)
{
    if(project.isEmpty())
        project = activeProject;

    projects[activeProject]->setShaderToProgram(shader,programName);

    saveProject();
}

/**
 * @brief InfoManager::getActiveProject Return active project object
 * @return Project object or null if not exists
 */
MetaProject *InfoManager::getActiveProject()
{
    if(activeProject.isEmpty())
        return NULL;

    return projects.value(activeProject,NULL);
}

/**
 * @brief InfoManager::getProject Return project object with given name
 * @param name Name of the project
 * @return Project objector null if not exists
 */
MetaProject *InfoManager::getProject(QString name)
{
    return projects.value(name);
}

/**
 * @brief InfoManager::getProjectNames Get names of all projects.
 * @return Names of projects.
 */
QStringList InfoManager::getProjectNames() const
{
    return projects.keys();
}

/**
 * @brief InfoManager::getProjects Get all loaded projects.
 * @return Project objects.
 */
QList<MetaProject *> InfoManager::getProjects() const
{
    return projects.values();
}

/**
 * @brief InfoManager::closeProject Close project given project if exists. If existing is active, set as active something else.
 * @param name Name of the project we want to close.
 * @return True if project was closed, false if project do not exists.
 */
bool InfoManager::closeProject(QString name)
{
    if(!projects.contains(name))
        return false;


    if(activeProject == name)
        setActiveProject("");

    MetaProject* proj = projects.take(name);

    emit projectRemoved(name);
    delete proj;

    /*
    if(projects.isEmpty())
        return true;
    */

    //setActiveProject(projects.values().first()->getName());

    return true;
}

/**
 * @brief InfoManager::projectChanged Slot for saving project when changed.
 */
void InfoManager::projectChanged(QString /*projectName*/)
{
    MetaProject* proj = dynamic_cast<MetaProject*>(sender());

    if(proj == NULL)
        return;

    saveProject(proj->getName());
}

/**
 * @brief InfoManager::blockProjectSignals Block signals from all other metaprojects and unblock projects from given project
 * @param name Name of the project which now will be set as active
 */
void InfoManager::blockProjectSignals(QString name)
{
    foreach(MetaProject* project, projects)
    {
        if(project->getName() != name)
        {
            project->blockSignals(true);
        }
        else
            project->blockSignals(false);
    }
}
