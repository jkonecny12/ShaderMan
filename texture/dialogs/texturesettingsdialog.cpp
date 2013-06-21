#include "texturesettingsdialog.h"
#include "ui_texturesettingsdialog.h"
#include "infomanager.h"
#include <QMessageBox>

#define TEXNAME "Texture"

/**
 * @brief TextureSettingsDialog::TextureSettingsDialog Create empty settings dialog
 * @param parent Parent of this Object
 */
TextureSettingsDialog::TextureSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureSettingsDialog)
{
    ui->setupUi(this);
    texStorage = NULL;
    isTextureSet = false;

    setUpComponents();
}


/**
 * @brief TextureSettingsDialog::TextureSettingsDialog Create texture settings dialog and create new texture from given path.
 * @param path Path to texture.
 * @param parent Parent of this Object.
 */
TextureSettingsDialog::TextureSettingsDialog(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextureSettingsDialog)
{
    ui->setupUi(this);

    //this->path = QDir::toNativeSeparators(path);
    this->path = path;
    isTextureSet = false;
    texStorage = new TextureStorage(TEXNAME, path, TextureStorage::TEXTURE_2D);

    setUpComponents();

    readTexture();
}

/**
 * @brief TextureSettingsDialog::~TextureSettingsDialog Destroy this object and all children
 */
TextureSettingsDialog::~TextureSettingsDialog()
{
    delete ui;

    if(result() == Rejected && isTextureSet == false)
        delete texStorage;
}

/**
 * @brief TextureSettingsDialog::setTexture Set texture to this settings dialog. Set all parameters from here.
 * @param texture Texture object from where we get values.
 */
void TextureSettingsDialog::setTexture(TextureStorage* texture)
{
    if(texStorage != NULL)
        delete texStorage;

    texStorage = texture;

    isTextureSet = true;

    readTexture();
}

/**
 * @brief TextureSettingsDialog::getTextureName Get texture name (must be unique)
 * @return Return texture name
 */
QString TextureSettingsDialog::getName()
{
    return ui->texNameEdit->text();
}

/**
 * @brief TextureSettingsDialog::accept Override method for accepting dialog and saving new texture
 */
void TextureSettingsDialog::accept()
{
    InfoManager* info = InfoManager::getInstance();

    // test if name was changed to available name
    if(texStorage != NULL)
    {
        if(!isTextureSet)
        {
            if(!testNames())
                return;
        }
        else if(texStorage->getName() != ui->texNameEdit->text())
        {
            if(!testNames())
                return;
        }


    }

    TextureStorage::TexType target =
            static_cast<TextureStorage::TexType>(ui->texTypeBox->itemData(
                                                     ui->texTypeBox->currentIndex()).toInt());

    if(texStorage == NULL)
    {
        // create new texture
        texStorage = new TextureStorage(ui->texNameEdit->text(),
                                                     ui->texPathLabel->text(),
                                                     target);
    }

    texStorage->setName(ui->texNameEdit->text());
    texStorage->setType(target);
    texStorage->setMipmap(ui->mipmapChecker->isChecked());

    int filter = ui->minFilterBox->itemData(ui->minFilterBox->currentIndex()).toInt();
    texStorage->setMinFilter(static_cast<TextureStorage::TexFilter>(filter));
    filter = ui->magFilterBox->itemData(ui->magFilterBox->currentIndex()).toInt();
    texStorage->setMagFilter(static_cast<TextureStorage::TexFilter>(filter));

    int wrap = ui->clampSBox->itemData(ui->clampSBox->currentIndex()).toInt();
    texStorage->setSWrap(static_cast<TextureStorage::TexWrap>(wrap));
    wrap = ui->clampRBox->itemData(ui->clampRBox->currentIndex()).toInt();
    texStorage->setRWrap(static_cast<TextureStorage::TexWrap>(wrap));

    info->getActiveProject()->addTexture(texStorage);

    setResult(QDialog::Accepted);
    hide();
}

/**
 * @brief TextureSettingsDialog::setUpComponents Initialize components of this object
 */
