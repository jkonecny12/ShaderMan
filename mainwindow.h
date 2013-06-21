#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include "oglwindow.h"
#include "infomanager.h"
#include "dialogs/newfile/newfiledialog.h"
#include "dialogs/new_project/newprojectdialog.h"
#include "uniform/dialogs/uniformvariableeditor.h"
#include "uniform/dialogs/createuniformvariables.h"
#include "model_work/dialogs/modelattachmentdialog.h"
#include "texture/dialogs/texturedialog.h"
#include "dialogs/program_manager/programmanagerdialog.h"
#include "dialogs/filetype/filetypedialog.h"

class QFileDialog;
class QDir;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void connectSignals();
    bool isProjectActive(const QString proj = "", bool printWarnings = true);
    
private slots:
    void openFile();
    void createNewFile();
    void createNewProject();
    void openFromTreeView(QModelIndex index);
    void hideShowLog();
    void showSetUniformVarDialog();
    void showCreateUniformDialog();
    void showLoadModelDialog();
    void showAttachModelDialog();
    void showProgramDialog();
    void showProjectSettings();

    void refreshActiveProject();
    void connectCreatedProject(QString name);

    void projectRemoved(QString name);
    void changeActiveProject(QString newName, QString oldName);

    void showTextureDialog();

    void showMeasureDockWidget();

    void buildShader();
    void removeShader();

    void shaderRemoved(QString name);
    void connectShaders(QString name);

signals:
    void reloadModel();
    void reloadProgramSettings();
    void reloadTextures();
    void reloadUniformVariables();
    void pauseDrawing(bool pause);
    void newOpenGLSettings();

private:
    Ui::MainWindow *ui;
    InfoManager* infoM;

};

#endif // MAINWINDOW_H
