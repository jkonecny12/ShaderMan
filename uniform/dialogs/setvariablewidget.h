#ifndef SETVARIABLEWIDGET_H
#define SETVARIABLEWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

/**
 * @brief The SetVariableWidget class Widget cointaining other widgets for better gui work
 *  Inside of this are widgets in one row of UniformVariableEditor
 */
class SetVariableWidget : public QWidget
{
    Q_OBJECT
public:
    enum WIDGETS {Uniform_label, Uniform_comboBox, Program_name, ShaderName_LineEdit, Delete_PushButton};
    explicit SetVariableWidget(QWidget *parent = 0);
    QWidget* getWidget(WIDGETS widget);
    void addLine(QString uniformName, QString programName, QString variableName);
    QString getUniformName();
    QString getProgramName();
    QString getVariableName();

private:
    void initialize();

private:
    QLabel* varTypeLabel;
    QComboBox* uniformNameBox;
    QComboBox* programNameBox;
    QLineEdit* varNameLineEdit;
    QPushButton* deleteButton;

signals:
    void deleteWidget(QWidget* widget);

private slots:
    void deletePush();
    void setUniformType(QString name);
    
};

#endif // SETVARIABLEWIDGET_H