void TextureSettingsDialog::setUpComponents()
{
    connect(ui->texNameEdit,SIGNAL(textChanged(QString)),ui->texNameLabel,SLOT(setText(QString)));
    connect(ui->mipmapChecker,SIGNAL(toggled(bool)),this,SLOT(mipMapChecked(bool)));

    ui->texNameEdit->setText(TEXNAME);
    ui->texPathLabel->setText(path);
    ui->texName->setText(TEXNAME);

    ui->clampTBox->hide();
    ui->clampTLabel->hide();

    //fill up combo boxes
    QStringList labelType = TextureStorage::getGLTypeString();

    for(int i = 0; i < labelType.size(); ++i)
    {
        ui->texTypeBox->addItem(labelType.value(i), QVariant(i));
    }

    ui->texTypeBox->setCurrentIndex(1);

    filterMipMap = TextureStorage::getGLFilterString();
    filter << filterMipMap.value(0) << filterMipMap.value(1);

    for(int i = 0; i < filter.size(); ++i)
    {
        ui->minFilterBox->addItem(filter.value(i), QVariant(i));

        if(i == TextureStorage::NEAREST)
        {
            ui->magFilterBox->addItem(filter.value(i), QVariant(i));
        }
        else if(i == TextureStorage::LINEAR)
        {
            ui->magFilterBox->addItem(filter.value(i), QVariant(i));
            ui->magFilterBox->setCurrentIndex(ui->magFilterBox->model()->rowCount()-1);
        }
    }

    ui->minFilterBox->setCurrentIndex(TextureStorage::LINEAR);
    //ui->magFilterBox->setCurrentIndex(TextureStorage::LINEAR);

    QStringList labelWrap = TextureStorage::getGLWrapString();

    for(int i = 0; i < labelWrap.size(); ++i)
    {
        ui->clampRBox->addItem(labelWrap.value(i), QVariant(i));
        ui->clampSBox->addItem(labelWrap.value(i), QVariant(i));
        ui->clampTBox->addItem(labelWrap.value(i), QVariant(i));
    }

    ui->clampRBox->setCurrentIndex(TextureStorage::REPEAT);
    ui->clampSBox->setCurrentIndex(TextureStorage::REPEAT);
    ui->clampTBox->setCurrentIndex(TextureStorage::REPEAT);
}

/**
 * @brief TextureSettingsDialog::readTexture Return information from texture and set this to settings dialog.
 */
void TextureSettingsDialog::readTexture()
{
    if(texStorage == NULL)
        return;

    ui->texNameEdit->setText(texStorage->getName());
    ui->texName->setText(texStorage->getName());
    ui->texPathLabel->setText(texStorage->getPath());

    ui->texTypeBox->setCurrentIndex(texStorage->getType());
    ui->mipmapChecker->setChecked(texStorage->isMipMap());

    setUpMinFilter();

    ui->minFilterBox->setCurrentIndex(texStorage->getMinFilter());

    int id = ui->magFilterBox->findData(QVariant(texStorage->getMagFilter()));

    if(id != -1)
        ui->magFilterBox->setCurrentIndex(id);

    ui->clampRBox->setCurrentIndex(texStorage->getRWrap());
    ui->clampSBox->setCurrentIndex(texStorage->getSWrap());
}

/**
 * @brief TextureSettingsDialog::testNames Test if setted texture name is available.
 * @return Return true if name can be used.
 */
bool TextureSettingsDialog::testNames()
{
    QStringList names = InfoManager::getInstance()->getActiveProject()->getTextureNameList();

    foreach(QString name, names)
    {
        if(ui->texNameEdit->text() == name)
        {
            QMessageBox box;
            box.setText(tr("You canno't use the same name twice!"));
            box.setIcon(QMessageBox::Warning);
            box.setDefaultButton(QMessageBox::Ok);
            box.exec();
            return false;
        }
    }

    return true;
}

/**
 * @brief TextureSettingsDialog::setUpMinFilter Setup combobox for showing only NEAREST and LINEAR items when mipmap is disabled.
 * This settings is only for min filter, becouse mag filter have only NEAREST and LINEAR all time.
 */
void TextureSettingsDialog::setUpMinFilter()
{
    bool checked = ui->mipmapChecker->isChecked();

    int minId = ui->minFilterBox->currentIndex();

    ui->minFilterBox->clear();

    if(checked)
    {
        for(int i = 0; i < filterMipMap.size(); ++i)
        {
            ui->minFilterBox->addItem(filterMipMap.value(i), QVariant(i));
        }

        ui->minFilterBox->setCurrentIndex(minId);
    }
    else
    {
        for(int i = 0; i < filter.size(); ++i)
        {
            ui->minFilterBox->addItem(filter.value(i), QVariant(i));
        }

        if(minId > 1)
            ui->minFilterBox->setCurrentIndex(TextureStorage::LINEAR);
        else
            ui->minFilterBox->setCurrentIndex(minId);
    }
}

/**
 * @brief TextureSettingsDialog::mipMapChecked Show and hide Mipmap level combo box.
 * @param checked If true then mipmap checker is checked, false otherwise.
 */
void TextureSettingsDialog::mipMapChecked(bool /*checked*/)
{
    setUpMinFilter();
}

/**
 * @brief TextureSettingsDialog::clampTvisibility Only if TEXTURE_3D is set as type you can use T coordinate.
 * @param id Id of item set in typeBox.
 */
/*
void TextureSettingsDialog::clampTvisibility(int id)
{
    if(ui->texTypeBox->itemData(id) == TextureStorage::TEXTURE_3D)
    {
        ui->clampTBox->show();
        ui->clampTLabel->show();
    }
    else
    {
        ui->clampTBox->hide();
        ui->clampTLabel->hide();
    }
}
*/
