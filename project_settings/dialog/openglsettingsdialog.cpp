#include "openglsettingsdialog.h"
#include "ui_openglsettingsdialog.h"

/**
 * @brief OpenglSettingsDialog::OpenglSettingsDialog Create dialog with OpenGL settings which will be saved to project.
 * @param parent Parent of this dialog.
 */
OpenglSettingsDialog::OpenglSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenglSettingsDialog)
{
    ui->setupUi(this);

    info = InfoManager::getInstance();
    settings = info->getActiveProject()->getSettings();

    ui->blendingCheck->setChecked(false);
    blending(false);

    initClearSpinBoxes();
    initComboBoxes();
    setDefaultValues();

    ui->blendWidget->setVisible(false);

    connect(ui->blendingCheck,SIGNAL(toggled(bool)),this,SLOT(blending(bool)));

}

/**
 * @brief OpenglSettingsDialog::~OpenglSettingsDialog Destroy allocated resources.
 */
OpenglSettingsDialog::~OpenglSettingsDialog()
{
    delete ui;
}

/**
 * @brief OpenglSettingsDialog::accept Save all settings and accept this dialog window.
 */
void OpenglSettingsDialog::accept()
{
    // clear color
    float r = ui->clearRedSpin->value();
    float g = ui->clearGreenSpin->value();
    float b = ui->clearBlueSpin->value();
    float a = ui->clearAlphaSpin->value();

    settings->setBackgroundColor(r, g, b, a);

    settings->setDepthTest(ui->depthTestBox->isChecked());

    // blending
    bool isBlend = ui->blendingCheck->isChecked();

    settings->setBlending(isBlend);

    if(isBlend)
    {
        SettingsStorage::BLEND_FUNCTION sFactorFunc, dFactorFunc;

        int sId = ui->blendSFactorBox->currentIndex();
        int sFactor = ui->blendSFactorBox->itemData(sId).toInt();
        sFactorFunc = static_cast<SettingsStorage::BLEND_FUNCTION>(sFactor);

        int dId = ui->blendDFactorBox->currentIndex();
        int dFactor = ui->blendDFactorBox->itemData(dId).toInt();
        dFactorFunc = static_cast<SettingsStorage::BLEND_FUNCTION>(dFactor);

        settings->setBlendFunc(sFactorFunc, dFactorFunc);
    }

    QDialog::accept();

}

/**
 * @brief OpenglSettingsDialog::initClearSpinBoxes Initialize spin boxes for OpenGL clear color.
 */
void OpenglSettingsDialog::initClearSpinBoxes()
{
    ui->clearRedSpin->setMaximum(1.0);
    ui->clearRedSpin->setMinimum(0.0);
    ui->clearRedSpin->setSingleStep(0.1);

    ui->clearGreenSpin->setMaximum(1.0);
    ui->clearGreenSpin->setMinimum(0.0);
    ui->clearGreenSpin->setSingleStep(0.1);

    ui->clearBlueSpin->setMaximum(1.0);
    ui->clearBlueSpin->setMinimum(0.0);
    ui->clearBlueSpin->setSingleStep(0.1);

    ui->clearAlphaSpin->setMaximum(1.0);
    ui->clearAlphaSpin->setMinimum(0.0);
    ui->clearAlphaSpin->setSingleStep(0.1);
}

/**
 * @brief OpenglSettingsDialog::initComboBoxes Initialize combo boxes with blending functions.
 */
void OpenglSettingsDialog::initComboBoxes()
{
    QStringList list = settings->getBlendFuncStrings();

    // s factor
    for(int i = 0; i < list.size(); ++i)
    {
        QString itemStr = list.value(i);

        if(itemStr == "SRC_COLOR" || itemStr == "ONE_MINUS_SRC_COLOR")
            continue;

        ui->blendSFactorBox->addItem(itemStr, QVariant(i));
    }

    // d factor
    for(int i = 0; i < list.size(); ++i)
    {
        QString itemStr = list.value(i);

        if(itemStr == "DST_COLOR" || itemStr == "ONE_MINUS_DST_COLOR" ||
                itemStr == "SRC_ALPHA_SATURATE")
            continue;

        ui->blendDFactorBox->addItem(itemStr, QVariant(i));
    }

    ui->blendDFactorBox->setCurrentIndex(0);
    ui->blendSFactorBox->setCurrentIndex(0);
}

/**
 * @brief OpenglSettingsDialog::setDefaultValues Set default values from active project.
 */
void OpenglSettingsDialog::setDefaultValues()
{
    // set clear color
    const float *clearColors = settings->getBackgroundColors();

    ui->clearRedSpin->setValue(clearColors[0]);
    ui->clearGreenSpin->setValue(clearColors[1]);
    ui->clearBlueSpin->setValue(clearColors[2]);
    ui->clearAlphaSpin->setValue(clearColors[3]);

    // set blending
    bool isBlending = settings->isBlending();

    ui->blendingCheck->setChecked(isBlending);
    blending(isBlending);

    if(isBlending) // set blending values to combo boxes
    {
        int sFactor = settings->getBlendSFactor();
        int dFactor = settings->getBlendDFactor();

        int sId = ui->blendSFactorBox->findData(QVariant(sFactor));
        int dId = ui->blendDFactorBox->findData(QVariant(dFactor));

        ui->blendSFactorBox->setCurrentIndex(sId);
        ui->blendDFactorBox->setCurrentIndex(dId);
    }

    // set depth test
    ui->depthTestBox->setChecked(settings->isDepthTest());
}

/**
 * @brief OpenglSettingsDialog::blending Hide/show additional settings for blending.
 * @param checked If blending is checked show settings, otherwise hide settings.
 */
void OpenglSettingsDialog::blending(bool checked)
{
    if(checked)
    {
        ui->blendSFactorBox->setVisible(true);
        ui->blendDFactorBox->setVisible(true);
        ui->blendSFactorLabel->setVisible(true);
        ui->blendDFractorLabel->setVisible(true);
    }
    else
    {
        ui->blendSFactorBox->setVisible(false);
        ui->blendDFactorBox->setVisible(false);
        ui->blendSFactorLabel->setVisible(false);
        ui->blendDFractorLabel->setVisible(false);
    }
}
