#include "createuniformvariables.h"
#include "ui_createuniformvariables.h"
#include "uniform/models/standardmodelunifvalidity.h"
#include "uniform/dialogs/generateVariableDialogs/selecttypedialog.h"
#include "uniform/models/multiplyunifmodel.h"
#include "uniform/models/combounifnamesdelegate.h"
#include <QStandardItem>
#include <QtAlgorithms>
#include <QMessageBox>

using namespace UniformTypes;

/**
 * @brief CreateUniformVariables::CreateUniformVariables Create new dialog for creation of uniform variables.
 * @param parent Parent of this dialog.
 */
CreateUniformVariables::CreateUniformVariables(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateUniformVariables)
{
    ui->setupUi(this);
    customConnect();
    isArray = false;

    mode = NONE;

    ui->noneRadio->setChecked(true);

    inputVarTypeToComboBox(ui->varTypesComboBox);
    ui->arrayControlWidget->hide();
    //ui->addVariableButton->hide();
    ui->removePageButton->hide();

    tables.append(new QTableView(this));

    //boxDelegate = new ComboBoxDelegate(this);
    //lineDelegate = new LineEditDelegate(LineEditDelegate::FLOAT,this);


    tables[0]->setItemDelegate(new LineEditDelegate(LineEditDelegate::FLOAT,this));

    //QStandardItemModel* model = new QStandardItemModel(tables[0]);
    int index = ui->varTypesComboBox->currentIndex();
    QVariant var = ui->varTypesComboBox->itemData(index,Qt::UserRole+2);
    UNIFORM_TYPES sType = static_cast<UNIFORM_TYPES>(var.toInt());

    StandardModelUnifValidity* model = new StandardModelUnifValidity(sType, tables[0]);

    //model->appendRow(new QStandardItem("true"));
    tables[0]->setModel(model);

    //createModel(1,1,false,0);
    createModel(1,1,0);

    ui->setVarStackWidget->addWidget(tables[0]);
    tables[0]->show();

    //save current index number (more times change to same value)
    boxIndex = ui->varTypesComboBox->currentIndex();

    variableListInit();

    QList<int> list;
    list << 30 << 200;

    ui->splitter->setSizes(list);
}

/**
 * @brief CreateUniformVariables::~CreateUniformVariables Destroy this dialog and free all resources.
 */
CreateUniformVariables::~CreateUniformVariables()
{
    delete ui;
    qDeleteAll(tables);
}

/**
  Connect signals and slots
  */
void CreateUniformVariables::customConnect()
{
    //connect(ui->arrayCheckBox,SIGNAL(toggled(bool)),this,SLOT(arraySet(bool)));
    //connect(ui->setVarStackWidget,SIGNAL(currentChanged(int)),this,SLOT(addTableSlot(int)));
    connect(ui->varTypesComboBox,SIGNAL(activated(int)),this,SLOT(typeSet(int)));
    connect(ui->leftButton,SIGNAL(clicked()),this,SLOT(leftButtonSlot()));
    connect(ui->rightButton,SIGNAL(clicked()),this,SLOT(rightButtonSlot()));
    connect(ui->addVariableButton,SIGNAL(clicked()),this,SLOT(addVariable()));
    connect(ui->removeVarBtn,SIGNAL(clicked()),this,SLOT(removeVariable()));
    connect(ui->removePageButton,SIGNAL(clicked()),this,SLOT(removeStackedPage()));
    connect(ui->uniformView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(loadData(QModelIndex)));
    connect(ui->generateVarButton,SIGNAL(clicked()),this,SLOT(generateVariable()));

    connect(ui->noneRadio,SIGNAL(clicked()),this,SLOT(modeSet()));
    connect(ui->arrayRadio,SIGNAL(clicked()),this,SLOT(modeSet()));
    connect(ui->multiplyRadio,SIGNAL(clicked()),this,SLOT(modeSet()));
}

/**
 * @brief CreateUniformVariables::addUniformVar Add uniform variable to model.
 * @param name Name of the new uniform variable.
 */
void CreateUniformVariables::addUniformVar(QString name)
{
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->uniformView->model());

    if(!model->findItems(name).isEmpty())
        return;


    model->appendRow(new QStandardItem(name));

    model->sort(0);
}

/**
  Input variable types to combobox
  */
