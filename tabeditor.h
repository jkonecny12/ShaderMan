#ifndef TABEDITOR_H
#define TABEDITOR_H

#include <QTabWidget>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "codeeditor.h"
#include "infomanager.h"
//#include "scieditor.h"

class QFileDialog;
class QDir;
class QDebug;
class QFile;


class TabEditor : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabEditor(QWidget *parent = 0);
    void newDocument(QString path, InfoManager::SHADERTYPE type);
    void openFile(QString path, InfoManager::SHADERTYPE type);
    void saveAll();

    bool testModified();

    QString getOpenedPath();

private:
    void customConnect();
    bool isOpened(QString path);
    bool activateTab(QString path);
    int findIndex(QString path);
    
signals:
    
public slots:
    void closeTab(int index);
    void closeTab(QString path, MetaProject *project = NULL);
    //void openFiles(QStringList list);
    void saveFile();
    void saveAsFile();
    void fileModified();

private:
    QList<CodeEditor*> activeFiles;
    InfoManager* infoManager;
    
};

#endif // TABEDITOR_H
