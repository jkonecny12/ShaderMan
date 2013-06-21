#include "datatimer.h"

/**
 * @brief DataTimer::DataTimer Create custom QTimer object with special data.
 * @param timerId Id of this timer, can be ducplicated.
 * @param type Type of this Timer.
 * @param parent Parent of this QObject.
 */
DataTimer::DataTimer(long timerId, TIMER_TYPE type, QObject *parent) :
    QTimer(parent)
{
    this->id = timerId;
    this->type = type;

    connect(this,SIGNAL(timeout()),this,SLOT(sendNewSignal()));
}

/**
 * @brief DataTimer::getTimerId Return id of this timer.
 * @return Id of this timer.
 */
long DataTimer::getTimerId() const
{
    return id;
}

/**
 * @brief DataTimer::getTimerType Return type of this timer.
 * @return Type of this timer.
 */
DataTimer::TIMER_TYPE DataTimer::getTimerType() const
{
    return type;
}

/**
 * @brief DataTimer::sendNewSignal When timeout signal was emited, emit new timeout signal with id.
 */
void DataTimer::sendNewSignal()
{
    emit timeout(id);
}

