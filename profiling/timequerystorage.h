#ifndef TIMEQUERYSTORAGE_H
#define TIMEQUERYSTORAGE_H

#include <QString>
#include <QList>
#include <QStack>

class TimeQueryStorage
{
public:
    TimeQueryStorage(QString progName);

    QString getName() const;

    void pushQuery(const uint queryId);
    uint topQuery() const;
    uint popQuery();

    int getQuerySize() const;

    void setTime(double time);
    double getTime() const;

    //void setFinalTime(double time);
    double getFinalTime() const;

    bool isQueryEmpty() const;

    void testUsage();
    /**
     * @brief isUsable If this flag is set to true this object can be used for another measuring,
     * if not you will get bad values with old and new measures in one value.
     * @return Return true if can be used, false otherwise.
     */
    inline bool isUsable() const {return this->canUse;}

private:
    const QString progName;
    double time;
    double finalTime;
    QList<uint> query;
    bool canUse;
};

#endif // TIMEQUERYSTORAGE_H