void CreateUniformVariables::inputVarTypeToComboBox(QComboBox *box)
{
    QList<QStandardItem*> data;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(box->model());

    //UserRole+1 == exact uniform type (DMAT2, BVEC3...)
    //UserRole+2 == scalar type (FLOAT, DOUBLE ...)
    //UserRole+3 == uniform size (SCALAR, MAT2, VEC3...)

    int i = 0;
    //names.append("float"); types.append(FLOAT);
    data.append(new QStandardItem("float"));data[i]->setData(QVariant(FLOAT),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(SCALAR),Qt::UserRole+3);
    ++i;

    //names.append("double"); types.append(DOUBLE);
    //data.append(new QStandardItem("double"));data[i]->setData(QVariant(DOUBLE),Qt::UserRole+1);
    //data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(SCALAR),Qt::UserRole+3);
    //++i;

    //names.append("int"); types.append(INT);
    data.append(new QStandardItem("int"));data[i]->setData(QVariant(INT),Qt::UserRole+1);
    data[i]->setData(QVariant(INT),Qt::UserRole+2);data[i]->setData(QVariant(SCALAR),Qt::UserRole+3);
    ++i;
    //names.append("uint"); types.append(UINT);
    data.append(new QStandardItem("uint"));data[i]->setData(QVariant(UINT),Qt::UserRole+1);
    data[i]->setData(QVariant(UINT),Qt::UserRole+2);data[i]->setData(QVariant(SCALAR),Qt::UserRole+3);
    ++i;
    //names.append("bool"); types.append(BOOL);
    //data.append(new QStandardItem("bool"));data[i]->setData(QVariant(BOOL),Qt::UserRole+1);
    //data[i]->setData(QVariant(BOOL),Qt::UserRole+2);data[i]->setData(QVariant(SCALAR),Qt::UserRole+3);

    //++i;
    //names.append("vec2"); types.append(VEC2);
    data.append(new QStandardItem("vec2"));data[i]->setData(QVariant(VEC2),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(VEC2),Qt::UserRole+3);
    ++i;
    //names.append("vec3"); types.append(VEC3);
    data.append(new QStandardItem("vec3"));data[i]->setData(QVariant(VEC3),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(VEC3),Qt::UserRole+3);
    ++i;
    //names.append("vec4"); types.append(VEC4);
    data.append(new QStandardItem("vec4"));data[i]->setData(QVariant(VEC4),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(VEC4),Qt::UserRole+3);
    ++i;
    /*
    //names.append("ivec2"); types.append(IVEC2);
    data.append(new QStandardItem("ivec2"));data[i]->setData(QVariant(IVEC2),Qt::UserRole+1);
    data[i]->setData(QVariant(INT),Qt::UserRole+2);data[i]->setData(QVariant(VEC2),Qt::UserRole+3);
    ++i;
    //names.append("ivec3"); types.append(IVEC3);
    data.append(new QStandardItem("ivec3"));data[i]->setData(QVariant(IVEC3),Qt::UserRole+1);
    data[i]->setData(QVariant(INT),Qt::UserRole+2);data[i]->setData(QVariant(VEC3),Qt::UserRole+3);
    ++i;
    //names.append("ivec4"); types.append(IVEC4);
    data.append(new QStandardItem("ivec4"));data[i]->setData(QVariant(IVEC4),Qt::UserRole+1);
    data[i]->setData(QVariant(INT),Qt::UserRole+2);data[i]->setData(QVariant(VEC4),Qt::UserRole+3);
    ++i;
    */
    //names.append("bvec2"); types.append(BVEC2);
    //data.append(new QStandardItem("bvec2"));data[i]->setData(QVariant(BVEC2),Qt::UserRole+1);
    //data[i]->setData(QVariant(BOOL),Qt::UserRole+2);data[i]->setData(QVariant(VEC2),Qt::UserRole+3);
    //++i;
    //names.append("bvec3"); types.append(BVEC3);
    //data.append(new QStandardItem("bvec3"));data[i]->setData(QVariant(BVEC3),Qt::UserRole+1);
    //data[i]->setData(QVariant(BOOL),Qt::UserRole+2);data[i]->setData(QVariant(VEC3),Qt::UserRole+3);
    //++i;
    //names.append("bvec4"); types.append(BVEC4);
    //data.append(new QStandardItem("bvec4"));data[i]->setData(QVariant(BVEC4),Qt::UserRole+1);
    //data[i]->setData(QVariant(BOOL),Qt::UserRole+2);data[i]->setData(QVariant(VEC4),Qt::UserRole+3);
    //++i;
    /*
    //names.append("dvec2"); types.append(DVEC2);
    data.append(new QStandardItem("dvec2"));data[i]->setData(QVariant(DVEC2),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(VEC2),Qt::UserRole+3);
    ++i;
    //names.append("dvec3"); types.append(DVEC3);
    data.append(new QStandardItem("dvec3"));data[i]->setData(QVariant(DVEC3),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(VEC3),Qt::UserRole+3);
    ++i;
    //names.append("dvec4"); types.append(DVEC4);
    data.append(new QStandardItem("dvec4"));data[i]->setData(QVariant(DVEC4),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(VEC4),Qt::UserRole+3);
    ++i;
    */

    /*
    //names.append("uvec2"); types.append(UVEC2);
    data.append(new QStandardItem("uvec2"));data[i]->setData(QVariant(UVEC2),Qt::UserRole+1);
    data[i]->setData(QVariant(UINT),Qt::UserRole+2);data[i]->setData(QVariant(VEC2),Qt::UserRole+3);
    ++i;
    //names.append("uvec3"); types.append(UVEC3);
    data.append(new QStandardItem("uvec3"));data[i]->setData(QVariant(UVEC3),Qt::UserRole+1);
    data[i]->setData(QVariant(UINT),Qt::UserRole+2);data[i]->setData(QVariant(VEC3),Qt::UserRole+3);
    ++i;
    //names.append("uvec4"); types.append(UVEC4);
    data.append(new QStandardItem("uvec4"));data[i]->setData(QVariant(UVEC4),Qt::UserRole+1);
    data[i]->setData(QVariant(UINT),Qt::UserRole+2);data[i]->setData(QVariant(VEC4),Qt::UserRole+3);
    ++i;
    */

    //names.append("mat2"); types.append(MAT2);
    data.append(new QStandardItem("mat2"));data[i]->setData(QVariant(MAT2),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT2),Qt::UserRole+3);
    ++i;
    //names.append("mat3"); types.append(MAT3);
    data.append(new QStandardItem("mat3"));data[i]->setData(QVariant(MAT3),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT3),Qt::UserRole+3);
    ++i;
    //names.append("mat4"); types.append(MAT4);
    data.append(new QStandardItem("mat4"));data[i]->setData(QVariant(MAT4),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT4),Qt::UserRole+3);
    ++i;
    /*
    ++i;
    //names.append("dmat2"); types.append(DMAT2);
    data.append(new QStandardItem("dmat2"));data[i]->setData(QVariant(DMAT2),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT2),Qt::UserRole+3);
    ++i;
    //names.append("dmat3"); types.append(DMAT3);
    data.append(new QStandardItem("dmat3"));data[i]->setData(QVariant(DMAT3),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT3),Qt::UserRole+3);
    ++i;
    //names.append("dmat4"); types.append(DMAT4);
    data.append(new QStandardItem("dmat4"));data[i]->setData(QVariant(DMAT4),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT4),Qt::UserRole+3);
    ++i;
    //names.append("dmat2"); types.append(DMAT2);
    data.append(new QStandardItem("dmat2"));data[i]->setData(QVariant(IMAT2),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT2),Qt::UserRole+3);
    ++i;
    //names.append("dmat3"); types.append(DMAT3);
    data.append(new QStandardItem("dmat3"));data[i]->setData(QVariant(IMAT3),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT3),Qt::UserRole+3);
    ++i;
    //names.append("dmat4"); types.append(DMAT4);
    data.append(new QStandardItem("dmat4"));data[i]->setData(QVariant(IMAT4),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT4),Qt::UserRole+3);
    ++i;
    //names.append("dmat2"); types.append(DMAT2);
    data.append(new QStandardItem("dmat2"));data[i]->setData(QVariant(UMAT2),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT2),Qt::UserRole+3);
    ++i;
    //names.append("dmat3"); types.append(DMAT3);
    data.append(new QStandardItem("dmat3"));data[i]->setData(QVariant(UMAT3),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT3),Qt::UserRole+3);
    ++i;
    //names.append("dmat4"); types.append(DMAT4);
    data.append(new QStandardItem("dmat4"));data[i]->setData(QVariant(UMAT4),Qt::UserRole+1);
    data[i]->setData(QVariant(DOUBLE),Qt::UserRole+2);data[i]->setData(QVariant(MAT4),Qt::UserRole+3);
    */

    /*
    //names.append("mat2x3"); types.append(MAT2X3);
    data.append(new QStandardItem("mat2x3"));data[i]->setData(QVariant(MAT2X3),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT2X3),Qt::UserRole+3);
    ++i;
    //names.append("mat2x4"); types.append(MAT2X4);
    data.append(new QStandardItem("mat2x4"));data[i]->setData(QVariant(MAT2X4),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT2X4),Qt::UserRole+3);
    ++i;
    //names.append("mat3x2"); types.append(MAT3X2);
    data.append(new QStandardItem("mat3x2"));data[i]->setData(QVariant(MAT3X2),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT3X2),Qt::UserRole+3);
    ++i;
    //names.append("mat3x4"); types.append(MAT3X4);
    data.append(new QStandardItem("mat3x4"));data[i]->setData(QVariant(MAT3X4),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT3X4),Qt::UserRole+3);
    ++i;
    //names.append("mat4x2"); types.append(MAT4X2);
    data.append(new QStandardItem("mat4x2"));data[i]->setData(QVariant(MAT4X2),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT4X2),Qt::UserRole+3);
    ++i;
    //names.append("mat4x3"); types.append(MAT4X3);
    data.append(new QStandardItem("mat4x3"));data[i]->setData(QVariant(MAT4X3),Qt::UserRole+1);
    data[i]->setData(QVariant(FLOAT),Qt::UserRole+2);data[i]->setData(QVariant(MAT4X3),Qt::UserRole+3);
    */

    /*
    //names.append("dmat2x3"); types.append(DMAT2X3);
    data.append(new QStandardItem("dmat2x3"));data[0]->setData(QVariant(MAT2X3));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);

    //names.append("dmat2x4"); types.append(DMAT2X4);
    data.append(new QStandardItem("dmat2x4"));data[0]->setData(QVariant(MAT2X4));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);

    //names.append("dmat3x2"); types.append(DMAT3X2);
    data.append(new QStandardItem("dmat3x2"));data[0]->setData(QVariant(MAT3X2));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);

    //names.append("dmat3x4"); types.append(DMAT3X4);
    data.append(new QStandardItem("dmat3x4"));data[0]->setData(QVariant(MAT3X4));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);

    //names.append("dmat4x2"); types.append(DMAT4X2);
    data.append(new QStandardItem("dmat4x2"));data[0]->setData(QVariant(MAT4X2));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);

    //names.append("dmat4x3"); types.append(DMAT4X3);
    data.append(new QStandardItem("dmat4x3"));data[0]->setData(QVariant(MAT4X3));
    data[0]->setData(QVariant(DOUBLE),Qt::UserRole+2);
    */
