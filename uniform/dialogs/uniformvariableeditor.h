#ifndef UNIFORMVARIABLEEDITOR_H
#define UNIFORMVARIABLEEDITOR_H

#include <QDialog>
#include <QDebug>
#include <QComboBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QWidget>
#include "setvariablewidget.h"

namespace Ui {
class UniformVariableEditor;
}

class UniformVariableEditor : public QDialog
{
    Q_OBJECT

public:
    struct UniformAttachment {
        QString uniformName;
        QString programName;
        QString variableName;
    };
    
public:
    explicit UniformVariableEditor(QWidget *parent = 0);
    ~UniformVariableEditor();
    void setUniformSettings(QList<UniformAttachment> list);
    QList<UniformAttachment> getUniformSettings();
    QList<UniformAttachment> getRemovedSettings();

private:
    void customConnect();
    
private:
    Ui::UniformVariableEditor *ui;
    const int layerIncY;
    int layerY;
    QVBoxLayout* vbox;
    QList<SetVariableWidget*> lines;
    QList<UniformAttachment> removed;

private slots:
    void addNewUniformWidgetLine();
    void removeUniformWidgetLine(QWidget* widget);
};

#endif // UNIFORMVARIABLEEDITOR_H
