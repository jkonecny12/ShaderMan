#include "texturedialogbutton.h"

/**
 * @brief textureDialogButton::textureDialogButton Create button with persistent model index
 * @param index Index from model
 * @param parent Parent of this widget
 */
TextureDialogButton::TextureDialogButton(QModelIndex index, QWidget *parent) :
    QPushButton(parent)
{
    persistent = new QPersistentModelIndex(index);

    connect(this,SIGNAL(clicked()),this,SLOT(clickedIndex()));
}

/**
 * @brief textureDialogButton::textureDialogButton Create button with persistent model index and label
 * @param label Label for this button
 * @param index Index from model
 * @param parent Parent of this widget
 */
TextureDialogButton::TextureDialogButton(QString label, QModelIndex index, QWidget *parent) :
    QPushButton(label, parent)
{
    persistent = new QPersistentModelIndex(index);

    connect(this,SIGNAL(clicked()),this,SLOT(clickedIndex()));
}

/**
 * @brief textureDialogButton::textureDialogButton Create button with persistent model index and label
 * @param label Label for this button
 * @param icon Icon for this button
 * @param index Index from model
 * @param parent Parent of this widget
 */
TextureDialogButton::TextureDialogButton(QString label, QIcon icon, QModelIndex index, QWidget *parent) :
    QPushButton(icon, label, parent)
{
    persistent = new QPersistentModelIndex(index);

    connect(this,SIGNAL(clicked()),this,SLOT(clickedIndex()));
}

/**
 * @brief TextureDialogButton::~TextureDialogButton Destroy this object with persistent model index
 */
TextureDialogButton::~TextureDialogButton()
{
    if(persistent != NULL)
        delete persistent;
}

/**
 * @brief textureDialogButton::clickedIndex Emit new signal with persistent model index
 */
void TextureDialogButton::clickedIndex()
{
    emit clicked(persistent);
}

