#ifndef TIMESERIESDATA_H
#define TIMESERIESDATA_H

#include <qwt_series_data.h>
#include <QPointF>
#include <QList>

class TimeSeriesData : public QwtSeriesData<QPointF>
{
public:
    explicit TimeSeriesData(int maxDrawings);
    
    void addData(double i);
    void clear();

    // get and set maximum
    inline void setMaxSampleNumber(int max) {this->max = max; }
    inline int getMaxSampleNumber() {return max;}

    virtual QRectF boundingRect() const;
    virtual QPointF sample(size_t i) const;
    virtual size_t size() const;

    double getAverage();
    double getMaximum();
    double getMinimum();

private:
    QList<double> drawings;
    int max;
    const int startX;
    
};

#endif // TIMESERIESDATA_H