/*
    for(int i = 0; i < names.count(); ++i)
    {
        box->addItem(names[i],QVariant(types[i]));
    }
    */

    QStandardItem* item = model->invisibleRootItem();
    item->appendRows(data);

}

/**
 * @brief CreateUniformVariables::createModel Create new model for table.
 * @param rows How many rows the table will have.
 * @param columns How many columns the table will have.
 * @param isBoolean If the model will use bool types.
 * @param stackedIndex Where we get our old model in stack panel.
 */
void CreateUniformVariables::createModel(const int rows, const int columns,
                                         int stackedIndex)
{
    if(mode == MULTIPLY)
        return;

    QStandardItemModel* model = static_cast<QStandardItemModel*>(tables[stackedIndex]->model());
    model->clear();
    //model->setRowCount(rows);

    /*
    if(isBoolean)
    {
        QList<QStandardItem*> inData;

        for(int row = 0; row < rows; ++row)
        {
            for(int column = 0; column < columns; ++column)
            {
                inData.append(new QStandardItem("true"));
            }

            model->appendRow(inData);
            inData.clear();
        }
    }
    else
    {
    */

    QList<QStandardItem*> inData;

    for(int row = 0; row < rows; ++row)
    {
        for(int column = 0; column < columns; ++column)
        {
            inData.append(new QStandardItem("0"));
        }

        model->appendRow(inData);
        inData.clear();
    }

    //}
}

