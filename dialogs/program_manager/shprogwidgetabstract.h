#ifndef SHPROGWIDGETABSTRACT_H
#define SHPROGWIDGETABSTRACT_H

#include <QWidget>

class ShProgWidgetAbstract : public QWidget
{
    Q_OBJECT
public:
    explicit ShProgWidgetAbstract(QString type, QWidget *parent = 0);

    /**
     * @brief ShProgWidgetAbstract::getVariableValue Abstract get method for override
     * @return Return Value of this widget
     */
    virtual QString getVariableValue() = 0;
    virtual bool isValid() = 0;
    
signals:
    void removeBtn(QString type);
    
public slots:

private slots:
    void removeBtnSlot();

protected:
    QString type;
    
};

#endif // SHPROGWIDGETABSTRACT_H
