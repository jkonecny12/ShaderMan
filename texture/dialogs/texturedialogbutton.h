#ifndef TEXTUREDIALOGREMOVEBUTTON_H
#define TEXTUREDIALOGREMOVEBUTTON_H

#include <QPushButton>
#include <QPersistentModelIndex>

class TextureDialogButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TextureDialogButton(QModelIndex index, QWidget *parent = 0);
    explicit TextureDialogButton(QString label, QModelIndex index, QWidget *parent = 0);
    explicit TextureDialogButton(QString label, QIcon icon, QModelIndex index, QWidget *parent = 0);

    ~TextureDialogButton();

private:
    QPersistentModelIndex* persistent;

signals:
    void clicked(QPersistentModelIndex* model);
    
private slots:
    void clickedIndex();
    
};

#endif // TEXTUREDIALOGREMOVEBUTTON_H
