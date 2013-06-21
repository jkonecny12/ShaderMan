#ifndef SETPROGNAMEDIALOG_H
#define SETPROGNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class SetProgNameDialog;
}

class SetProgNameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetProgNameDialog(QWidget *parent = 0);
    ~SetProgNameDialog();

    QString getValue();

public slots:
    virtual void accept();
    
private:
    Ui::SetProgNameDialog *ui;
};

#endif // SETPROGNAMEDIALOG_H
