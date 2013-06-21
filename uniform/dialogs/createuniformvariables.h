#ifndef CREATEUNIFORMVARIABLES_H
#define CREATEUNIFORMVARIABLES_H

#include <QDialog>
#include <QComboBox>
#include <QStringList>
#include <QHash>
#include <QDebug>
#include <QTableView>
#include <QList>
#include <QVariant>
#include <QStandardItemModel>

#include <QMatrix4x4>
#include <QMatrix4x3>
#include <QMatrix4x2>
#include <QMatrix3x4>
#include <QMatrix3x3>
#include <QMatrix3x2>
#include <QMatrix2x4>
#include <QMatrix2x3>
#include <QMatrix2x2>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QLineEdit>
#include <QItemDelegate>

#include "comboboxdelegate.h"
#include "tools/lineeditdelegate.h"
#include "uniform/storage/uniformvariable.h"
#include "infomanager.h"

namespace Ui {
class CreateUniformVariables;
}

/**
 * @brief The CreateUniformVariables class Dialog for creation new uniform variables.
 */
class CreateUniformVariables : public QDialog
{
    Q_OBJECT

public:
    explicit CreateUniformVariables(QWidget *parent = 0);
    ~CreateUniformVariables();
    void addUniformVar(QString name);

private:
    void customConnect();
    void inputVarTypeToComboBox(QComboBox *box);
    void createModel(const int rows, const int columns, int stackedIndex);
    void removeAllStacked(bool createNew = true);
    void variableListInit();
    bool testModels(bool mark);
    void setModelType(UniformTypes::UNIFORM_TYPES type);

    QString getCurrentVariable();

signals:
    void addTable();

private slots:
    void arraySet(bool set);
    void modeSet();
    void createMultiplyModel();
    void typeSet(int index,int tabIndex = -1);
    void leftButtonSlot();
    void rightButtonSlot();
    void removeStackedPage();
    void addVariable();
    void removeVariable();
    void loadData(QModelIndex index);

    void generateVariable();

private:
    enum Mode {NONE, ARRAY, MULTIPLY};
    Ui::CreateUniformVariables *ui;
    bool isArray;
    //QWidget *widget;
    QList<QTableView*> tables;
    QList<QString> varList;
    int boxIndex;
    Mode mode;

    struct {
        UniformTypes::UNIFORM_TYPES uniformType; //exact type
        UniformTypes::UNIFORM_TYPES variableType; //float,bool...
        UniformTypes::UNIFORM_TYPES variableSize; //vec2,mat3...
        QList<QVariant> variableList;
    } UnifStorage;

};

#endif // CREATEUNIFORMVARIABLES_H