/**
 * @brief CreateUniformVariables::removeAllStacked Remove all tables from stack widget.
 * @param createNew If we need to create new table.
 */
void CreateUniformVariables::removeAllStacked(bool createNew)
{
    //remove all old widgets
    foreach(QTableView* tab, tables)
    {
        ui->setVarStackWidget->removeWidget(tab);
        delete tab;
    }

    tables.clear();

    if(createNew)
    {
        //create new one
        QTableView* tab = new QTableView(this);
        tables.append(tab);

        tab->setItemDelegate(new LineEditDelegate(LineEditDelegate::FLOAT,this));

        int index = ui->varTypesComboBox->currentIndex();
        QVariant var = ui->varTypesComboBox->itemData(index,Qt::UserRole+2);
        UNIFORM_TYPES sType = static_cast<UNIFORM_TYPES>(var.toInt());

        StandardModelUnifValidity* model = new StandardModelUnifValidity(sType, tab);
        tab->setModel(model);

        //createModel(1,1,false,0);
        createModel(1,1,0);

        ui->setVarStackWidget->addWidget(tab);
        tab->show();

        //actualize label
        ui->pageNumLabel->setText(QString("1/1"));
    }
}

/**
 * @brief CreateUniformVariables::variableListInit Initialize variable list from existing variables
 */
void CreateUniformVariables::variableListInit()
{
    //int index = ui->varTypesComboBox->currentIndex();
    //QVariant var = ui->varTypesComboBox->itemData(index,Qt::UserRole+2);
    //UNIFORM_TYPES sType = static_cast<UNIFORM_TYPES>(var.toInt());

    QStandardItemModel* model = new QStandardItemModel(ui->uniformView);

    InfoManager* info = InfoManager::getInstance();

    QStringList list = info->getActiveProject()->getUniformNames();
    QStringListIterator it(list);


    while(it.hasNext())
    {
        model->appendRow(new QStandardItem(it.next()));
    }

    model->sort(0);

    ui->uniformView->setModel(model);
}


