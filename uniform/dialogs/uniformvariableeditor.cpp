#include "uniformvariableeditor.h"
#include "ui_uniformvariableeditor.h"

UniformVariableEditor::UniformVariableEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UniformVariableEditor),
    layerIncY(30)
{
    ui->setupUi(this);

    layerY = 30;

    vbox = new QVBoxLayout(ui->scrollArea);
    ui->scrollWidget->setLayout(vbox);
    ui->scrollArea->setWidget(ui->scrollWidget);
    ui->scrollArea->setWidgetResizable(true);

    customConnect();
}

void UniformVariableEditor::customConnect()
{
    connect(ui->addLineButton,SIGNAL(clicked()),this,SLOT(addNewUniformWidgetLine()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(reject()));
}

UniformVariableEditor::~UniformVariableEditor()
{
    delete ui;
}

void UniformVariableEditor::setUniformSettings(QList<UniformVariableEditor::UniformAttachment> list)
{
    foreach(UniformVariableEditor::UniformAttachment a, list)
    {

        SetVariableWidget* widget = new SetVariableWidget(ui->scrollWidget);

        vbox->addWidget(widget);

        connect(widget,SIGNAL(deleteWidget(QWidget*)),this,SLOT(removeUniformWidgetLine(QWidget*)));

        lines.append(widget);

        //set default values
        widget->addLine(a.uniformName,a.programName,a.variableName);
    }

    // recalculate for proper scroll bar
    ui->scrollWidget->resize(ui->scrollWidget->sizeHint());
    qApp->processEvents();
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}

/**
 * @brief UniformVariableEditor::getUniformSettings Return list of assigned uniform variables
 * @return List of struct with all neede information
 */
QList<UniformVariableEditor::UniformAttachment> UniformVariableEditor::getUniformSettings()
{
    QList<UniformAttachment> ret;
    /*QWidget* widgetChild;*/

    foreach(SetVariableWidget* widget, lines)
    {
        UniformAttachment val;
        val.programName = widget->getProgramName();

        if(val.programName.isEmpty())
            continue;

        val.uniformName = widget->getUniformName();

        if(val.uniformName.isEmpty())
            continue;

        val.variableName = widget->getVariableName();

        if(val.variableName.isEmpty())
            continue;

        ret.append(val);
    }

    return ret;
}

/**
 * @brief UniformVariableEditor::getRemovedSettings Return uniform variable attachment which was removed from dialog.
 * @return List of removed attachments
 */
QList<UniformVariableEditor::UniformAttachment> UniformVariableEditor::getRemovedSettings()
{
    return removed;
}


/** SLOTS **/

/**
  SLOT
  Add new line to with widgets to menu
  */
void UniformVariableEditor::addNewUniformWidgetLine()
{
    SetVariableWidget* widget = new SetVariableWidget(ui->scrollWidget);

    // recalculate for proper scroll bar
    vbox->addWidget(widget);
    ui->scrollWidget->resize(ui->scrollWidget->sizeHint());
    qApp->processEvents();
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());

    connect(widget,SIGNAL(deleteWidget(QWidget*)),this,SLOT(removeUniformWidgetLine(QWidget*)));

    lines.append(widget);
}

/**
 * @brief UniformVariableEditor::removeUniformWidgetLine Remove one line with widgets
 * @param widget Pointer to line which user want to remove
 */
void UniformVariableEditor::removeUniformWidgetLine(QWidget *widget)
{
    SetVariableWidget* myWidget = static_cast<SetVariableWidget*>(widget);

    //add to removed field
    UniformAttachment at;
    at.programName = myWidget->getProgramName();
    at.uniformName = myWidget->getUniformName();
    at.variableName = myWidget->getVariableName();
    removed.append(at);

    //remove widget from scroll widget
    vbox->removeWidget(widget);
    lines.removeOne(myWidget);
    delete widget;

    // recalculate for proper scroll bar
    ui->scrollWidget->resize(ui->scrollWidget->sizeHint());
    qApp->processEvents();
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}
