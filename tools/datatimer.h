#ifndef DATATIMER_H
#define DATATIMER_H

#include <QTimer>

class DataTimer : public QTimer
{
    Q_OBJECT
public:
    enum TIMER_TYPE {TIME, ACTION_PRESSED};

    explicit DataTimer(long timerId, TIMER_TYPE type, QObject *parent = 0);

    long getTimerId() const;
    TIMER_TYPE getTimerType() const;
    
signals:
    void timeout(long id);
    
public slots:

private slots:
    void sendNewSignal();

private:
    long id;
    TIMER_TYPE type;
    
};

#endif // DATATIMER_H