/**
 * @brief CreateUniformVariables::testModels Test all active table models for validity of uniform variables.
 * @param mark If true mark non-valid items in tables.
 * @return True if all tabs are valid, false otherwise.
 */
bool CreateUniformVariables::testModels(bool mark)
{
    bool isValid = true;

    foreach(QTableView* tab, tables)
    {
        StandardModelUnifValidity* model = static_cast<StandardModelUnifValidity*>(tab->model());

        if(!model->isValid(mark))
        {
            isValid = false;
            if(!mark)
                break;
        }
    }

    return isValid;
}

/**
 * @brief CreateUniformVariables::setModelType Set new type to all models.
 * @param type Scalar type we want to set. (INT, UINT, FLOT ...)
 */
void CreateUniformVariables::setModelType(UNIFORM_TYPES type)
{
    foreach(QTableView* tab, tables)
    {
        StandardModelUnifValidity* model = static_cast<StandardModelUnifValidity*>(tab->model());

        model->setType(type);
    }
}

/**
 * @brief CreateUniformVariables::getCurrentVariable Return name of the current selected uniform variable.
 * @return Name of the selected uniform variable.
 */
QString CreateUniformVariables::getCurrentVariable()
{
    return ui->uniformView->selectionModel()->currentIndex().data().toString();;
}

/** SLOTS **/

/**
  SLOT
  Array checkbox is set
  */
void CreateUniformVariables::arraySet(bool set)
{
    qDebug() << "array uniform variables set to " << set;

    //isArray = set;

    if(set)
    {
        ui->arrayControlWidget->show();
        //ui->addVariableButton->show();
        ui->removePageButton->show();
    }
    else
    {
        ui->arrayControlWidget->hide();
        //ui->addVariableButton->hide();
        ui->removePageButton->hide();
    }
}

/**
 * @brief CreateUniformVariables::modeSet If some radio buttons was clicked, then set apropriate mode.
 */
void CreateUniformVariables::modeSet()
{
    QObject* radio = sender();

    QString name = radio->objectName();

    if(name == "noneRadio")
    {
        if(mode == NONE)
            return;

        mode = NONE;
        isArray = false;
        ui->arrayRadio->setChecked(false);
        ui->multiplyRadio->setChecked(false);

        removeAllStacked();
        ui->varTypesComboBox->setCurrentIndex(0);
    }
    else if(name == "arrayRadio")
    {
        if(mode == ARRAY)
            return;

        mode = ARRAY;
        isArray = true;
        ui->noneRadio->setChecked(false);
        ui->multiplyRadio->setChecked(false);

        removeAllStacked();
        ui->varTypesComboBox->setCurrentIndex(0);
    }
    else if(name == "multiplyRadio")
    {
        if(mode == MULTIPLY)
            return;

        mode = MULTIPLY;
        isArray = false;
        ui->noneRadio->setChecked(false);
        ui->arrayRadio->setChecked(false);

        createMultiplyModel();
    }

    arraySet(isArray);
}

/**
 * @brief CreateUniformVariables::createMultiplyModel Create new multiply mode matrix.
 */
void CreateUniformVariables::createMultiplyModel()
{
    removeAllStacked(false);

    QTableView* tab = new QTableView(this);
    tables.append(tab);

    MetaProject* proj = InfoManager::getInstance()->getActiveProject();

    QStringList list = proj->getUniformNames();
    //QString name = ui->uniformView->selectionModel()->currentIndex().data().toString();
    QStringList listName;

    int id = ui->varTypesComboBox->currentIndex();
    UNIFORM_TYPES type = static_cast<UNIFORM_TYPES>(ui->varTypesComboBox->itemData(id, Qt::UserRole+1).toInt());

    for(int i = 0; i < list.size(); ++i)
    {
        //if(name == list.value(i))
        //    continue;

        UniformVariable* uVar = proj->getUniformVariable(list.value(i));

        if(uVar == NULL)
            continue;

        if(uVar->isMultiplyMode())
            continue;

        if(uVar->getUniformType() != type)
            continue;

        listName.append(list.value(i));
    }

    tab->setModel(new MultiplyUnifModel(tab));
    ui->setVarStackWidget->addWidget(tab);
    ComboUnifNamesDelegate* delegate = new ComboUnifNamesDelegate(listName, this);
    tab->setItemDelegate(delegate);
}

/**
 * SLOT
 * Add table or others to QStackedWidget
 *
 * @brief CreateUniformVariables::typeSet
 * @param index index of QComboBox active type
 */
