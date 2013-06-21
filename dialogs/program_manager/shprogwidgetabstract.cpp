#include "shprogwidgetabstract.h"
#include <QHBoxLayout>
#include <QPushButton>

/**
 * @brief ShProgWidgetAbstract::ShProgWidgetAbstract Create abstract widget with remove button and signal for remove
 * @param parent Parent of this widget
 */
ShProgWidgetAbstract::ShProgWidgetAbstract(QString type, QWidget *parent) :
    QWidget(parent),
    type(type)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    this->setLayout(layout);

    QPushButton* rmButton = new QPushButton(this);
    QIcon icon(":/button_delete.png");
    rmButton->setIcon(icon);
    rmButton->setIconSize(icon.actualSize(QSize(25,25)));

    layout->addWidget(rmButton);

    connect(rmButton,SIGNAL(clicked()),this,SLOT(removeBtnSlot()));
}

/**
 * @brief ShProgWidgetAbstract::removeBtnSlot Emit remove button with setted type.
 */
void ShProgWidgetAbstract::removeBtnSlot()
{
    emit removeBtn(type);
}
