#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "timeseriesdata.h"
#include <qwt_plot_curve.h>
#include "oglwindow.h"

namespace Ui {
class ProfileWidget;
}

class ProfileWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProfileWidget(QWidget *parent = 0);
    ~ProfileWidget();

    void setGLWindow(OGLwindow* window);

public slots:
    void addValue(double time);
    void insertShProgram(QString name);
    void refreshShPrograms(QStringList shPrograms);
    void refreshStats();
    void setTimeList(QString progName);
    void queryDestroyed(QString progName);

private slots:
    void getNewValues();
    void setShaderProgram(QString name);
    
private:
    Ui::ProfileWidget *ui;
    int numberDrawings;
    TimeSeriesData* series;
    QwtPlotCurve* curve;
    OGLwindow* ogl;
    QHash<QString,const TimeQueryStorage*> timeList;
    QString shProg;
    bool allSet;
};

#endif // PROFILEWIDGET_H
