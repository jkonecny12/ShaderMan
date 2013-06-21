#ifndef TEXTURESETTINGSDIALOG_H
#define TEXTURESETTINGSDIALOG_H

#include <QDialog>
#include "texture/texturestorage.h"

namespace Ui {
class TextureSettingsDialog;
}

class TextureSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextureSettingsDialog(QWidget* parent = 0);
    explicit TextureSettingsDialog(QString path, QWidget *parent = 0);
    ~TextureSettingsDialog();

    void setTexture(TextureStorage* texture);
    QString getName();
    /**
     * @brief getTexture Get created texture (this texture is not in infomanager).
     * @return Return texture.
     */
    inline TextureStorage* getTexture() const {return texStorage;}

    virtual void accept();

private:
    void setUpComponents();
    void readTexture();
    bool testNames();

    void setUpMinFilter();

private slots:
    void mipMapChecked(bool checked);
    //void clampTvisibility(int id);

private:
    Ui::TextureSettingsDialog *ui;
    QString path;
    TextureStorage* texStorage;
    bool isTextureSet;
    QStringList filterMipMap;
    QStringList filter;
};

#endif // TEXTURESETTINGSDIALOG_H
