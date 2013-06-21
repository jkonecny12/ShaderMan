#ifndef TEXTUREDIALOG_H
#define TEXTUREDIALOG_H

#include <QDialog>
#include <QPersistentModelIndex>

namespace Ui {
class TextureDialog;
}

class TextureDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TextureDialog(QWidget *parent = 0);
    ~TextureDialog();

    void addTexture(QString name);
    void showTextureImage(QPixmap img);

private slots:
    void addNewTexture();
    void removeTexture(QPersistentModelIndex *index);
    void openSettings(QPersistentModelIndex* index);
    void showTexture(int row, int);
    
private:
    Ui::TextureDialog *ui;
};

#endif // TEXTUREDIALOG_H
