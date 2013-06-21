#include "timequerystorage.h"

/**
 * @brief TimeQueryStorage::TimeQueryStorage Create new query object. Set canUse flag to true and zore measured times.
 * @param progName Shader program name what this object measuring.
 */
TimeQueryStorage::TimeQueryStorage(QString progName) :
    progName(progName),
    canUse(true)
{
    time = 0;
    finalTime = 0;
}

/**
 * @brief TimeQueryStorage::getName Return shader program name setted to this object.
 * @return Shader program name.
 */
QString TimeQueryStorage::getName() const
{
    return progName;
}

/**
 * @brief TimeQueryStorage::pushQuery Save new query to back of list.
 * @param queryId Id of created OpenGL query.
 */
void TimeQueryStorage::pushQuery(const uint queryId)
{
    query.push_back(queryId);
}

/**
 * @brief TimeQueryStorage::topQuery Return oldest OpenGL query id.
 * @return Return OpenGL query id.
 */
uint TimeQueryStorage::topQuery() const
{
    return query.front();
}

/**
 * @brief TimeQueryStorage::popQuery Return oldest OpenGL query id and remove it.
 * @return Return OpenGL query id.
 */
uint TimeQueryStorage::popQuery()
{
    uint ret = query.front();
    query.removeFirst();
    return ret;
}

/**
 * @brief TimeQueryStorage::getQuerySize Get number of saved queries.
 * @return Number of saved queries.
 */
int TimeQueryStorage::getQuerySize() const
{
    return query.size();
}

/**
 * @brief TimeQueryStorage::setTime Set new measured time (not final time, it can be only part of final time).
 * @param time Measured time.
 */
void TimeQueryStorage::setTime(double time)
{
    this->time = time;
}

/**
 * @brief TimeQueryStorage::getTime Return measured time (not final time, can be only part of final value).
 * @return Measured time.
 */
double TimeQueryStorage::getTime() const
{
    return time;
}

/**
 * @brief TimeQueryStorage::getFinalTime Return final time of last drawing with this shader program.
 * @return Final time of last drawing.
 */
double TimeQueryStorage::getFinalTime() const
{
    return finalTime;
}

/**
 * @brief TimeQueryStorage::isQueryEmpty Is this query object empty.
 * @return True if no query is saved here, false otherwise.
 */
bool TimeQueryStorage::isQueryEmpty() const
{
    return query.isEmpty();
}

/**
 * @brief TimeQueryStorage::testUsage Test if this object can be used for creating new queries.
 * If this object have empty query list then it can be used and setting canUse flag to true, to false otherwise.
 */
void TimeQueryStorage::testUsage()
{
    canUse = isQueryEmpty();

    // we get all values from last draw
    // save time to final
    if(canUse)
    {
        finalTime = time;
        time = 0;
    }
}
