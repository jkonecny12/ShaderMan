#ifndef SHPROGTEXTUREWIDGET_H
#define SHPROGTEXTUREWIDGET_H

#include "shprogwidgetabstract.h"
#include <QStringList>
#include <QLineEdit>
#include <QComboBox>

class ShProgTextureWidget : public ShProgWidgetAbstract
{
    Q_OBJECT
public:
    explicit ShProgTextureWidget(QString type, QWidget *parent = 0);

    QString getVariableValue();
    QString getTexture();

    virtual bool isValid();

    void setTextureNames(const QStringList names);
    void setTextureName(const QString name);
    void setPointName(const QString name);
    
signals:
    void textureChanged(const QString point, const QString newTexture);
    void editingFinished(const QString oldName, const QString newName);
    
public slots:

private slots:
    void textureChangedSlot(QString texName);
    void editingFinishedSlot();

private:
    QLineEdit* pointName;
    QComboBox* textureNames;
    QString oldPoint;
    
};

#endif // SHPROGTEXTUREWIDGET_H
