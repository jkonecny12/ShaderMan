#ifndef INFOMANAGER_H
#define INFOMANAGER_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QFile>
#include <QFileInfo>
#include <QStandardItem>
#include <QtAlgorithms>
#include <QDebug>
#include <QMutex>
#include <QDataStream>
#include "meta_data/metaproject.h"

/**
 * @brief The InfoManager class Singleton class representing all information in this program.
 * Contains projects.
 */
class InfoManager : public QObject
{
    Q_OBJECT

// Singleton part of the object
private:
    InfoManager(); // hide constructor
    InfoManager(const InfoManager &); // hide copy constructor
    InfoManager& operator=(const InfoManager &); // hide assign op
    static InfoManager* m_Instance;
    static QMutex mutex;

public:
    static InfoManager* getInstance();
    static void dropInstance();

// Effective part of class
public:
    enum SHADERTYPE {VERTEX = 0, FRAGMENT = 1, TEXT}; // type of the Shader
    bool isActiveProject();  
    void addShader(QString path, InfoManager::SHADERTYPE type, QString projectName = "");
    void deleteAll();
    //void addPorjectToTV(QStandardItem* root, QString name = ""); // add project to TreeView
    bool saveProject(QString projectName = ""); // save project
    bool loadProject(QString projectPath); // load project
    QString getProjectBaseDir(QString projectName = "");
    //QString getActiveShader(SHADERTYPE type);

    //working with shader programs
    QStringList getProgramNames(QString project = "");
    MetaShaderProg *getProgram(QString name);
    void createProgram(QString name, QString project = "");
    void attachShaderToProgram(QString shader, QString programName, QString project = "");

    //work with project object
    bool addProject(QString name, QDir baseDir);
    bool addProject(MetaProject* project);
    void setActiveProject(QString name);
    MetaProject* getActiveProject();
    MetaProject* getProject(QString name);

    QStringList getProjectNames() const;
    QList<MetaProject*> getProjects() const;
    bool closeProject(QString name);

public slots:
    void projectChanged(QString projectName);

protected:
    const static quint32 magicNumber = 0xC56EE8F8;
    const static qint32 versionMajorNumber = 0;
    const static qint32 versionMinorNumber = 1;

private:
    QHash<QString,MetaProject*> projects;
    QString activeProject;

signals:
    void defaultProjectChanged(QString newProject, QString oldProject);
    void projectCreated(QString name);
    void projectRemoved(QString name);

private slots:
    void blockProjectSignals(QString name);
};

#endif // INFOMANAGER_H
