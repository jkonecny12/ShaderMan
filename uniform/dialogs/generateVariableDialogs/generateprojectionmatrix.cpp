#include "generateprojectionmatrix.h"
#include "ui_generateprojectionmatrix.h"
#include <QDoubleValidator>
#include <QIntValidator>
#include <QMessageBox>
#include "infomanager.h"

/**
 * @brief GenerateProjectionMatrix::GenerateProjectionMatrix Create dialog for generation projection matrix.
 * @param parent Parent of this dialog.
 */
GenerateProjectionMatrix::GenerateProjectionMatrix(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateProjectionMatrix),
    name("projection")
{
    ui->setupUi(this);
    isOrtho = false;

    ui->orthographicWidget->setVisible(false);
    ui->perspectiveRadio->setChecked(true);

    emptyMessage.setText(tr("You must fill in all fields."));
    emptyMessage.setIcon(QMessageBox::Warning);

    QDoubleValidator* planeValidator = new QDoubleValidator(-1000,1000,0,this);
    QDoubleValidator* angleValidator = new QDoubleValidator(0,360,0,this);
    QIntValidator* aspectValid = new QIntValidator(0,1000,this);

    ui->perspVecrticalAngleEdit->setValidator(angleValidator);
    ui->perspAspectWidthEdit->setValidator(aspectValid);
    ui->perspAspectHeightEdit->setValidator(aspectValid);
    ui->perspFarPlaneEdit->setValidator(planeValidator);
    ui->perspNearPlaneEdit->setValidator(planeValidator);

    connect(ui->orthographicRadio,SIGNAL(clicked()),this,SLOT(radioOrthoClicked()));
    connect(ui->perspectiveRadio,SIGNAL(clicked()),this,SLOT(radioPerspClicked()));
}

/**
 * @brief GenerateProjectionMatrix::~GenerateProjectionMatrix Destroy this dialog and free resources.
 */
GenerateProjectionMatrix::~GenerateProjectionMatrix()
{
    delete ui;
}

/**
 * @brief GenerateProjectionMatrix::accept Test setted parameters and create new projection matrix.
 */
void GenerateProjectionMatrix::accept()
{
    MetaProject* proj = InfoManager::getInstance()->getActiveProject();
    QList<QVariant> list;

    if(isOrtho) // orthographic matrix
    {
        QString left = ui->orthoLeftEdit->text();
        QString right = ui->orthoRightEdit->text();
        QString bottom = ui->orthoBottomEdit->text();
        QString top = ui->orthoTopEdit->text();
        QString near = ui->orthoNearEdit->text();
        QString far = ui->orthoFarEdit->text();

        if(left.isEmpty() || right.isEmpty() ||
                bottom.isEmpty() || top.isEmpty() ||
                near.isEmpty() || far.isEmpty())
        {
            emptyMessage.exec();
            return;
        }

        list << // first row
                QVariant(QString("2 / ((%1) - (%2))").arg(right, left)) <<
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("-(((%1) + (%2)) / ((%1) - (%2)))").arg(right, left)) <<
                // second row
                QVariant(QString("0")) <<
                QVariant(QString("2 / ((%1) - (%2))").arg(top, bottom)) <<
                QVariant(QString("0")) <<
                QVariant(QString("-(((%1) + (%2)) / ((%1) - (%2)))").arg(top, bottom)) <<
                // third row
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("-2 / ((%1) - (%2))").arg(far, near)) <<
                QVariant(QString("-(((%1) + (%2)) / ((%1) - (%2)))").arg(far, near)) <<
                // fourth row
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("1"));
    }
    else // perspective matrix
    {
        QString vertAngle = ui->perspVecrticalAngleEdit->text();
        QString aspectWidth = ui->perspAspectWidthEdit->text();
        QString aspectHeight = ui->perspAspectHeightEdit->text();
        QString farPlane = ui->perspFarPlaneEdit->text();
        QString nearPlane = ui->perspNearPlaneEdit->text();

        if(vertAngle.isEmpty() ||
                aspectWidth.isEmpty() || aspectHeight.isEmpty() ||
                farPlane.isEmpty() || nearPlane.isEmpty())
        {
            emptyMessage.exec();
            return;
        }

        float aspect = static_cast<float>(aspectWidth.toInt()) / static_cast<float>(aspectHeight.toInt());

        list << // first row
                QVariant(QString("(1.0 / tan(%1 * 0.0087266)) / %2").arg(vertAngle, QString("%1").arg(aspect))) <<
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                // second row
                QVariant(QString("0")) <<
                QVariant(QString("1.0 / tan(%1 * 0.0087266)").arg(vertAngle)) <<
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                // third row
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("(%1 + %2) / (%2 - %1)").arg(farPlane, nearPlane)) <<
                QVariant(QString("(2 * %1 * %2) / (%2 - %1)").arg(farPlane, nearPlane)) <<
                // fourth row
                QVariant(QString("0")) <<
                QVariant(QString("0")) <<
                QVariant(QString("-1")) <<
                QVariant(QString("0"));
    }

    UniformVariable *variable = new UniformVariable(name, list,
                                                    UniformTypes::FLOAT,
                                                    UniformTypes::MAT4,
                                                    UniformTypes::MAT4);

    if(!proj->addUniformVariable(variable))
    {
        proj->removeUniformVariable(name);
        proj->addUniformVariable(variable);
    }

    setResult(QDialog::Accepted);
    setVisible(false);
}

/**
 * @brief GenerateProjectionMatrix::radioOrthoClicked If orthographics radio button is clicked disable others radio buttons.
 */
void GenerateProjectionMatrix::radioOrthoClicked()
{
    ui->perspectiveWidget->setVisible(false);
    ui->orthographicWidget->setVisible(true);
    ui->perspectiveRadio->setChecked(false);

    isOrtho = true;
}

/**
 * @brief GenerateProjectionMatrix::radioPerspClicked If perspective radio button is clicked disable others radio buttons.
 */
void GenerateProjectionMatrix::radioPerspClicked()
{
    ui->orthographicWidget->setVisible(false);
    ui->perspectiveWidget->setVisible(true);
    ui->orthographicRadio->setChecked(false);

    isOrtho = false;
}