void CreateUniformVariables::typeSet(int index, int tabIndex)
{
    if(mode == MULTIPLY)
    {
        createMultiplyModel();
        return;
    }

    //specific type
    QVariant var = ui->varTypesComboBox->itemData(index,Qt::UserRole+1);
    UNIFORM_TYPES utype = static_cast<UNIFORM_TYPES>(var.toInt());

    qDebug() << "Type set: " << utype;

    if(isArray && boxIndex != index)
    {
        qDebug() << "All stacked widget removed";
        removeAllStacked();
    }

    boxIndex = index;

    int tabId;
    //get tableId in Stacked widget
    if(tabIndex == -1)
        tabId = ui->setVarStackWidget->currentIndex();
    else
        tabId = tabIndex;

    delete (tables[tabId]->itemDelegate());

    //float,bool,etc..
    var = ui->varTypesComboBox->itemData(index,Qt::UserRole+2);
    UNIFORM_TYPES sType = static_cast<UNIFORM_TYPES>(var.toInt());

    //information need for creation
    int row;
    int column;
    LineEditDelegate::TYPE leType;
    //bool isBool = false;

    //what type
    switch(sType)
    {
    case INT:
        leType = LineEditDelegate::INT;
        break;

    case UINT:
        leType = LineEditDelegate::UINT;
        break;

    case FLOAT:
        leType = LineEditDelegate::FLOAT;
        break;

        /*
    case DOUBLE:
        leType = LineEditDelegate::DOUBLE;
        break;
        */

    default:
        leType = LineEditDelegate::FLOAT;
        break;
    }

    //is this only scalar
    var = ui->varTypesComboBox->itemData(index,Qt::UserRole+3);
    if(!var.isValid())
    {
        /*
        if(isBool)
        {
            createModel(1,1,true,tabId);
            tables[tabId]->setItemDelegate(new ComboBoxDelegate(tables[tabId]));
        }
        else
        {
        */
        //createModel(1,1,false,tabId);
        createModel(1,1,tabId);
        tables[tabId]->setItemDelegate(new LineEditDelegate(leType,tables[tabId]));
        //}
        return;
    }
    //it is matrix or vector

    UNIFORM_TYPES mType = static_cast<UNIFORM_TYPES>(var.toInt());

    //set table size
    switch(mType)
    {
    case VEC2:
        row = 1;
        column = 2;
        break;
    case VEC3:
        row = 1;
        column = 3;
        break;
    case VEC4:
        row = 1;
        column = 4;
        break;

    case MAT2:
        row = 2;
        column = 2;
        break;
    case MAT3:
        row = 3;
        column = 3;
        break;
    case MAT4:
        row = 4;
        column = 4;
        break;

    case MAT2X3:
        row = 3;
        column = 2;
        break;
    case MAT2X4:
        row = 4;
        column = 2;
        break;
    case MAT3X2:
        row = 2;
        column = 3;
        break;
    case MAT3X4:
        row = 4;
        column = 3;
        break;
    case MAT4X2:
        row = 2;
        column = 4;
        break;
    case MAT4X3:
        row = 3;
        column = 4;
        break;

    default:
        row = 1;
        column = 1;
        break;
    }

    //create needed model
    /*
    if(isBool)
    {
        createModel(row,column,true,tabId);
        tables[tabId]->setItemDelegate(new ComboBoxDelegate(tables[tabId]));
    }
    */
    //else
    //{
    //createModel(row,column,false,tabId);
    createModel(row,column,tabId);
    tables[tabId]->setItemDelegate(new LineEditDelegate(leType,tables[tabId]));
    //}

}

/**
 * @brief CreateUniformVariables::leftButtonSlot Reaction when user clicked on left button. Move to other table or do not move anywhere.
 */
void CreateUniformVariables::leftButtonSlot()
{
    int page = ui->setVarStackWidget->currentIndex();

    if(page >= 1)
    {
        page -= 1;
        ui->setVarStackWidget->setCurrentIndex(page);
    }

    ui->pageNumLabel->setText(QString("%1/%2").arg(page+1).arg(ui->setVarStackWidget->count()));
}

/**
 * @brief CreateUniformVariables::rightButtonSlot Reaction when user clicked on right button. Move to next table or create new and move.
 */
void CreateUniformVariables::rightButtonSlot()
{
    int page = ui->setVarStackWidget->currentIndex();

    if(page == ui->setVarStackWidget->count()-1)
    {
        qDebug() << "Adding page to stacked widget page" << page+1;

        QTableView* tab = new QTableView(this);
        tables.append(tab);

        tab->setItemDelegate(new LineEditDelegate(LineEditDelegate::FLOAT,this));

        int index = ui->varTypesComboBox->currentIndex();
        QVariant var = ui->varTypesComboBox->itemData(index,Qt::UserRole+2);
        UNIFORM_TYPES sType = static_cast<UNIFORM_TYPES>(var.toInt());

        StandardModelUnifValidity* model = new StandardModelUnifValidity(sType, tab);
        //model->appendRow(new QStandardItem("true"));
        tab->setModel(model);

        //createModel(1,1,false,page+1);

        ui->setVarStackWidget->insertWidget(page+1,tab);
        tab->show();

        typeSet(ui->varTypesComboBox->currentIndex(),page+1);
    }


    ui->setVarStackWidget->setCurrentIndex(page+1);

    ui->pageNumLabel->setText(QString("%1/%2").arg(page+2).arg(ui->setVarStackWidget->count()));
}

