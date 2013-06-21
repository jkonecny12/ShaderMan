#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include <QDialog>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QSet>
#include "infomanager.h"
#include "shprogvariablewidget.h"
#include "shprogtexturewidget.h"

namespace Ui {
class ProgramManagerDialog;
}

class ProgramManagerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ProgramManagerDialog(QWidget *parent = 0);
    ~ProgramManagerDialog();

private:
    void setOldProgName();
    MetaShaderProg* getActiveShProgram();
    bool testWidgets(QSet<ShProgWidgetAbstract *> widgets);

    QList<uint> getPositions(QSet<ShProgVariableWidget*> widgets);
    uint firstFreePosition(QList<uint> positions);

private slots:
    void addColorsWidget();
    void addColorsWidget(const uint position, const QString variable = "");
    void addTexCoordsWidget();
    void addTexCoordsWidget(const uint position, const QString variable = "");
    void addTextureWidget();
    void addTextureWidget(const QString textureVariable, const QString pointVariable = "");

    void removeWidget(QString type);
    void removeAllWidgets();

    void addShaderProgram();
    void removeShaderProgram();

    void setDefaultValues(QModelIndex current);
    void testProgramName();

    void vertexShaderSet(int currentIndex);
    void fragmentShaderSet(int currentIndex);

    void verticesVariableSet();
    void normalsVariableSet();

    void colorsVariableSet(QString oldName, QString newName, uint position);
    void colorsVariableRemoved(const QString name);

    void texCoordsVariableSet(QString oldName, QString newName, uint position);
    void texCoordsVariableRemoved(const QString name);

    void textureVariableSet(QString point, QString newTexture);
    void texturePointVariableSet(QString oldPoint, QString newPoint);
    void textureVariableRemoved(const QString name);

    void testColorPosition(QString name, uint position);
    void testTexCoordPosition(QString name, uint position);
    
private:
    Ui::ProgramManagerDialog *ui;
    QVBoxLayout* colorsLayout;
    QVBoxLayout* texCoordsLayout;
    QVBoxLayout* textureLayout;
    QSet<ShProgVariableWidget*> colorsWidgets;
    QSet<ShProgVariableWidget*> texCoordsWidgets;
    QSet<ShProgTextureWidget*> textureWidgets;
    InfoManager* info;
};

#endif // PROGRAMMANAGER_H
