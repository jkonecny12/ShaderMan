#include "texturedialog.h"
#include "ui_texturedialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include "texturesettingsdialog.h"
#include "texturedialogbutton.h"
#include <QDebug>
#include "infomanager.h"

#define RM_BTN_COL 0
#define NAME_COL 1
#define SET_BTN_COL 2
#define COL_NUM 3

/**
 * @brief TextureDialog::TextureDialog Dialog for texture creating
 * @param parent Parent of this QWidget
 */
TextureDialog::TextureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureDialog)
{
    ui->setupUi(this);

    // default sizes
    QList<int> sizes;
    sizes << 300 << 600;
    ui->splitter->setSizes(sizes);

    ui->textureList->setColumnCount(COL_NUM);
    QStringList names;
    names << tr("Remove") << tr("Name") << tr("Edit");
    ui->textureList->setHorizontalHeaderLabels(names);
    ui->textureList->adjustSize();

    // create graphics view
    QGraphicsScene* scene = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(scene);

    connect(ui->addTextureButton,SIGNAL(clicked()),this,SLOT(addNewTexture()));
    connect(ui->textureList,SIGNAL(cellClicked(int,int)),this,SLOT(showTexture(int,int)));

    QStringList list = InfoManager::getInstance()->getActiveProject()->getTextureNameList();

    foreach(QString texName, list)
    {
        addTexture(texName);
    }
}

/**
 * @brief TextureDialog::~TextureDialog Destroy this widget and all childrens
 */
TextureDialog::~TextureDialog()
{
    delete ui;
}

/**
 * @brief TextureDialog::addTexture Import image as texture
 */
void TextureDialog::addNewTexture()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.bmp *.gif)"));

    if(fileName == "")
        return;

    //fileName = QDir::toNativeSeparators(fileName);

    TextureSettingsDialog settings(fileName);
    settings.exec();

    if(settings.result() == settings.Rejected)
        return;

    InfoManager::getInstance()->getActiveProject()->addTexture(settings.getTexture());

    addTexture(settings.getName());
}

/**
 * @brief TextureDialog::addTexture Add texture to table with buttons and given name
 * @param name Name of the new texture
 */
void TextureDialog::addTexture(QString name)
{
    int rowNum = ui->textureList->rowCount();

    // Add texture name to table
    ui->textureList->setRowCount(rowNum + 1);
    QTableWidgetItem* item = new QTableWidgetItem(name);
    ui->textureList->setItem(rowNum, NAME_COL, item);

    // Add remove item button to table
    TextureDialogButton* btnRemove = new TextureDialogButton(tr("Remove"), ui->textureList->model()->index(rowNum, RM_BTN_COL), ui->textureList);
    connect(btnRemove,SIGNAL(clicked(QPersistentModelIndex*)),this,SLOT(removeTexture(QPersistentModelIndex*)));
    ui->textureList->setIndexWidget(ui->textureList->model()->index(rowNum, RM_BTN_COL), btnRemove);

    // Open settings menu button for this texture
    TextureDialogButton* btnTexture = new TextureDialogButton(tr("Settings"), ui->textureList->model()->index(rowNum, SET_BTN_COL), ui->textureList);
    connect(btnTexture,SIGNAL(clicked(QPersistentModelIndex*)),this,SLOT(openSettings(QPersistentModelIndex*)));
    ui->textureList->setIndexWidget(ui->textureList->model()->index(rowNum, SET_BTN_COL), btnTexture);

    TextureStorage* tex = InfoManager::getInstance()->getActiveProject()->getTexture(name);

    if(tex == NULL)
        return;

    QPixmap pixmap(tex->getPath());
    showTextureImage(pixmap);
}

/**
 * @brief TextureDialog::showTextureImage Show texture in Graphics view
 * @param img What we want show
 */
void TextureDialog::showTextureImage(QPixmap img)
{
    QGraphicsScene* scene = ui->graphicsView->scene();
    scene->clear();
    scene->addPixmap(img);
}

/**
 * @brief TextureDialog::removeTexture Remove texture from table and from project
 * @param index
 */
void TextureDialog::removeTexture(QPersistentModelIndex* index)
{
    QMessageBox box;
    box.setText(tr("Do you want to remove this texture?"));
    box.setInformativeText(tr("This will not remove texture file from your harddisk."));
    box.setIcon(QMessageBox::Warning);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setDefaultButton(QMessageBox::Cancel);

    if(!index->isValid())
    {
        QMessageBox msgbox;
        msgbox.setText(tr("Unknown error when removing textures."));
        msgbox.setIcon(QMessageBox::Critical);
        msgbox.setStandardButtons(QMessageBox::Ok);
        return;
    }

    int ret = box.exec();

    if(ret == QMessageBox::Ok)
    {
        QModelIndex idName = index->model()->index(index->row(), NAME_COL);
        QString name = idName.data().toString();
        ui->textureList->removeRow(index->row());

        // remove from project
        InfoManager::getInstance()->getActiveProject()->removeTexture(name);
    }
}

/**
 * @brief TextureDialog::openSettings Open Settings dialog for texture on index
 * @param index Index in table identify needed texture.
 */
void TextureDialog::openSettings(QPersistentModelIndex *index)
{
    //qDebug(index->data().toString());
    QModelIndex nameIndex = index->model()->index(index->row(), NAME_COL);

    TextureStorage* storage = InfoManager::getInstance()->getActiveProject()->getTexture(nameIndex.data().toString());

    if(storage == NULL)
    {
        QMessageBox msg;
        msg.setText(tr("Unknown error when using texture!"));
        msg.setIcon(QMessageBox::Critical);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }


    TextureSettingsDialog dialog;
    QString oldName = storage->getName();

    dialog.setTexture(storage);

    dialog.exec();

    // repair table if name changed
    if(oldName != storage->getName())
    {
        ui->textureList->setItem(index->row(), NAME_COL, new QTableWidgetItem(storage->getName()));
    }
}

/**
 * @brief TextureDialog::showTexture Show texture in graphics view window
 * @param row Row of table for needed texture.
 */
void TextureDialog::showTexture(int row, int /*column*/)
{
    QModelIndex index = ui->textureList->model()->index(row,NAME_COL);

    InfoManager* info = InfoManager::getInstance();

    TextureStorage* texture = info->getActiveProject()->getTexture(index.data().toString());

    if(texture == NULL)
        return;

    /*
    QString imgPath = texture->getPath();
    imgPath = info->getActiveProject()->pathToAbsolute(imgPath);
    */

    QImage img = texture->getImage();
    QPixmap pix;
    pix.convertFromImage(img);

    QGraphicsScene* scene = ui->graphicsView->scene();

    scene->clear();
    scene->addPixmap(pix);
}
