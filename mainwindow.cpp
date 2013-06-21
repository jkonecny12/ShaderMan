#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "storage/projecttreeitem.h"
#include "project_settings/dialog/openglsettingsdialog.h"

/**
 * @brief MainWindow::MainWindow Create main window of application
 * @param parent Parent of this widget
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    // set logger to OGLwindow and create OpenGL window
    ui->GL_Window_underlay->createOpenGLWindow(ui->logViewer);

    // hide show log button - log is up
    ui->showButtonLayout->hide();
    ui->dockWidget->setVisible(false);

    infoM = InfoManager::getInstance();

    ui->dockWidgetContents->setGLWindow(ui->GL_Window_underlay->returnOGLwindow());

    connectSignals();
}

/**
 * @brief MainWindow::~MainWindow Destroy this widget
 */
MainWindow::~MainWindow()
{
    delete ui;
    infoM->deleteAll(); // delete all meta shaders data
    infoM->dropInstance();
}

/**
 * @brief MainWindow::closeEvent Test if all opened files are saved, if not ask user about this.
 * @param event Close event, we can accept or ignore it.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(ui->tabEditorWindow->testModified())
    {
        event->accept();
    }
    else
    {
        QMessageBox box;
        box.setText(tr("Not all your files are saved. Do you want save save them?"));
        box.setInformativeText(tr("If you press No button, then all not saved changes will be lost!"));
        box.setIcon(QMessageBox::Question);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel | QMessageBox::SaveAll);
        box.setDefaultButton(QMessageBox::SaveAll);

        int ret = box.exec();

        if(ret == QMessageBox::Cancel)
            event->ignore();
        else if(ret == QMessageBox::Ok)
            event->accept();
        else if(ret == QMessageBox::SaveAll)
        {
            ui->tabEditorWindow->saveAll();
            event->accept();
        }
    }
}

/**
  Connect custom slots and signals
  */
