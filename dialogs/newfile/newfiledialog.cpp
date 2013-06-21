#include "newfiledialog.h"
#include "ui_newfiledialog.h"
#include <QTextStream>
#include <QResource>

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);

    initialize();

    // set default values
    path = QDir::homePath();
    //path = QDir::toNativeSeparators(path) + QDir::separator() + "vertex";
    path = path + '/' + "vertex";

    //ui->pathEdit->setText(path + QDir::separator() + fName);
    ui->pathEdit->setText(path + '/' + fName);

    //connect signals and slots
    customConnect();

}

/**
 * @brief NewFileDialog::NewFileDialog Create new file dialog
 * @param projectBaseDir Starting directory
 * @param parent Parent of this QObject
 */
NewFileDialog::NewFileDialog(QString projectBaseDir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewFileDialog)
{
    ui->setupUi(this);

    initialize();

    // set default values
    //path = QDir::toNativeSeparators(projectBaseDir) + QDir::separator() + "vertex";
    path = projectBaseDir + '/' + "vertex";

    //ui->pathEdit->setText(path + QDir::separator() + fName);
    ui->pathEdit->setText(path + '/' + fName);

    //connect signals and slots
    customConnect();
}

NewFileDialog::~NewFileDialog()
{
    delete ui;
}

/**
  Return absolute path to new file
  */
QString NewFileDialog::getPath()
{
    //return(path + QDir::separator() + fName);
    return(path + '/' + fName);
}

/**
  Return type of new file
  Return code InfoManager::SHADERTYPE
  */
InfoManager::SHADERTYPE NewFileDialog::getType()
{
    return fType;
}


void NewFileDialog::customConnect()
{
    connect(selectionM,SIGNAL(selectionChanged(const QItemSelection & , const QItemSelection & )),
            this,SLOT(selectionChangedSlot(const QItemSelection &,const QItemSelection &)));
    connect(ui->BrowseButton,SIGNAL(clicked()),this,SLOT(openSaveManager()));
    connect(ui->pathEdit,SIGNAL(editingFinished()),this,SLOT(pathEdited()));
}

void NewFileDialog::initialize()
{
    fName = "vertex.vert";

    // set choose list
    model = new QStandardItemModel;
    QStandardItem *item = model->invisibleRootItem();
    item->appendRow(new QStandardItem("Vertex Shader"));
    item->appendRow(new QStandardItem("Fragment Shader"));
    //item->appendRow(new QStandardItem("Text File"));

    // add to listView
    ui->pickList->setModel(model);

    // set selection
    ui->pickList->setCurrentIndex(model->index(0,0));
    selectionM = ui->pickList->selectionModel();
    fType = InfoManager::VERTEX;

    isDefault = true;
}

/** SLOTS **/

/**
  SLOT
  When user select on list
  */
void NewFileDialog::selectionChangedSlot(const QItemSelection & /* newSelection */, const QItemSelection & /* oldSelection */)
{
    const QModelIndex index = ui->pickList->currentIndex();

    QString selected = index.data().toString();
    qDebug() << selected;

    if(isDefault && selected != "Text File")
    {
        QDir dir(path);
        dir.cdUp();
        path = dir.absolutePath();
    }

    if(selected == "Vertex Shader")
    {
        if(isDefault)
        {
            fName = "vertex.vert";
            //path += QDir::separator() + QString("vertex");
            path += '/' + QString("vertex");
        }

        fType = InfoManager::VERTEX;
    }
    else if(selected == "Fragment Shader")
    {
        if(isDefault)
        {
            fName = "fragment.frag";
            //path += QDir::separator() + QString("fragment");
            path += '/' + QString("fragment");
        }

        fType = InfoManager::FRAGMENT;
    }

    //ui->pathEdit->setText(path + QDir::separator() + fName);
    ui->pathEdit->setText(path + '/' + fName);
}

/**
  SLOT
  Browse file
  */
void NewFileDialog::openSaveManager()
{
    QString start = ui->pathEdit->text();
    if(!QFile(start).exists())
        start = QDir::homePath();

    QString temp;
    // get path
    temp = QFileDialog::getSaveFileName(this,tr("File Name"),start);
    //temp = QDir::toNativeSeparators(temp);

    if(temp.isEmpty())
        return;

    // set name and path
    fName = QFileInfo(temp).fileName();
    path = QFileInfo(temp).absolutePath();
    //path = QDir::toNativeSeparators(path);

    //ui->pathEdit->setText(path + QDir::separator() + fName);
    ui->pathEdit->setText(path + '/' + fName);

    isDefault = false;
}

/**
  SLOT
  Path was edited, we need to synchronize this
  */
void NewFileDialog::pathEdited()
{
    // get text
    QString line = ui->pathEdit->text();

    path = QFileInfo(line).absolutePath();
    //path = QDir::toNativeSeparators(path);
    fName = QFileInfo(line).fileName();

    isDefault = false;
}

/**
 * SLOT
 * @brief NewFileDialog::accept overwrited accept function,
 *  we need to check if the file can be created and if can create it.
 */
void NewFileDialog::accept()
{
    // create file
    //QFile f(path + QDir::separator() + fName);
    QFile f(path + '/' + fName);
    if(!f.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::critical(this,tr("File is not wirtable"),
                              tr("You canno't create file on this location"));
        return;
    }

    if(fType == InfoManager::FRAGMENT)
    {
        QResource r(":/shaders/app_default.frag");
        QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
        f.write(b);
    }
    else if(fType == InfoManager::VERTEX)
    {
        QResource r(":/shaders/app_default.vert");
        QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
        f.write(b);
    }
    else
    {
        f.write("");
    }

    f.close();

    setResult(Accepted);
    hide();
}
