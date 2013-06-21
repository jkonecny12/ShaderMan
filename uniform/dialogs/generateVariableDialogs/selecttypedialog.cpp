#include "selecttypedialog.h"
#include "ui_selecttypedialog.h"
#include "uniform/dialogs/generateVariableDialogs/generateprojectionmatrix.h"
#include "matrixgenerator.h"
#include "infomanager.h"
#include "setnamewithbuttonsdialog.h"

/**
 * @brief SelectTypeDialog::SelectTypeDialog Create dialog for selecting type of generated uniform variables.
 * @param parent Parent of this dialog.
 */
SelectTypeDialog::SelectTypeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectTypeDialog),
    generateProjection("Projection Matrix 4x4"),
    generateView("View Matrix 4x4"),
    generateRotXMatrix("Rotation Matrix 4x4 - X axis with buttons"),
    generateRotYMatrix("Rotation Matrix 4x4 - Y axis with buttons"),
    generateRotZMatrix("Rotation Matrix 4x4 - Z axis with buttons"),
    projName("projection"),
    viewName("view"),
    rotXName("rotX"),
    rotYName("rotY"),
    zoomZName("zoomZ"),
    type(NONE)
{
    ui->setupUi(this);
    QStringList list;

    list << generateProjection << generateView; // <<
            //generateRotXMatrix << generateRotYMatrix << generateRotZMatrix;

    ui->comboBox->addItems(list);
    ui->comboBox->setCurrentIndex(0);
}

/**
 * @brief SelectTypeDialog::~SelectTypeDialog Destroy this dialog and free resources.
 */
SelectTypeDialog::~SelectTypeDialog()
{
    delete ui;
}

/**
 * @brief SelectTypeDialog::accept If user click Ok button, then test do what user want.
 * This method can create new dialog window, or create needed variable, if no information is needed.
 */
void SelectTypeDialog::accept()
{
    QString selectedType = ui->comboBox->currentText();

    if(selectedType == generateProjection)
    {
        GenerateProjectionMatrix dialog;
        dialog.exec();

        if(dialog.result() == GenerateProjectionMatrix::Accepted)
        {
            setResult(true);
            type = PROJECTION;
            setVisible(false);
        }
        else
        {
            setResult(false);
            setVisible(false);
        }
    }
    else if(selectedType == generateView)
    {
        QMessageBox box;
        box.setText(tr("Do you want to create view matrix?"));
        box.setInformativeText(tr("Uniform variables '%1','%2','%3','%4' will override older variables with the same name.")
                               .arg(viewName, rotXName, rotYName, zoomZName));
        box.setIcon(QMessageBox::Question);
        box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

        int ret = box.exec();

        if(ret == QMessageBox::Ok)
        {
            type = VIEW;
            createViewMatrix();
            setResult(true);
            setVisible(false);
        }
        else
        {
            setResult(false);
            setVisible(false);
        }
    }
    else if(selectedType.startsWith("Rotation") && selectedType.endsWith("buttons"))
    {
        SetNameWithButtonsDialog dialog;

        dialog.exec();

        if(dialog.result() == SetNameWithButtonsDialog::Accepted)
        {
            //QString name = dialog.getVariableName();
            //int buttonMinus = dialog.getButtonMinus();
            //int buttonPlus = dialog.getButtonPlus();

            //TODO complete this
        }
    }
}

/**
 * @brief SelectTypeDialog::getType Get type of generated variable.
 * @return Type of generated variable.
 */
SelectTypeDialog::ResultType SelectTypeDialog::getType()
{
    return type;
}

/**
 * @brief SelectTypeDialog::createViewMatrix Create new view matrix.
 * First create rotX matrix and rotY matrix for ratation, after that create translation matrix zoomZ.
 * Second create view matrix with multiplication mode and set these matrices for multiply.
 */
void SelectTypeDialog::createViewMatrix()
{
    MetaProject* proj = InfoManager::getInstance()->getActiveProject();

    // create rotation matrix in axis X
    QStringList listX = MatrixGenerator::getRotXWithButtons(MatrixGenerator::Y);
    QList<QVariant> varListX = toVariants(listX);

    // create rotation matrix in axis X
    UniformVariable* u = new UniformVariable(rotXName,varListX,UniformTypes::FLOAT,
                                             UniformTypes::MAT4,UniformTypes::MAT4);

    forceAddUniform(u, proj);

    // create rotation matrix in axis Y
    QStringList listY = MatrixGenerator::getRotYWithButtons(MatrixGenerator::X);
    QList<QVariant> varListY = toVariants(listY);

    u = new UniformVariable(rotYName,varListY,UniformTypes::FLOAT,
                            UniformTypes::MAT4,UniformTypes::MAT4);

    forceAddUniform(u, proj);

    // create translation matrix in axis Z
    QStringList listZ = MatrixGenerator::getTranslation(QVector3D(0.0,0.0,-4.0),false,false,true);
    QList<QVariant> varListZ = toVariants(listZ);

    u = new UniformVariable(zoomZName,varListZ,UniformTypes::FLOAT,
                            UniformTypes::MAT4, UniformTypes::MAT4);

    forceAddUniform(u, proj);

    // create view matrix
    QList<QVariant> listView;
    listView << QVariant(zoomZName) << QVariant(rotXName) << QVariant(rotYName);

    u = new UniformVariable(viewName,listView,UniformTypes::FLOAT,UniformTypes::MAT4,UniformTypes::MAT4,true);

    forceAddUniform(u, proj);
}

/**
 * @brief SelectTypeDialog::toVariants Get StringList of variables and return QList<QVariant> list for another use.
 * @param list List of some strings.
 * @return Variants for another use.
 */
QList<QVariant> SelectTypeDialog::toVariants(QStringList list)
{
    QList<QVariant> ret;
    foreach(QString s, list)
    {
        ret.append(QVariant(s));
    }

    return ret;
}
