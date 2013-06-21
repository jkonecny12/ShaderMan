#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"
#include <QResource>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    customConnect();

    // set default values
    ui->fragmentNameEdit->setText("fragment.frag");
    ui->vertexNameEdit->setText("vertex.vert");
    ui->browseDirEdit->setText(QDir::homePath());
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::customConnect()
{
    connect(ui->browseButton,SIGNAL(clicked()),this,SLOT(browseDir()));
}

/**
  Get project base directory
  */
QString NewProjectDialog::getBaseDir()
{
    //return(QDir::toNativeSeparators(dirPath + QDir::separator() + projName));
    return dirPath + '/' + projName;
}

/**
  Get project name
  */
QString NewProjectDialog::getProjectName()
{
    return(projName);
}

/**
  Get fragment shader name
  */
QString NewProjectDialog::getFragName()
{
    //return(QString("fragment") + QDir::separator() + fragName);
    return(QString("fragment") + '/' + fragName);
}

/**
  Get vertex shader name
  */
QString NewProjectDialog::getVertName()
{
    //return(QString("vertex") + QDir::separator() + vertexName);
    return(QString("vertex") + '/' + vertexName);
}


/** SLOTS **/

/**
  SLOT
  Browse dir dialog
  */
void NewProjectDialog::browseDir()
{
    dirPath = QFileDialog::getExistingDirectory(this,tr("Project directory"),QDir::homePath());

    //dirPath = QDir::toNativeSeparators(dirPath);

    ui->browseDirEdit->setText(dirPath);
}

/**
  SLOT
  Test project name
  */
void NewProjectDialog::accept()
{
    //test names -> cannot be empty
    if(ui->projectNameEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Name is empty"),tr("Project name cannot be empty"));
        return;
    }
    else if(ui->vertexNameEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Name is empty"),tr("Name of the vertex shader cannot be empty"));
        return;
    }
    else if(ui->fragmentNameEdit->text().isEmpty())
    {
        QMessageBox::warning(this,tr("Name is empty"),tr("Name of the fragment shader cannot be empty"));
        return;
    }
    else
    {
        dirPath = ui->browseDirEdit->text();
        projName = ui->projectNameEdit->text();
        fragName = ui->fragmentNameEdit->text();
        vertexName = ui->vertexNameEdit->text();

        //dirPath = QDir::toNativeSeparators(dirPath);
        //QFileInfo inf = QFileInfo(dirPath + QDir::separator() + projName);
        QFileInfo inf = QFileInfo(dirPath + '/' + projName);

        // create project directory
        if(QDir(dirPath).mkdir(projName))
        {
            //QString path = dirPath + QDir::separator() + projName;
            QString path = dirPath + '/' + projName;
            QDir projDir = QDir(path);
            // create subdirectories
            if(!projDir.mkdir("vertex"))
            {
                QMessageBox::warning(this,tr("IO_Error"),tr("Error directory %1 is not writable")
                                     //.arg(projDir.absolutePath() + QDir::separator() + "vertex"));
                                     .arg(projDir.absolutePath() + '/' + "vertex"));
                return;
            }
            if(!projDir.mkdir("fragment"))
            {
                QMessageBox::warning(this,tr("IO_Error"),tr("Error directory %1 is not writable")
                                     //.arg(projDir.absolutePath() + QDir::separator() + "fragment"));
                                     .arg(projDir.absolutePath() + '/' + "fragment"));
                return;
            }
            // create files
            //QFile vFile(path + QDir::separator() + "vertex" + QDir::separator() + vertexName);
            QFile vFile(path + '/' + "vertex" + '/' + vertexName);
            //QFile fFile(path + QDir::separator() + "fragment" + QDir::separator() + fragName);
            QFile fFile(path + '/' + "fragment" + '/' + fragName);

            if(!vFile.open(QFile::WriteOnly | QFile::Text))
            {
                QMessageBox::warning(this,tr("IO_Error"),tr("Error file %1 cannot be created")
                                     .arg(QFileInfo(vFile).absoluteFilePath()));
                return;
            }
            else
            {
                QResource r(":/shaders/app_default.vert");
                QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
                vFile.write(b);
            }

            if(!fFile.open(QFile::WriteOnly | QFile::Text))
            {
                QMessageBox::warning(this,tr("IO_Error"),tr("Error file %1 cannot be created")
                                     .arg(QFileInfo(fFile).absoluteFilePath()));
                return;
            }
            else
            {
                QResource r(":/shaders/app_default.frag");
                QByteArray b( reinterpret_cast< const char* >( r.data() ), r.size() );
                fFile.write(b);
            }

            setResult(QDialog::Accepted);
            hide();
            return;
        }
        else // project directory not created
            QMessageBox::warning(this,tr("test"),tr("Error directory %1 canno't be created").arg(inf.absolutePath()));
    }

}
