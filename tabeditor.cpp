#include "tabeditor.h"
#include <QMessageBox>

TabEditor::TabEditor(QWidget *parent) :
    QTabWidget(parent)
{
    customConnect();

    infoManager = InfoManager::getInstance();
}

/**
  Create custom connect signal and slots
  */
void TabEditor::customConnect()
{
    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}


/**
  Create new shader file
  */
void TabEditor::newDocument(QString path, InfoManager::SHADERTYPE type)
{
    CodeEditor *editor = new CodeEditor(this, path, type);
    activeFiles.append(editor);
    // when text change add * to tab name
    connect(editor,SIGNAL(textChanged()),this,SLOT(fileModified()));
    connect(editor,SIGNAL(fileSaved()),this,SLOT(fileModified()));

    addTab(editor, QFileInfo(path).fileName());
    setCurrentWidget(editor);
}

/**
  Open file in editor
  If file is opened activete tab
  */
void TabEditor::openFile(QString path, InfoManager::SHADERTYPE type)
{
    if(activateTab(path))
        return;

    CodeEditor *edit = new CodeEditor(this, path, type);
    addTab(edit,QFileInfo(path).fileName());
    activeFiles.append(edit);
    connect(edit,SIGNAL(textChanged()),this,SLOT(fileModified()));
    connect(edit,SIGNAL(fileSaved()),this,SLOT(fileModified()));
    setCurrentWidget(edit);
}

/**
 * @brief TabEditor::saveAll Save all opened tabs.
 */
void TabEditor::saveAll()
{
    int tabs = count();

    for(int i = 0; i < tabs; ++i)
    {
        CodeEditor* editor = static_cast<CodeEditor*>(widget(i));
        editor->saveFile();
    }
}

/**
 * @brief TabEditor::testModified Test all tabs if they are modified.
 * @return If all tabs are not modified return true, else return false.
 */
bool TabEditor::testModified()
{
    int tabs = count();

    for(int i = 0; i < tabs; ++i)
    {
        CodeEditor* editor = static_cast<CodeEditor*>(widget(i));
        if(editor->isModified())
            return false;
    }

    return true;
}

/**
 * @brief TabEditor::getOpenedPath Get path to the file opened in active CodeEditor.
 * @return Empty string if nothing is opened, else path to file.
 */
QString TabEditor::getOpenedPath()
{
    QWidget* widget = currentWidget();

    if(widget == NULL)
        return QString();

    CodeEditor* editor = static_cast<CodeEditor*>(widget);

    return editor->getFilePath();
}

/**
  Question if this file is opened in project
  */
bool TabEditor::isOpened(QString path)
{
    QListIterator<CodeEditor*> it(activeFiles);

    //path = QDir::toNativeSeparators(path);

    while(it.hasNext())
    {
        QString i = it.next()->getFilePath();
        //i = QDir::toNativeSeparators(i);

        if(i == path)
            return true;
    }

    return false;
}

/**
  Activate Tab where file with path is opened
  */
bool TabEditor::activateTab(QString path)
{
    QListIterator<CodeEditor*> it(activeFiles);
    //path = QDir::toNativeSeparators(path);

    CodeEditor* editor;

    while(it.hasNext())
    {
        editor = it.next();
        //if(QDir::toNativeSeparators(editor->getFilePath()) == path)
        if(editor->getFilePath() == path)
        {
            setCurrentWidget(editor);
            return true;
        }
    }

    return false;
}

/**
 * @brief TabEditor::findIndex Find index of editor with path;
 * @param path Path to opened file.
 * @return Index of editor with opened file, if no editor found return -1;
 */
int TabEditor::findIndex(QString path)
{
    int num = count();
    //path = QDir::toNativeSeparators(path);

    for(int i = 0; i < num; ++i)
    {
        CodeEditor* editor = static_cast<CodeEditor*>(widget(i));

        if(editor->getFilePath() == path)
        {
            return i;
        }
    }

    return -1;
}

/**
  SLOT
  Close tab on index.
  */
void TabEditor::closeTab(int index)
{
    CodeEditor *editor = static_cast<CodeEditor*>(this->currentWidget());

    if(editor == NULL)
        return;

    //infoManager->delShader(editor->getFilePath());

    this->removeTab(index);
    activeFiles.removeAt(index);
}

/**
 * @brief TabEditor::closeTab Close tab with editor with given path.
 * @param path Path to shader file.
 */
void TabEditor::closeTab(QString path, MetaProject *project)
{
    MetaProject* proj;

    if(project == NULL)
        proj = dynamic_cast<MetaProject*>(sender());
    else
        proj = project;

    if(proj != NULL)
    {
        path = proj->pathToAbsolute(path);
        int index = findIndex(path);

        closeTab(index);
    }
}

/**
  SLOT
  Save file in active tab
  */
void TabEditor::saveFile()
{
    if(currentIndex() == -1)
        return;

    CodeEditor *editor = static_cast<CodeEditor*>(currentWidget());
    QString path;

    path = editor->getFilePath();
    //path = QDir::toNativeSeparators(path);

    editor->saveFile();

    setTabText(currentIndex(),QFileInfo(path).fileName());
}

/**
 * @brief TabEditor::saveAsFile Open dialog for saving file as.
 * Only save file as new, do not open it or add to project it.
 */
void TabEditor::saveAsFile()
{
    if(currentIndex() == -1)
        return;

    QString path = QFileDialog::getSaveFileName(this, tr("Save shader"),
                                                QDir::homePath(), tr("Shader file"));

    if(path.isEmpty())
        return;

    CodeEditor* editor = static_cast<CodeEditor*>(currentWidget());

    if(!editor->saveFileAs(path))
    {
        QMessageBox box;
        box.setText(tr("File save failed!"));
        box.setInformativeText(tr("You canno't save file to this position!"));
        box.setIcon(QMessageBox::Warning);
        box.exec();
    }
}

/**
  SLOT
  When file modified set * to tab name
  */
void TabEditor::fileModified()
{
    //int id = currentIndex();

    //CodeEditor* editor = static_cast<CodeEditor*>(currentWidget());
    CodeEditor* editor = static_cast<CodeEditor*>(sender());

    if(editor == NULL)
    {
        qDebug() << "Sender codeeditor is null";
    }

    int id = indexOf(editor);

    QString name = editor->getFilePath();
    name = QFileInfo(name).fileName();

    if(editor->isModified())
        setTabText(id, name + QString("*"));
    else
        setTabText(id, name);
}
