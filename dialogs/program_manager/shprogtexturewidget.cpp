#include "shprogtexturewidget.h"
#include <QHBoxLayout>

/**
 * @brief ShProgTextureWidget::ShProgTextureWidget Create this widget with attachment point line edit and texture combo box.
 * @param type Type of this widget.
 * @param parent Parent of this widget.
 */
ShProgTextureWidget::ShProgTextureWidget(QString type, QWidget *parent) :
    ShProgWidgetAbstract(type,parent)
{
    QHBoxLayout* layout = static_cast<QHBoxLayout*>(this->layout());
    textureNames = new QComboBox(this);
    pointName = new QLineEdit(this);

    layout->addWidget(pointName);
    layout->addWidget(textureNames);

    connect(textureNames,SIGNAL(currentIndexChanged(QString)),this,SLOT(textureChangedSlot(QString)));
    connect(pointName,SIGNAL(editingFinished()),this,SLOT(editingFinishedSlot()));
}

/**
 * @brief ShProgTextureWidget::getVariableValue Get name of attachment point for texture.
 * @return Return attachment point.
 */
QString ShProgTextureWidget::getVariableValue()
{
    return pointName->text();
}

/**
 * @brief ShProgTextureWidget::getTexture Get texture name from this widget.
 * @return Return texture name set.
 */
QString ShProgTextureWidget::getTexture()
{
    return textureNames->currentText();
}

/**
 * @brief ShProgTextureWidget::isValid Test this widget for validity.
 * @return Return true if point name field is not empty, false otherwise.
 */
bool ShProgTextureWidget::isValid()
{
    return !pointName->text().isEmpty();
}

/**
 * @brief ShProgTextureWidget::setTextureNames Set items to combo box for texture names.
 * @param names Texture names.
 */
void ShProgTextureWidget::setTextureNames(const QStringList names)
{
    textureNames->addItems(names);
    textureNames->setCurrentIndex(0);
}

/**
 * @brief ShProgTextureWidget::setTextureName Set actual combo box texture.
 * @param name Name of the texture.
 */
void ShProgTextureWidget::setTextureName(const QString name)
{
    int pos = textureNames->findText(name, Qt::MatchFixedString | Qt::MatchCaseSensitive);

    if(pos == -1)
        textureNames->setCurrentIndex(0);

    textureNames->setCurrentIndex(pos);
}

/**
 * @brief ShProgTextureWidget::setPointName Set attachment point for this widget.
 * @param name Name of the attachment point.
 */
void ShProgTextureWidget::setPointName(const QString name)
{
    pointName->setText(name);
    oldPoint = name;
}

/**
 * @brief ShProgTextureWidget::textureChangedSlot Reemit signal for texture change box, now with attachment point and texture.
 * @param texName Texture name for emitting signal.
 */
void ShProgTextureWidget::textureChangedSlot(QString texName)
{
    emit textureChanged(pointName->text(), texName);
}

/**
 * @brief ShProgTextureWidget::editingFinishedSlot Reemit signal when editing of attachment point ended, now with old and current value of this edit.
 */
void ShProgTextureWidget::editingFinishedSlot()
{
    QString point = pointName->text();

    emit editingFinished(oldPoint, point);

    oldPoint = point;
}
