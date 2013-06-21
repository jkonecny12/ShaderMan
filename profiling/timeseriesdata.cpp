#include "timeseriesdata.h"

/**
 * @brief TimeSeriesData::TimeSeriesData Constructor
 * @param maxDrawings Set max number of remembered drawings
 */
TimeSeriesData::TimeSeriesData(int maxDrawings) :
    QwtSeriesData<QPointF>(),
    startX(0)
{
    max = maxDrawings;
}

/**
 * @brief TimeSeriesData::addData Add plotting data to this model nad convert this from nano seconds to micro seconds
 * @param i Data for plotting
 */
void TimeSeriesData::addData(double i)
{
    i /= 1000;

    drawings.append(i);

    if(drawings.size() > max+1)
        drawings.removeFirst();
}

/**
 * @brief TimeSeriesData::clear Clear measured drawings.
 */
void TimeSeriesData::clear()
{
    drawings.size();
}

/**
 * @brief TimeSeriesData::boundingRect Calculated rectangle for bounding box of this data
 * @return Return calculated rectangle
 */
QRectF TimeSeriesData::boundingRect() const
{
    double maxY = drawings.value(0);
    //double minY = maxY;

    foreach(double i, drawings)
    {
        /*
        if(minY > i)
            minY = i;
        */

        if(maxY < i)
            maxY = i;
    }

    QRectF bound(startX, 0.0, max, maxY );

    return bound;
}

/**
 * @brief TimeSeriesData::sample Return sample for plotting
 * @param i Identifier of sample
 * @return Return point in graph
 */
QPointF TimeSeriesData::sample(size_t i) const
{
    return QPointF(i,drawings.value(i));
}

/**
 * @brief TimeSeriesData::size Get size of this data
 * @return Return number of samples
 */
size_t TimeSeriesData::size() const
{
    return drawings.size();
}

/**
 * @brief TimeSeriesData::getAverage Get samples average
 * @return Return average from these samples
 */
double TimeSeriesData::getAverage()
{
    double average = 0.0;

    foreach(double d, drawings)
    {
        average += d;
    }

    average = average / drawings.size();

    return average;
}

/**
 * @brief TimeSeriesData::getMaximum Get maximum from all samples
 * @return Return maximum
 */
double TimeSeriesData::getMaximum()
{
    double ret = drawings.value(0);

    foreach(double min, drawings)
    {
        if(ret < min)
            ret = min;
    }

    return ret;
}

/**
 * @brief TimeSeriesData::getMinimum Get minimum from all samples
 * @return Return minimum
 */
double TimeSeriesData::getMinimum()
{
    double ret = drawings.value(0);

    foreach(double max, drawings)
    {
        if(ret > max)
            ret = max;
    }

    return ret;
}
