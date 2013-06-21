#ifndef SHPROGVARIABLEWIDGET_H
#define SHPROGVARIABLEWIDGET_H

#include <QLineEdit>
#include "shprogwidgetabstract.h"

class ShProgVariableWidget : public ShProgWidgetAbstract
{
    Q_OBJECT
public:
    explicit ShProgVariableWidget(QString type, QWidget *parent = 0);
    
    virtual QString getVariableValue();
    void setVariable(const QString variable);
    uint getPosition();
    void setPosition(uint pos);

    bool isValid();

signals:
    void editingFinished(const QString oldValue, const QString newValue, const uint position);
    void positionChanged(const QString mainValue, const uint oldPosition);

public slots:

private slots:
    void editingFinishedSlot();
    void positionChangedSlot();

private:
    QLineEdit* edit;
    QLineEdit* position;
    QString old;
    
};

#endif // SHPROGVARIABLEWIDGET_H
