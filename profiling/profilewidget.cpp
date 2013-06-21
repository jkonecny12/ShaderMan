#include "profilewidget.h"
#include "ui_profilewidget.h"
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#define ALL tr("ALL")

/**
 * @brief ProfileWidget::ProfileWidget Constructor
 * @param parent Parent of this QObject child
 */
ProfileWidget::ProfileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfileWidget),
    numberDrawings(100)
{
    ui->setupUi(this);

    // set correct labels
    ui->averageLabel->setText("Average " + QString::fromUtf8("µ") + "s:");
    ui->maxLabel->setText("Max " + QString::fromUtf8("µ") + "s:");
    ui->minLabel->setText("Min " + QString::fromUtf8("µ") + "s:");

    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom,tr("Last %1 renders").arg(numberDrawings));
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, QString("Draw time " + QString::fromUtf8("µ") + "s"));
    ui->qwtPlot->setAutoReplot(true);

    series = new TimeSeriesData(numberDrawings);

    curve = new QwtPlotCurve();
    QPen pen(QColor("blue"));
    curve->setPen(pen);
    curve->setData(series);
    curve->attach(ui->qwtPlot);

    ui->qwtPlot->resize(ui->qwtPlot->width(),100);
    resize(width(), 100);

    ui->shProgComboBox->addItem(ALL);
    ui->shProgComboBox->setCurrentIndex(0);

    allSet = true;

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->start(500);

    connect(timer,SIGNAL(timeout()),this,SLOT(getNewValues()));
    connect(ui->shProgComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setShaderProgram(QString)));
}

/**
 * @brief ProfileWidget::~ProfileWidget Destructor
 */
ProfileWidget::~ProfileWidget()
{
    delete ui;
}

/**
 * @brief ProfileWidget::setGLWindow Set OpenGL window. We get measure time from there.
 * @param window Our OpenGL window.
 */
void ProfileWidget::setGLWindow(OGLwindow *window)
{
    this->ogl = window;
}

/**
 * @brief ProfileWidget::setTimeList Set new shader program for measuring. Get needed time query object from OpenGL window.
 * @param progName Shader program name what we want measure now.
 */
void ProfileWidget::setTimeList(QString progName)
{
    if(progName != shProg && !allSet)
        return;

    timeList.clear();

    if(allSet)
    {
        QList<const TimeQueryStorage*> list = ogl->getTimeQueries();

        foreach(const TimeQueryStorage* s, list)
        {
            timeList.insert(s->getName(), s);
        }
    }
    else
    {
        const TimeQueryStorage* storage = ogl->getTimeQuery(shProg);

        if(storage != NULL)
            timeList.insert(shProg, storage);
    }
}

/**
 * @brief ProfileWidget::queryDestroyed Some query was destroyed, if we measure this query now, we need to remove it from list and
 * stop measure.
 * @param progName
 */
void ProfileWidget::queryDestroyed(QString progName)
{
    if(timeList.contains(progName))
        timeList.remove(progName);
}

/**
 * @brief ProfileWidget::addValue Added time data for plotting
 * @param time Drawing time data
 */
void ProfileWidget::addValue(double time)
{
    series->addData(time);
    ui->qwtPlot->replot();

    refreshStats();
}

/**
 * @brief ProfileWidget::insertShProgram Insert shader program to combobox for filter
 * @param name Name of the shader program
 */
void ProfileWidget::insertShProgram(QString name)
{
    ui->shProgComboBox->addItem(name);
}

/**
 * @brief ProfileWidget::refreshShPrograms Remove all shader programs from combobox and set new items from shPrograms
 * @param shPrograms List of shader programs to insert here
 */
void ProfileWidget::refreshShPrograms(QStringList shPrograms)
{
    ui->shProgComboBox->clear();
    ui->shProgComboBox->addItem(ALL);

    ui->shProgComboBox->addItems(shPrograms);
}

/**
 * @brief ProfileWidget::refreshStats Set new statistics
 */
void ProfileWidget::refreshStats()
{
    double av = series->getAverage();
    double min = series->getMinimum();
    double max = series->getMaximum();

    ui->averageNumLabel->setText(QString("%1").arg(av));
    ui->minNumLabel->setText(QString("%1").arg(min));
    ui->maxNumLabel->setText(QString("%1").arg(max));
}

/**
 * @brief ProfileWidget::getNewValues Get new values from measured objects.
 */
void ProfileWidget::getNewValues()
{
    if(timeList.isEmpty())
        return;

    double result = 0;

    foreach(const TimeQueryStorage* s, timeList)
    {
        result += s->getFinalTime();
    }

    if(result == 0)
        return;

    addValue(result);
}

/**
 * @brief ProfileWidget::setShaderProgram Set shader program what we want measure,
 * if ALL is set then measure all active shader programs.
 * @param name Shader program name we want measure.
 */
void ProfileWidget::setShaderProgram(QString name)
{
    if(name == ALL)
        allSet = true;
    else
        allSet = false;

    shProg = name;

    setTimeList(name);
}
