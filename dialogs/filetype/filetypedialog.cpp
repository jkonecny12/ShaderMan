#include "filetypedialog.h"
#include "ui_filetypedialog.h"

FileTypeDialog::FileTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileTypeDialog)
{
    ui->setupUi(this);

    customConnect();
}

FileTypeDialog::~FileTypeDialog()
{
    delete ui;
}

/**
  Connect signals to slots
  */
void FileTypeDialog::customConnect()
{
    connect(ui->vertex_button,SIGNAL(clicked()),this,SLOT(vertexPressed()));
    connect(ui->fragment_button,SIGNAL(clicked()),this,SLOT(fragmentPressed()));
    //connect(ui->text_button,SIGNAL(clicked()),this,SLOT(textPressed()));
    connect(ui->cancel_button,SIGNAL(clicked()),SLOT(reject()));
}

/**
  Return which button was pressed
  */
InfoManager::SHADERTYPE FileTypeDialog::getFileType()
{
    return fileType;
}

/** SLOTS **/

/**
  Set result to vertex shader
  */
void FileTypeDialog::vertexPressed()
{
    fileType = InfoManager::VERTEX;
    accept();
}

/**
  Set result to fragment shader
  */
void FileTypeDialog::fragmentPressed()
{
    fileType = InfoManager::FRAGMENT;
    accept();
}

/**
  Set result to text file
  */
void FileTypeDialog::textPressed()
{
    fileType = InfoManager::TEXT;
    accept();
}
