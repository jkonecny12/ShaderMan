#ifndef OPENGLSETTINGSDIALOG_H
#define OPENGLSETTINGSDIALOG_H

#include <QDialog>
#include "project_settings/settingsstorage.h"
#include "infomanager.h"

namespace Ui {
class OpenglSettingsDialog;
}

/**
 * @brief The OpenglSettingsDialog class Dialog for set settings to OpenGL window. This setting is saved to project.
 */
class OpenglSettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenglSettingsDialog(QWidget *parent = 0);
    ~OpenglSettingsDialog();

    void accept();

private:
    void initClearSpinBoxes();
    void initComboBoxes();
    void setDefaultValues();

private slots:
    void blending(bool checked);
    
private:
    Ui::OpenglSettingsDialog *ui;
    InfoManager* info;
    SettingsStorage* settings;
};

#endif // OPENGLSETTINGSDIALOG_H
