#ifndef GENERATEPROJECTIONMATRIX_H
#define GENERATEPROJECTIONMATRIX_H

#include <QDialog>
#include <QMessageBox>
#include "uniform/storage/uniformvariable.h"

namespace Ui {
class GenerateProjectionMatrix;
}

/**
 * @brief The GenerateProjectionMatrix class Dialog for generation projection matrix.
 */
class GenerateProjectionMatrix : public QDialog
{
    Q_OBJECT
    
public:
    explicit GenerateProjectionMatrix(QWidget *parent = 0);
    ~GenerateProjectionMatrix();

public slots:
    void accept();

private slots:
    void radioOrthoClicked();
    void radioPerspClicked();
    
private:
    Ui::GenerateProjectionMatrix *ui;
    bool isOrtho;
    QString name;
    QMessageBox emptyMessage;
};

#endif // GENERATEPROJECTIONMATRIX_H