/**
 * @brief CreateUniformVariables::removeStackedPage Remove actual page from stack widget.
 */
void CreateUniformVariables::removeStackedPage()
{
    int count = ui->setVarStackWidget->count();
    if(count > 1)
    {
        int currentId = ui->setVarStackWidget->currentIndex();

        QWidget* widget = ui->setVarStackWidget->currentWidget();
        ui->setVarStackWidget->removeWidget(widget);
        tables.removeOne(static_cast<QTableView*>(widget));
        delete widget;

        if(currentId > 0)
            currentId -= 1;

        ui->setVarStackWidget->setCurrentIndex(currentId);
        ui->pageNumLabel->setText(QString("%1/%2").arg(currentId+1).arg(count-1));
    }
}

/**
 * @brief CreateUniformVariables::addVariable Add variable to list of uniform variables
 */
void CreateUniformVariables::addVariable()
{
    QString name = ui->varName->text();
    MetaProject* actProj = InfoManager::getInstance()->getActiveProject();

    if(name.isEmpty())
    {
        QMessageBox::critical(this,tr("Variable name must be set"),
                              tr("You must set variable name to continue."));
        return;
    }

    UniformVariable* var;

    if(mode != MULTIPLY)
    {

        if(!testModels(false))
        {
            QMessageBox::warning(this,tr("Variables are not correct"),
                                 tr("Please correct bad variable values in table!"));
            return;
        }

        QList<QVariant> list;

        QStandardItemModel* model;

        foreach(QTableView* view, tables)
        {
            model = static_cast<QStandardItemModel*>(view->model());

            for(int row = 0; row < model->rowCount(); ++row)
            {
                for(int column = 0; column < model->columnCount();++column)
                {
                    list.append(model->data(model->index(row,column)));
                }
            }
        }

        QComboBox* box = ui->varTypesComboBox;
        int index = box->currentIndex();

        if(actProj->hasUniformVariable(name))
        {
            var = actProj->getUniformVariable(name);

            var->loadVariableData(name,list,
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+2).toInt()),
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+3).toInt()),
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+1).toInt()));
        }
        else
        {
            //add variable to project
            var = new UniformVariable(name,list,
                                           static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+2).toInt()),
                                           static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+3).toInt()),
                                           static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+1).toInt()));

            if(!actProj->addUniformVariable(var))
            {
                actProj->removeUniformVariable(var->getName());
                actProj->addUniformVariable(var);
            }
        }

    }
    else // multiply mode
    {
        QStringList list;
        QList<QVariant> lVar;

        MultiplyUnifModel* model = static_cast<MultiplyUnifModel*>(tables.value(0)->model());

        list = model->getNameList();

        if(list.size() == 0)
        {
            QMessageBox box;
            box.setText(tr("You canno't save empty variable!"));
            box.setIcon(QMessageBox::Warning);
            box.exec();
            return;
        }

        foreach(QString name, list)
        {
            lVar.append(QVariant(name));
        }

        QComboBox* box = ui->varTypesComboBox;
        int index = box->currentIndex();

        if(actProj->hasUniformVariable(name))
        {
            var = actProj->getUniformVariable(name);

            var->loadVariableData(name,lVar,
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+2).toInt()),
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+3).toInt()),
                                  static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+1).toInt()),
                                  true);
        }
        else
        {

            var = new UniformVariable(name, lVar,
                                      static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+2).toInt()),
                                      static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+3).toInt()),
                                      static_cast<UNIFORM_TYPES>(box->itemData(index,Qt::UserRole+1).toInt()),
                                      true);

            if(!actProj->addUniformVariable(var))
            {
                actProj->removeUniformVariable(var->getName());
                actProj->addUniformVariable(var);
            }

        }

    }


    qDebug() << "Uniform variable: " << var->getName();

    addUniformVar(name);
}

/**
 * @brief CreateUniformVariables::removeVariable Remove uniform variable from list of uniform variables
 */
void CreateUniformVariables::removeVariable()
{
    QModelIndexList selected = ui->uniformView->selectionModel()->selectedIndexes();

    //no variable selected
    if(selected.isEmpty())
    {
        qDebug() << "Nothing selected";
        return;
    }

    //remove from project
    MetaProject* proj = InfoManager::getInstance()->getActiveProject();
    proj->removeUniformVariable(selected[0].data().toString());

    //remove from list
    ui->uniformView->model()->removeRow(selected[0].row());
}

