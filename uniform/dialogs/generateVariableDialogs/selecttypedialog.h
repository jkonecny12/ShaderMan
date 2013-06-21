#ifndef SELECTTYPEDIALOG_H
#define SELECTTYPEDIALOG_H

#include <QDialog>
#include <QList>
#include <QVariant>
#include <QStringList>

#include "uniform/storage/uniformvariable.h"
#include "meta_data/metaproject.h"

namespace Ui {
class SelectTypeDialog;
}

/**
 * @brief The SelectTypeDialog class Dialog for selecting type of generated uniform variable.
 */
class SelectTypeDialog : public QDialog
{
    Q_OBJECT
    
public:
    enum ResultType {PROJECTION, VIEW, NONE};

    explicit SelectTypeDialog(QWidget *parent = 0);
    ~SelectTypeDialog();

    virtual void accept();

    ResultType getType();

private:
    void createViewMatrix();
    QList<QVariant> toVariants(QStringList list);

    /**
     * @brief forceAddUniform Add uniform variable to project.
     * If this variable already exists, then remove it and set new.
     * @param u Uniform variable what we want to save.
     * @param proj Where we saving our variable.
     */
    inline void forceAddUniform(UniformVariable* u, MetaProject* proj)
    {
        if(!proj->addUniformVariable(u))
        {
            proj->removeUniformVariable(u->getName());
            proj->addUniformVariable(u);
        }
    }
    
private:
    Ui::SelectTypeDialog *ui;
    const QString generateProjection;
    const QString generateView;
    const QString generateRotXMatrix;
    const QString generateRotYMatrix;
    const QString generateRotZMatrix;
    const QString projName;
    const QString viewName;
    const QString rotXName;
    const QString rotYName;
    const QString zoomZName;
    ResultType type;
};

#endif // SELECTTYPEDIALOG_H
