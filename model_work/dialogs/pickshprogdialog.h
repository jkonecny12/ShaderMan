#ifndef ATTACHSHPROGDRIALOG_H
#define ATTACHSHPROGDRIALOG_H

#include <QDialog>

namespace Ui {
class PickShProgDialog;
}

class PickShProgDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PickShProgDialog(QWidget *parent = 0);
    ~PickShProgDialog();

    const QString getShProgSet();

private:
    void setUpComboBox();
    
private:
    Ui::PickShProgDialog *ui;
};

#endif // ATTACHSHPROGDRIALOG_H