/**
 * @brief CreateUniformVariables::loadData Load data from uniform variable to stacked widget
 * @param index Uniform variable name in list
 */
void CreateUniformVariables::loadData(QModelIndex index)
{
    InfoManager* info = InfoManager::getInstance();

    UniformVariable* val = info->getActiveProject()->getUniformVariable(index.data().toString());

    QList<QVariant> items = val->getValues();

    //set array variable
    if(val->isArray())
    {
        arraySet(true);
        ui->arrayRadio->setChecked(true);
        ui->noneRadio->setChecked(false);
        ui->multiplyRadio->setChecked(false);
        //ui->arrayCheckBox->setChecked(true);
        mode = ARRAY;
    }
    else if(val->isMultiplyMode())
    {
        arraySet(false);
        ui->noneRadio->setChecked(false);
        ui->arrayRadio->setChecked(false);
        ui->multiplyRadio->setChecked(true);
        mode = MULTIPLY;
        //ui->arrayCheckBox->setChecked(false);
    }
    else
    {
        arraySet(false);
        ui->noneRadio->setChecked(true);
        ui->arrayRadio->setChecked(false);
        ui->multiplyRadio->setChecked(false);
        mode = NONE;
    }

    //remove all old widgets
    removeAllStacked(false);

    //set combox box
    int id = ui->varTypesComboBox->findData(val->getUniformType(),Qt::UserRole+1);
    ui->varTypesComboBox->setCurrentIndex(id);

    //set name
    ui->varName->setText(val->getName());

    if(!val->isMultiplyMode())
    {
        //create tables
        int rowsCount = val->getRowsCount();
        int columnsCount = val->getColumnCount();
        int row = 0;
        int column = 0;
        QListIterator<QVariant> it(items);
        QTableView* view = new QTableView(ui->setVarStackWidget);
        QStandardItemModel* model = new StandardModelUnifValidity(val->getScalarType(), view);
        model->setRowCount(rowsCount);
        model->setColumnCount(columnsCount);
        view->setModel(model);
        tables.append(view);

        qDebug() << rowsCount << " " << columnsCount;

        while(it.hasNext())
        {
            if(row == rowsCount && column == columnsCount)
            {
                view = new QTableView(ui->setVarStackWidget);
                model = new QStandardItemModel(rowsCount,columnsCount,view);
                view->setModel(model);
                tables.append(view);
                row = 0;
                column = 0;
            }
            else if(column >= columnsCount)
                column = 0;

            //model->setItem(row,column,new QStandardItem(it.next()));
            model->setData(model->index(row,column),it.next());

            column++;

            if(column >= columnsCount)
                row++;
        }

        foreach(QTableView* view, tables)
        {
            ui->setVarStackWidget->addWidget(view);
        }

        //set page and label
        ui->pageNumLabel->setText(QString("%1/%2").arg(1).arg(ui->setVarStackWidget->count()+1));
        ui->setVarStackWidget->setCurrentIndex(0);
    }
    else // multiply mode
    {
        createMultiplyModel();
        MultiplyUnifModel* model = static_cast<MultiplyUnifModel*>(tables.value(0)->model());
        QList<QVariant> list = val->getValues();
        QStringList nameList;

        foreach(QVariant v, list)
        {
            QString name = v.toString();

            UniformVariable* var = info->getActiveProject()->getUniformVariable(name);

            if(var == NULL)
            {
                QMessageBox box;
                box.setText(tr("Uniform variable %1 do not exists!").arg(name));
                box.setIcon(QMessageBox::Warning);
                box.setStandardButtons(QMessageBox::Ok);
                box.exec();
                continue;
            }

            if(var->isMultiplyMode())
            {
                QMessageBox box;
                box.setText(tr("Uniform variable %1 is multiply matrix!").arg(name));
                box.setInformativeText(tr("You canno't use multiply matrix."));
                box.setIcon(QMessageBox::Warning);
                box.setStandardButtons(QMessageBox::Ok);
                box.exec();
                continue;
            }

            nameList.append(name);
        }

        model->setNameList(nameList);
    }
}

/**
 * @brief CreateUniformVariables::generateVariable Show generate variables selection menu.
 */
void CreateUniformVariables::generateVariable()
{
    SelectTypeDialog dialog;

    dialog.exec();

    if(dialog.result() == SelectTypeDialog::Accepted)
    {
        if(dialog.getType() == SelectTypeDialog::PROJECTION)
            addUniformVar("projection");
        else if(dialog.getType() == SelectTypeDialog::VIEW)
        {
            addUniformVar("rotX");
            addUniformVar("rotY");
            addUniformVar("zoomZ");
            addUniformVar("view");
        }
    }

}
