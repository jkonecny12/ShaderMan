#ifndef SETNAMEWITHBUTTONSDIALOG_H
#define SETNAMEWITHBUTTONSDIALOG_H

#include <QDialog>

namespace Ui {
class SetNameWithButtonsDialog;
}

/**
 * @brief The SetNameWithButtonsDialog class Create dialog for getting needed information from user
 * to create rotation matrices with buttons control.
 */
class SetNameWithButtonsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetNameWithButtonsDialog(QWidget *parent = 0);
    ~SetNameWithButtonsDialog();

    QString getVariableName();
    int getButtonMinus();
    int getButtonPlus();

    void accept();
    
private:
    Ui::SetNameWithButtonsDialog *ui;
    QString name;
    int buttonMinus;
    int buttonPlus;
};

#endif // SETNAMEWITHBUTTONSDIALOG_H