void MainWindow::connectSignals()
{
    connect(infoM,SIGNAL(projectCreated(QString)),this,SLOT(connectShaders(QString)));

    // open dialogs
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(createNewFile()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(ui->actionNew_Project,SIGNAL(triggered()),this,SLOT(createNewProject()));
    connect(ui->actionSet_Uniform_Variables,SIGNAL(triggered()),this,SLOT(showSetUniformVarDialog()));
    connect(ui->action_Create_Uniform_Variables,SIGNAL(triggered()),this,SLOT(showCreateUniformDialog()));
    connect(ui->action_LoadModel,SIGNAL(triggered()),this,SLOT(showLoadModelDialog()));
    connect(ui->action_Attach_model,SIGNAL(triggered()),this,SLOT(showAttachModelDialog()));
    connect(ui->action_TextureSettings,SIGNAL(triggered()),this,SLOT(showTextureDialog()));
    connect(ui->action_Program_manager,SIGNAL(triggered()),this,SLOT(showProgramDialog()));
    connect(ui->action_OpenGL_settings,SIGNAL(triggered()),this,SLOT(showProjectSettings()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(removeShader()));

    connect(ui->action_Show_draw_time_statistics,SIGNAL(triggered()),this,SLOT(showMeasureDockWidget()));

    //connect(this,SIGNAL(sendFileNames(QStringList)),ui->tabEditorWindow,SLOT(openFiles(QStringList)));
    connect(ui->action_Save,SIGNAL(triggered()),ui->tabEditorWindow,SLOT(saveFile()));
    connect(ui->actionSave_As,SIGNAL(triggered()),ui->tabEditorWindow,SLOT(saveAsFile()));

    connect(ui->actionBuild_Run,SIGNAL(triggered()),this,SLOT(buildShader()));
    connect(this,SIGNAL(reloadModel()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(loadNewModel()));
    connect(this,SIGNAL(reloadProgramSettings()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(reloadShaderPrograms()));
    connect(this,SIGNAL(reloadTextures()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(newTextures()));
    connect(this,SIGNAL(reloadUniformVariables()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(newUniformValues()));
    connect(this,SIGNAL(newOpenGLSettings()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(setOpenGLSettings()));
    connect(ui->treeView,SIGNAL(activated(QModelIndex)),this,SLOT(openFromTreeView(QModelIndex)));

    // build log connects
    connect(ui->hideLogButton,SIGNAL(clicked()),this,SLOT(hideShowLog()));
    connect(ui->showLogButton,SIGNAL(clicked()),this,SLOT(hideShowLog()));

    // connect for drawing time measure
    /*
    connect(ui->GL_Window_underlay->returnOGLwindow(),SIGNAL(newMeasure(double)),
            static_cast<ProfileWidget*>(ui->dockWidget->widget()),SLOT(addValue(double)));
    */
    connect(ui->GL_Window_underlay->returnOGLwindow(),SIGNAL(queryCreated(QString)),
            ui->dockWidgetContents,SLOT(setTimeList(QString)));
    connect(ui->GL_Window_underlay->returnOGLwindow(),SIGNAL(queryDestroyed(QString)),
            ui->dockWidgetContents,SLOT(queryDestroyed(QString)));

    connect(this,SIGNAL(pauseDrawing(bool)),ui->GL_Window_underlay->returnOGLwindow(),SLOT(pauseDrawing(bool)));

    connect(infoM,SIGNAL(projectCreated(QString)),this,SLOT(connectCreatedProject(QString)));
    connect(infoM,SIGNAL(projectRemoved(QString)),this,SLOT(projectRemoved(QString)));
    connect(infoM,SIGNAL(defaultProjectChanged(QString,QString)),this,SLOT(buildShader()));
    connect(infoM,SIGNAL(defaultProjectChanged(QString,QString)),this,SLOT(refreshActiveProject()));

//    connect(infoM->getActiveProject(),SIGNAL(shProgCreated(QString)),
//            static_cast<ProfileWidget*>(ui->dockWidget->widget()),SLOT(insertShProgram(QString)));

    // connect buttons to OpenGL window
    connect(ui->oglButton0,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton0,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton1,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton1,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton2,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton2,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton3,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton3,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton4,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton4,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton5,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton5,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton6,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton6,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton7,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton7,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->oglButton8,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->oglButton8,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    //arrow buttons
    connect(ui->xmButton9,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->xmButton9,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->xpButton10,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->xpButton10,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->ymButton11,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->ymButton11,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->ypButton12,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->ypButton12,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->zmButton13,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->zmButton13,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));

    connect(ui->zpButton14,SIGNAL(pressed()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonPressed()));
    connect(ui->zpButton14,SIGNAL(released()),ui->GL_Window_underlay->returnOGLwindow(),SLOT(buttonReleased()));


}

/**
 * @brief MainWindow::isProjectActive Test if project with name proj or active project if not set proj is created or set as active.
 * @param proj Name of the project for testing, if not set active project is use instead
 * @return True if project is created false otherwise
 */
bool MainWindow::isProjectActive(const QString proj, bool printWarnings)
{
    if(proj.isEmpty())
    {
        if(!infoM->isActiveProject())
        {
            if(printWarnings)
                QMessageBox::critical(this,tr("Project not created!"),
                                  tr("You must create project and set it as active before you can continue."));
            return false;
        }
    }
    else
    {
        if(!infoM->getProject(proj))
        {
            if(printWarnings)
                QMessageBox::critical(this,tr("Project not created!"),
                                  tr("Project with name $1 is not created or loaded.").arg(proj));
            return false;
        }
    }

    return true;
}

/** SLOTS **/

/**
  SLOT
  File Open Dialog for get shader file names or other type of files
  and emit signals to other objects
  */
void MainWindow::openFile()
{
    QString fName = QFileDialog::getOpenFileName(this,tr("Choose File"),QDir::homePath());

    // cancel button pressed
    if(fName.isEmpty())
        return;

    QFileInfo fInfo(fName);

    if(!fInfo.isFile())
        return;

    if(fInfo.suffix() == "sm")
    {
        if(!infoM->loadProject(fName))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Project canno't be loaded."),QMessageBox::Ok);
        }
        //infoM->addPorjectToTV(ui->treeView->getRootItem());
        return;
    }

    // conno't load files to not existing project
    if(!isProjectActive())
        return;

    FileTypeDialog typeDialog(this);

    typeDialog.exec();

    if(typeDialog.result() == FileTypeDialog::Accepted)
    {
        InfoManager::SHADERTYPE type = typeDialog.getFileType();
        MetaProject* actProj = infoM->getActiveProject();
        QString relPath = actProj->pathToRelative(fName);
        bool isOk = true;

        switch(type)
        {
        case InfoManager::VERTEX:
            isOk = actProj->addVertex(relPath);
            break;
        case InfoManager::FRAGMENT:
            isOk = actProj->addFragment(relPath);
            break;
        default:
            isOk = false;
            break;
        }

        if(!isOk)
        {
            QMessageBox box;
            box.setText(tr("File '%1' canno't be loaded!").arg(fName));
            box.setIcon(QMessageBox::Warning);
            box.exec();
        }
    }
}

/**
  SLOT
  Create new file (include shaders) dialog
  */
void MainWindow::createNewFile()
{

    if(!infoM->isActiveProject())
    {
        QMessageBox::warning(this,tr("Active Project not set"),tr("You canno't create new file without a project"));
        return;
    }

    NewFileDialog newFile(infoM->getProjectBaseDir(),this);

    newFile.exec();

    // dialog was accepted
    if(newFile.result() == QDialog::Accepted)
    {
        QString path = newFile.getPath();
        InfoManager::SHADERTYPE type = newFile.getType();


        infoM->addShader(path,type);
        ui->tabEditorWindow->newDocument(path,type);

        //emit refreshList();
        // add file to TreeView
    }

}

/**
  SLOT
  Create new project
  Must have one Vertex and Fragment shader
  */
void MainWindow::createNewProject()
{
    NewProjectDialog dialog(this);

    dialog.exec();

    if(dialog.result() == QDialog::Accepted)
    {
        QString projName = dialog.getProjectName();

        infoM->addProject(dialog.getProjectName(),dialog.getBaseDir());
        infoM->addShader(dialog.getVertName(), InfoManager::VERTEX, projName);
        infoM->addShader(dialog.getFragName(), InfoManager::FRAGMENT, projName);

        // add project and files to TreeView
        if(!infoM->saveProject(dialog.getProjectName()))
        {
            QMessageBox::warning(this,tr("Warning"),tr("Project canno't saved."),QMessageBox::Ok);
        }
        //infoM->addPorjectToTV(ui->treeView->getRootItem());
    }
}

/**
  SLOT
  Open editor window when double clicked on TreeView
  */
void MainWindow::openFromTreeView(QModelIndex index)
{
    // get file path to shader from Qt::UserRole
    // get shader type to shader from Qt::UserRole+1
    QVariant var = index.data(Qt::UserRole);

    // user clicked on file to open it
    if(var.isValid() && var.toInt() == ProjectTreeItem::SHADER)
    {
        QVariant shName = index.data(Qt::UserRole+1);
        QVariant projName = index.data(Qt::UserRole+2);

        qDebug() << "Double clicked in TreeView: " << shName.toString();

        MetaProject* proj = infoM->getProject(projName.toString());
        MetaShader* sh = proj->getShader(shName.toString());
        QString path = proj->pathToAbsolute(sh->getShader());

        ui->tabEditorWindow->openFile(path,InfoManager::SHADERTYPE(sh->getType()));
    }

}

/**
  SLOT
  Hide or show logViewer window
  */
void MainWindow::hideShowLog()
{
    if(ui->showButtonLayout->isHidden())
    {
        ui->logLayout->hide();
        ui->showButtonLayout->show();
    }
    else
    {
        ui->showButtonLayout->hide();
        ui->logLayout->show();
    }
}

/**
  SLOT
  Show dialog for setting uniform variables
  */
void MainWindow::showSetUniformVarDialog()
{
    if(!isProjectActive())
        return;

    UniformVariableEditor dialog;

    QList<UniformVariable*> unifs = infoM->getActiveProject()->getUniformVariables();

    QList<UniformVariableEditor::UniformAttachment> settings;

    foreach(UniformVariable* u, unifs)
    {
        if(u->isAttached())
        {
            QList<UniformTypes::Attachment> attachments = u->getAttached();

            foreach(UniformTypes::Attachment a, attachments)
            {
                UniformVariableEditor::UniformAttachment uAt;

                uAt.programName = a.programName;
                uAt.uniformName = u->getName();
                uAt.variableName = a.variableName;

                settings.append(uAt);
            }
        }
    }

    dialog.setUniformSettings(settings);

    dialog.exec();

    if(dialog.result() == dialog.Accepted)
    {
        //removed attachments
        QList<UniformVariableEditor::UniformAttachment> removed = dialog.getRemovedSettings();

        if(!removed.isEmpty())
        {
            foreach(UniformVariableEditor::UniformAttachment rm, removed)
            {
                infoM->getActiveProject()->getUniformVariable(rm.uniformName)->
                        removeAttachment(rm.programName,rm.variableName);
            }
        }

        //add new attachments
        QList<UniformVariableEditor::UniformAttachment> ret = dialog.getUniformSettings();

        if(ret.isEmpty())
            return;

        foreach(UniformVariableEditor::UniformAttachment r, ret)
        {
            UniformVariable* uVar = infoM->getActiveProject()->getUniformVariable(r.uniformName);

            if(uVar == NULL)
                continue;
            else
            {
                uVar->addAttachment(r.programName,r.variableName);
            }
        }

        infoM->saveProject();

        emit reloadUniformVariables();
    }
}

/**
  SLOT
  Show dialog for creating uniform variables
  Create named uniform variables in this dialog
  */
void MainWindow::showCreateUniformDialog()
{
    if(!isProjectActive())
        return;

    emit pauseDrawing(true);

    CreateUniformVariables dialog;

    dialog.exec();

    infoM->saveProject();

    emit reloadUniformVariables();
    emit pauseDrawing(false);

}

/**
 * @brief MainWindow::showLoadModelDialog Show dialog windows for loading model
 */
void MainWindow::showLoadModelDialog()
{
    if(!isProjectActive())
        return;

    QString modelFile = QFileDialog::getOpenFileName(this,tr("Load Model"), QDir::homePath(),
                                 tr("3D Models (*.dae *.3ds *.ase *.obj *.ifc *.xgl *.zgl *.ply *.iwo *.iws *.lxo *.stl *.x *.ac *.ms3d);; All Files (*.*)"));

    if(modelFile.isEmpty())
        return;

    Model* model = new Model(0,infoM->getActiveProject()->getName());

    if(model->loadModel(modelFile))
    {
        infoM->getActiveProject()->setModel(model);
        MetaShaderProg* prog = infoM->getActiveProject()->getProgram();
        if(prog != NULL)
        {
            QString progName = prog->getName();
            model->setShaderPrograms(progName);
        }

        infoM->saveProject();
        emit reloadModel();
    }
    else
        QMessageBox::warning(this,tr("Loaded error"),tr("Object canno't be loaded:\n%1").arg(model->getErrorLog()));
}

/**
 * @brief MainWindow::showAttachModelDialog Show dialog window for attaching model to shader program
 */
void MainWindow::showAttachModelDialog()
{
    if(!isProjectActive())
        return;

    if(!infoM->getActiveProject()->isModelLoaded())
    {
        QMessageBox::warning(this,tr("No model"),tr("No model loaded in this project."));
        return;
    }
    ModelAttachmentDialog dialog;

    dialog.exec();

    infoM->saveProject();
    emit reloadProgramSettings();
}

/**
 * @brief MainWindow::showProgramDialog Create shader program manager dialog if active project is set
 */
void MainWindow::showProgramDialog()
{
    if(!isProjectActive())
        return;

    ProgramManagerDialog program;

    program.exec();

    // set any shader program to model meshes
    if(infoM->getActiveProject()->isModelLoaded() && !infoM->getActiveProject()->getProgramNames().isEmpty())
    {
        if(infoM->getActiveProject()->getModel()->getAttachedPrograms().isEmpty())
        {
            QSet<ModelNode*> nodes = infoM->getActiveProject()->getModel()->getNodes();
            MetaShaderProg* prog = infoM->getActiveProject()->getPrograms().value(0);

            if(!prog->isValid())
                return;

            QString name = prog->getName();

            foreach(ModelNode* node, nodes)
            {
                node->setShProgramToAll(name);
            }
        }
    }
    infoM->saveProject();
    emit reloadProgramSettings();
}

/**
 * @brief MainWindow::showProjectSettings Show project settings dialog window.
 */
void MainWindow::showProjectSettings()
{
    if(!isProjectActive())
        return;

    OpenglSettingsDialog dialog(this);

    dialog.exec();

    infoM->saveProject();

    emit newOpenGLSettings();
}

/**
 * @brief MainWindow::connectActiveProject Refresh information when active project changed.
 */
void MainWindow::refreshActiveProject()
{
    if(!isProjectActive("",false))
    {
        static_cast<ProfileWidget*>(ui->dockWidget->widget())->refreshShPrograms(QStringList());
        return;
    }

    static_cast<ProfileWidget*>(ui->dockWidget->widget())->
            refreshShPrograms(infoM->getActiveProject()->getProgramNames());

    emit newOpenGLSettings();
}

/**
 * @brief MainWindow::connectCreatedProject Connect new project to ProfileWidget slots
 * @param name Name of the new created project
 */
void MainWindow::connectCreatedProject(QString name)
{
    connect(infoM->getProject(name),SIGNAL(shProgCreated(QString)),
            static_cast<ProfileWidget*>(ui->dockWidget->widget()),SLOT(insertShProgram(QString)));

    connect(infoM->getProject(name),SIGNAL(shProgDestroyed(QString)),this,SLOT(refreshActiveProject()));
}

/**
 * @brief MainWindow::projectRemoved If project for closing is active project, then invalidate drawing.
 * @param name Name of the project to close.
 */
void MainWindow::projectRemoved(QString name)
{
    if(!isProjectActive("",false))
    {
        ui->GL_Window_underlay->returnOGLwindow()->invalidateRender();
        return;
    }

    QString activeProj = infoM->getActiveProject()->getName();

    if(activeProj == name)
        ui->GL_Window_underlay->returnOGLwindow()->invalidateRender();
}

/**
 * @brief MainWindow::changeActiveProject React when active project is changed.
 * If new project is not set then call projectRemoved method to invalidate draw.
 * If new project is set then build new project.
 * @param newName Name of the new project, if new project is empty then it is not set.
 * @param oldName Name of the old project, used for invalidation.
 */
void MainWindow::changeActiveProject(QString newName, QString oldName)
{
    if(newName.isEmpty())
    {
        projectRemoved(oldName);
    }
    else
    {
        buildShader();
        emit newOpenGLSettings();
    }
}

/**
 * @brief MainWindow::showTextureDialog Show texture options dialog
 */
void MainWindow::showTextureDialog()
{
    if(!isProjectActive())
        return;

    TextureDialog dialog;

    dialog.exec();

    infoM->saveProject();
    emit reloadTextures();
}

/**
 * @brief MainWindow::showMeasureDockWidget Show dock widget for graph with drawing.
 */
void MainWindow::showMeasureDockWidget()
{
    ui->dockWidget->setVisible(true);
}

/**
 * @brief MainWindow::buildShader Test if active project is set, if is then build and run new shaders.
 */
void MainWindow::buildShader()
{
    //if(!isProjectActive())
    //    return;

    ui->tabEditorWindow->saveAll();

    ui->GL_Window_underlay->returnOGLwindow()->runShaders();
}

/**
 * @brief MainWindow::removeShader Remove active opened shader from project.
 */
void MainWindow::removeShader()
{
    QString path = ui->tabEditorWindow->getOpenedPath();

    if(path.isEmpty())
        return;

    QList<MetaProject*> list = infoM->getProjects();

    foreach(MetaProject* p, list)
    {
        QString newPath = p->pathToRelative(path);
        MetaShader* shader = p->getShader(newPath);

        if(shader != NULL)
        {
            QMessageBox box;
            box.setText(tr("Do you want to remove this shader?"));
            box.setInformativeText(tr("Shader file will be deleted from harddrive."));
            box.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
            box.setIcon(QMessageBox::Question);
            box.setDefaultButton(QMessageBox::Cancel);

            int ret = box.exec();

            if(ret == QMessageBox::Yes)
            {
                p->removeShader(shader->getShader(),true);
                infoM->saveProject(p->getName());
            }
            return;
        }
    }
}

/**
 * @brief MainWindow::shaderRemoved Shader was removed so we close opened tabs in code editor.
 * @param name Name of the shader.
 */
void MainWindow::shaderRemoved(QString name)
{
    MetaProject* proj = static_cast<MetaProject*>(sender());

    if(proj == NULL)
        return;

    QString newPath = proj->pathToAbsolute(name);

    ui->tabEditorWindow->closeTab(newPath, proj);
    infoM->saveProject(proj->getName());
}

/**
 * @brief MainWindow::connectShaders Connect new created projects to slot in this window.
 * @param name Name of the created shaders.
 */
void MainWindow::connectShaders(QString name)
{
    MetaProject* proj = infoM->getProject(name);

    connect(proj,SIGNAL(shaderDestroyed(QString,MetaShader::SHADERTYPE)),this,SLOT(shaderRemoved(QString)));
}
