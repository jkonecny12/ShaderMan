#include "programmanagerdialog.h"
#include "ui_programmanagerdialog.h"
#include "infomanager.h"
#include "tools/shaderprogrammodel.h"
#include "setprognamedialog.h"
#include <QMessageBox>

#define COLOR_WIDGET "Colors"
#define TEXCOORD_WIDGET "TexCoords"
#define TEXTURE_WIDGET "Textures"

/**
 * @brief ProgramManagerDialog::ProgramManagerDialog Create dialog for managing shader programs
 * @param parent Parent of this widget
 */
ProgramManagerDialog::ProgramManagerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgramManagerDialog)
{
    info = InfoManager::getInstance();

    QIcon icon(":/button_plus.png");
    QSize size(25,25);

    // set icons and remove text from buttons
    ui->setupUi(this);
    ui->addColorsBtn->setIcon(icon);
    ui->addColorsBtn->setText("");
    ui->addColorsBtn->setIconSize(icon.actualSize(size));
    ui->addTexCoordsBtn->setIcon(icon);
    ui->addTexCoordsBtn->setText("");
    ui->addTexCoordsBtn->setIconSize(icon.actualSize(size));
    ui->addTexturesBtn->setIcon(icon);
    ui->addTexturesBtn->setText("");
    ui->addTexturesBtn->setIconSize(icon.actualSize(size));

    // set layouts
    colorsLayout = new QVBoxLayout(ui->colorsWidget);
    texCoordsLayout = new QVBoxLayout(ui->texCoordsWidget);
    textureLayout = new QVBoxLayout(ui->textureWidget);

    // list model
    ShaderProgramModel* model = new ShaderProgramModel();
    ui->listView->setModel(model);

    // fill ComboBoxes
    QList<MetaShader*> vertexs = info->getActiveProject()->getVertexShaders();

    foreach(MetaShader* sh, vertexs)
    {
        ui->vertexShBox->addItem(sh->getShaderName(),QVariant(sh->getShader()));
    }
    ui->vertexShBox->setCurrentIndex(-1);

    QList<MetaShader*> fragments = info->getActiveProject()->getFragmentShaders();

    foreach(MetaShader* sh, fragments)
    {
        ui->fragmentShBox->addItem(sh->getShaderName(),QVariant(sh->getShader()));
    }
    ui->fragmentShBox->setCurrentIndex(-1);

    // connect add button
    connect(ui->addColorsBtn,SIGNAL(clicked()),this,SLOT(addColorsWidget()));
    connect(ui->addTexCoordsBtn,SIGNAL(clicked()),this,SLOT(addTexCoordsWidget()));
    connect(ui->addTexturesBtn,SIGNAL(clicked()),this,SLOT(addTextureWidget()));

    // connect shader program buttons
    connect(ui->addShProgBtn,SIGNAL(clicked()),this,SLOT(addShaderProgram()));
    connect(ui->rmShProgBtn,SIGNAL(clicked()),this,SLOT(removeShaderProgram()));

    connect(ui->listView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(setDefaultValues(QModelIndex)));
    connect(ui->progNameEdit,SIGNAL(editingFinished()),this,SLOT(testProgramName()));

    // set shaders to shader program
    connect(ui->vertexShBox,SIGNAL(currentIndexChanged(int)),this,SLOT(vertexShaderSet(int)));
    connect(ui->fragmentShBox,SIGNAL(currentIndexChanged(int)),this,SLOT(fragmentShaderSet(int)));

    // set attachment points
    connect(ui->verticesEdit,SIGNAL(editingFinished()),this,SLOT(verticesVariableSet()));
    connect(ui->normalsEdit,SIGNAL(editingFinished()),this,SLOT(normalsVariableSet()));
}

/**
 * @brief ProgramManagerDialog::~ProgramManagerDialog Destroy this object
 */
ProgramManagerDialog::~ProgramManagerDialog()
{
    delete ui;
}

/**
 * @brief ProgramManagerDialog::setOldProgName Set old name value. It is used when user set bad name.
 */
void ProgramManagerDialog::setOldProgName()
{
    QString name = ui->listView->currentIndex().data().toString();

    ui->progNameEdit->setText(name);
}

/**
 * @brief ProgramManagerDialog::getActiveShProgram Get selected shader program as object.
 * @return Return slected shader program.
 */
MetaShaderProg *ProgramManagerDialog::getActiveShProgram()
{
    QString progName = ui->listView->selectionModel()->currentIndex().data().toString();
    MetaShaderProg* prog = info->getProgram(progName);

    return prog;
}

/**
 * @brief ProgramManagerDialog::testWidgets Test all widgets from widgets set for validity.
 *Valid widget is the one which have all variables setted.
 * @param widgets Set of row widgets for testing.
 * @return True if all widgets are valid, false otherwise.
 */
bool ProgramManagerDialog::testWidgets(QSet<ShProgWidgetAbstract *> widgets)
{
    foreach(ShProgWidgetAbstract* widget, widgets)
    {
        if(!widget->isValid())
            return false;
    }

    return true;
}

/**
 * @brief ProgramManagerDialog::getPositions Get all position values in variable widgets.
 * @param widgets Variable widgets used in colors and texture coords as rows.
 * @return Return list of used positions.
 */
QList<uint> ProgramManagerDialog::getPositions(QSet<ShProgVariableWidget *> widgets)
{
    QList<uint> ret;

    foreach(ShProgVariableWidget* widget, widgets)
    {
        ret.append(widget->getPosition());
    }

    return ret;
}

/**
 * @brief ProgramManagerDialog::firstFreePosition Find first free position in positions list.
 * @param positions List of uints where we looking for free position.
 * @return Return first free position in given list.
 */
uint ProgramManagerDialog::firstFreePosition(QList<uint> positions)
{
    uint pos = 0;

    while(pos != INT_MAX)
    {
        if(!positions.contains(pos))
        {
            return pos;
        }

        ++pos;
    }

    return 0;
}

/**
 * @brief ProgramManagerDialog::addColorsWidget Add new color row widget.
 */
void ProgramManagerDialog::addColorsWidget()
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        addColorsWidget(firstFreePosition(getPositions(colorsWidgets)));
    else
        addColorsWidget(firstFreePosition(prog->getColorPositions()));
}

/**
 * @brief ProgramManagerDialog::addColorsWidget Add new row of color variable widget.
 * @param position Position in color buffer list of this attachment.
 * @param variable Text what to set when widget is created.
 */
void ProgramManagerDialog::addColorsWidget(const uint position, const QString variable)
{
    QSet<ShProgWidgetAbstract*> absList;
    foreach(ShProgVariableWidget* w, colorsWidgets)
    {
        absList.insert(static_cast<ShProgWidgetAbstract*>(w));
    }

    if(!testWidgets(absList))
        return;

    ShProgVariableWidget* widget = new ShProgVariableWidget(COLOR_WIDGET,this);
    colorsLayout->addWidget(widget);
    widget->show();

    widget->setVariable(variable);
    widget->setPosition(position);

    colorsWidgets.insert(widget);

    connect(widget,SIGNAL(removeBtn(QString)),this,SLOT(removeWidget(QString)));
    connect(widget,SIGNAL(editingFinished(QString,QString,uint)),this,SLOT(colorsVariableSet(QString,QString,uint)));
    connect(widget,SIGNAL(positionChanged(QString,uint)),this,SLOT(testColorPosition(QString,uint)));
}

/**
 * @brief ProgramManagerDialog::addTexCoordsWidget Add new texture coordinate row widget.
 */
void ProgramManagerDialog::addTexCoordsWidget()
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        addTexCoordsWidget(firstFreePosition(getPositions(texCoordsWidgets)));
    else
        addTexCoordsWidget(firstFreePosition(prog->getTexCoordPositions()));
}

/**
 * @brief ProgramManagerDialog::addTexCoordsWidget Add new row of texture coordinate variable widget.
 * @param position Position in texture coordinate buffer list of this attachment.
 * @param variable Attachment point name for new texture coordinate row.
 */
void ProgramManagerDialog::addTexCoordsWidget(const uint position, const QString variable)
{
    QSet<ShProgWidgetAbstract*> absList;
    foreach(ShProgVariableWidget* tex, texCoordsWidgets)
    {
        absList.insert(static_cast<ShProgWidgetAbstract*>(tex));
    }

    if(!testWidgets(absList))
        return;

    ShProgVariableWidget* widget = new ShProgVariableWidget(TEXCOORD_WIDGET,this);
    texCoordsLayout->addWidget(widget);
    widget->show();

    widget->setVariable(variable);
    widget->setPosition(position);

    texCoordsWidgets.insert(widget);

    connect(widget,SIGNAL(removeBtn(QString)),this,SLOT(removeWidget(QString)));
    connect(widget,SIGNAL(editingFinished(QString,QString,uint)),this,SLOT(texCoordsVariableSet(QString,QString,uint)));
    connect(widget,SIGNAL(positionChanged(QString,uint)),this,SLOT(testTexCoordPosition(QString,uint)));
}

/**
 * @brief ProgramManagerDialog::addTextureWidget Add texture widget when user click on + button.
 */
void ProgramManagerDialog::addTextureWidget()
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    QStringList texNames = info->getActiveProject()->getTextureNameList();

    if(texNames.isEmpty())
    {
        QMessageBox box;
        box.setText(tr("You do not have texture to use!"));
        box.setInformativeText(tr("You must load some texture first."));
        box.setIcon(QMessageBox::Warning);
        box.exec();

        return;
    }

    addTextureWidget(texNames.first());

    /*
    QStringList widgetName;
    foreach(ShProgTextureWidget* tex, textureWidgets)
    {
        widgetName.append(tex->getTexture());
    }

    QStringList texNames = info->getActiveProject()->getTextureNameList();
    foreach(QString name, texNames)
    {
        if(!widgetName.contains(name))
            addTextureWidget(name);
    }
    */
}

/**
 * @brief ProgramManagerDialog::addTextureWidget Add new row of texture combo box widget and with attachment point line edit widget.
 * @param textureVariable Name of the texture for new texture row. If empty first combo box is set.
 * @param pointVariable Attachment point name for new texture row.
 */
void ProgramManagerDialog::addTextureWidget(const QString textureVariable, const QString pointVariable)
{
    QSet<ShProgWidgetAbstract*> absList;

    foreach(ShProgTextureWidget* w, textureWidgets)
    {
        absList.insert(static_cast<ShProgWidgetAbstract*>(w));
    }

    if(!testWidgets(absList))
        return;

    QStringList names = info->getActiveProject()->getTextureNameList();

    if(!names.contains(textureVariable))
        return;

    ShProgTextureWidget* widget = new ShProgTextureWidget(TEXTURE_WIDGET,this);
    textureLayout->addWidget(widget);
    widget->show();

    //QStringList names = info->getActiveProject()->getTextureNameList();
    widget->setTextureNames(names);

    widget->setPointName(pointVariable);

    if(!textureVariable.isEmpty())
        widget->setTextureName(textureVariable);

    textureWidgets.insert(widget);

    connect(widget,SIGNAL(removeBtn(QString)),this,SLOT(removeWidget(QString)));
    //connect(widget,SIGNAL(textureChanged(QString,QString)),this,SLOT(textureVariableRemoved(QString,QString)));
    //connect(widget,SIGNAL(editingFinished(QString,QString,QString)),this,SLOT(textureVariableSet(QString,QString,QString)));
    connect(widget,SIGNAL(textureChanged(QString,QString)),this,SLOT(textureVariableSet(QString,QString)));
    connect(widget,SIGNAL(editingFinished(QString,QString)),this,SLOT(texturePointVariableSet(QString,QString)));
}

/**
 * @brief ProgramManagerDialog::removeWidget Remove widget row and remove data from shader program.
 * @param type Type of this widget. Colors for colors widget, TexCoords for texture coordinates widget, Textures for textures widget.
 */
void ProgramManagerDialog::removeWidget(QString type)
{
    //QWidget* widget = static_cast<QWidget*>(sender());

    /*
    if(widget == NULL)
        return;
    */


    //ShProgVariableWidget* colorsWidget = qobject_cast<ShProgVariableWidget*>(sender());

    if(type == COLOR_WIDGET)
    {
        ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());
        QString name = widget->getVariableValue();
        colorsVariableRemoved(name);
        colorsWidgets.remove(widget);
        delete widget;
    }

    //ShProgVariableWidget* texCoordsWidget = qobject_cast<ShProgVariableWidget*>(sender());

    if(type == TEXCOORD_WIDGET)
    {
        ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());
        QString name = widget->getVariableValue();
        texCoordsVariableRemoved(name);
        texCoordsWidgets.remove(widget);
        delete widget;
    }

    //ShProgTextureWidget* texturesWidget = qobject_cast<ShProgTextureWidget*>(sender());

    if(type == TEXTURE_WIDGET)
    {
        ShProgTextureWidget* widget = static_cast<ShProgTextureWidget*>(sender());
        QString name = widget->getVariableValue();
        textureVariableRemoved(name);
        textureWidgets.remove(widget);
        delete widget;
    }

    //delete widget;
}

/**
 * @brief ProgramManagerDialog::removeAllWidgets Remove all row widgets from this dialog.
 */
void ProgramManagerDialog::removeAllWidgets()
{
    /*
    foreach(ShProgVariableWidget* widget, colorsWidgets)
    {
        if(widget != NULL)
            delete widget;
    }

    colorsWidgets.clear();
    */
    qDeleteAll(colorsWidgets);
    colorsWidgets.clear();
/*
    foreach(ShProgVariableWidget* widget, texCoordsWidgets)
    {
        if(widget != NULL)
            delete widget;
    }

    texCoordsWidgets.clear();
*/
    qDeleteAll(texCoordsWidgets);
    texCoordsWidgets.clear();
/*
    foreach(ShProgTextureWidget* widget, textureWidgets)
    {
        if(widget != NULL)
            delete widget;
    }

    textureWidgets.clear();
*/
    qDeleteAll(textureWidgets);
    textureWidgets.clear();
}

/**
 * @brief ProgramManagerDialog::addShaderProgram Add shader program to manager.
 */
void ProgramManagerDialog::addShaderProgram()
{
    SetProgNameDialog nameDialog(this);

    nameDialog.exec();

    if(nameDialog.result() == SetProgNameDialog::Accepted)
    {
        QString name = nameDialog.getValue();

        info->getActiveProject()->addProgram(name);
        MetaShaderProg* prog = info->getActiveProject()->getProgram(name);
        ShaderProgramModel* model = static_cast<ShaderProgramModel*>(ui->listView->model());

        connect(prog,SIGNAL(nameChanged(QString,QString)),model,SLOT(programNameChanged(QString,QString)));

        if(!info->getActiveProject()->getVertexShaders().isEmpty())
        {
            prog->setVertexShader(info->getActiveProject()->getVertexShaders().value(0)->getShader());
        }

        if(!info->getActiveProject()->getFragmentShaders().isEmpty())
        {
            prog->setFragmentShader(info->getActiveProject()->getFragmentShaders().value(0)->getShader());
        }

        prog->setVerticesAttach("vertex");

        ui->listView->setCurrentIndex(model->findIndex(prog));
    }
}

/**
 * @brief ProgramManagerDialog::removeShaderProgram Remove shader program from manager.
 */
void ProgramManagerDialog::removeShaderProgram()
{
    QModelIndex index = ui->listView->selectionModel()->selectedRows().at(0);
    QString name = index.data().toString();

    QMessageBox box;
    box.setText(tr("Do you really want to remove %1 shader program?").arg(name));
    box.setIcon(QMessageBox::Question);
    box.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
    int ret = box.exec();

    if(ret == QMessageBox::Ok)
        info->getActiveProject()->removeProgram(name);
}

/**
 * @brief ProgramManagerDialog::setDefaultValues Set values to dialog, when shader program is activated.
 * @param current Index to current shader program in list.
 */
void ProgramManagerDialog::setDefaultValues(QModelIndex current)
{
    MetaProject* actProj = info->getActiveProject();
    // set the program name
    QString progName = current.data().toString();
    ui->progNameEdit->setText(progName);

    // set shaders
    MetaShaderProg* prog = actProj->getProgram(progName);

    if(prog == NULL)
        return;

    QString fragment = prog->getFragmentShader();
    QString vertex = prog->getVertexShader();

    if(fragment.isEmpty())
        ui->fragmentShBox->setCurrentIndex(-1);
    else
    {
        if(!actProj->hasFragmentShader(fragment))
        {
            QMessageBox box;
            box.setText(tr("Fragment shader '%1' do not exists!").arg(fragment));
            box.setIcon(QMessageBox::Warning);

            box.exec();

            prog->setFragmentShader(actProj->getFragmentRelativePaths().value(0));
            fragment = prog->getFragmentShader();
        }

        int pos = ui->fragmentShBox->findData(QVariant(fragment));
        ui->fragmentShBox->setCurrentIndex(pos);
    }

    if(vertex.isEmpty())
        ui->vertexShBox->setCurrentIndex(-1);
    else
    {
        if(!actProj->hasVertexShader(vertex))
        {
            QMessageBox box;
            box.setText(tr("Vertex shader '%1' do not exists!").arg(vertex));
            box.setIcon(QMessageBox::Warning);

            box.exec();

            prog->setVertexShader(actProj->getVertexRelativePaths().value(0));
            vertex = prog->getVertexShader();
        }

        int pos = ui->vertexShBox->findData(QVariant(vertex));
        ui->vertexShBox->setCurrentIndex(pos);
    }

    // set attach points
    ui->verticesEdit->setText(prog->getVerticesAttach());
    ui->normalsEdit->setText(prog->getNormalsAttach());

    // remove all row widgets from dialog
    removeAllWidgets();

    // append new rows
    QList<uint> colorPositions = prog->getColorPositions();

    foreach(uint pos, colorPositions)
    {
        addColorsWidget(pos, prog->getColor(pos));
    }

    QList<uint> texCoordPositions = prog->getTexCoordPositions();

    foreach(uint pos, texCoordPositions)
    {
        addTexCoordsWidget(pos, prog->getTexCoord(pos));
    }

    QStringList list = prog->getTexturePoints();

    foreach(QString point, list)
    {
        if(point.isEmpty())
        {
            QMessageBox box;
            box.setText(tr("Unknow error with texture attachment point!"));
            box.setIcon(QMessageBox::Warning);
            box.exec();

            prog->removeTexture(point);
            continue;
        }
        QString tex = prog->getTexture(point);

        if(!actProj->hasTexture(tex))
        {
            QMessageBox box;
            box.setText(tr("Texture '%1' for attachment '%2' is missing!!").arg(tex,point));
            box.setInformativeText(tr("This attachment will be removed from shader program."));
            box.setIcon(QMessageBox::Warning);
            box.exec();

            prog->removeTexture(point);
            continue;
        }

        addTextureWidget(tex, point);
    }
}

/**
 * @brief ProgramManagerDialog::testProgramName Test and set name of the program.
 */
void ProgramManagerDialog::testProgramName()
{
    QString newName = ui->progNameEdit->text();
    QString oldName = ui->listView->currentIndex().data().toString();
    QMessageBox box;
    box.setIcon(QMessageBox::Warning);

    if(newName == oldName)
        return;

    if(newName.isEmpty())
    {
        box.setText(tr("You canno't use empty name!"));
        box.exec();
        setOldProgName();
        return;
    }

    QStringList names = info->getActiveProject()->getProgramNames();

    if(names.contains(newName))
    {
        box.setText(tr("This shader program name is used!"));
        box.exec();
        setOldProgName();
        return;
    }

    MetaShaderProg* prog = info->getActiveProject()->getProgram(oldName);
    prog->setProgramName(ui->progNameEdit->text());

    ui->listView->selectionModel()->clearSelection();
}

/**
 * @brief ProgramManagerDialog::vertexShaderSet Set vertex shader to shader program.
 * @param currentIndex Index of combo box with vertex shaders.
 */
void ProgramManagerDialog::vertexShaderSet(int currentIndex)
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    QString name = ui->vertexShBox->itemData(currentIndex).toString();

    if(name.isEmpty() || name == prog->getVertexShader())
        return;

    prog->setVertexShader(name);
}

/**
 * @brief ProgramManagerDialog::fragmentShaderSet Set fragment shader to shader program.
 * @param currentIndex Index of combo box with fragment shaders.
 */
void ProgramManagerDialog::fragmentShaderSet(int currentIndex)
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    QString name = ui->fragmentShBox->itemData(currentIndex).toString();

    if(name.isEmpty() || name == prog->getFragmentShader())
        return;

    prog->setFragmentShader(name);
}

/**
 * @brief ProgramManagerDialog::verticesVariableSet Set vertices attachment point from dialog to shader program.
 */
void ProgramManagerDialog::verticesVariableSet()
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    prog->setVerticesAttach(ui->verticesEdit->text());
}

/**
 * @brief ProgramManagerDialog::normalsVariableSet Set normals attachment point from dialog to shader program.
 */
void ProgramManagerDialog::normalsVariableSet()
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    prog->setNormalsAttach(ui->normalsEdit->text());
}

/**
 * @brief ProgramManagerDialog::colorsVariableSet Add color variable to shader program.
 * @param oldName Old name of color variable.
 * @param newName New name of color variable.
 */
void ProgramManagerDialog::colorsVariableSet(QString oldName, QString newName, uint position)
{
    if(newName.isEmpty() || oldName == newName)
        return;

    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    //if(prog->getColorPositions().contains(position))
    //    return;

    if(prog->isColor(oldName))
        prog->removeColor(oldName);

    prog->addColor(position,newName);
}

/**
 * @brief ProgramManagerDialog::colorsVariableRemoved Remove color variable from shader program.
 * @param name Name of the variable to remove.
 */
void ProgramManagerDialog::colorsVariableRemoved(const QString name)
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    prog->removeColor(name);
}

/**
 * @brief ProgramManagerDialog::texCoordsVariableSet Add texture coordinate variable to shader program and remove old value set.
 * @param oldName Old name of texture coord variable.
 * @param newName New name of texture coord variable.
 */
void ProgramManagerDialog::texCoordsVariableSet(QString oldName, QString newName, uint position)
{
    if(newName.isEmpty() || oldName == newName)
        return;

    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    //ShProgTextureWidget* widget = static_cast<ShProgTextureWidget*>(sender());

    //if(widget->getVariableValue())

    //if(prog->getTexCoordPositions().contains(position))
    //    return;

    if(prog->isTexCoord(oldName))
        prog->removeTexCoord(oldName);

    prog->addTexCoord(position, newName);

}

/**
 * @brief ProgramManagerDialog::texCoordsVariableRemoved Remove texture coordinate from shader program.
 * @param name Name of the variable to remove.
 */
void ProgramManagerDialog::texCoordsVariableRemoved(const QString name)
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    prog->removeTexCoord(name);
}

/**
 * @brief ProgramManagerDialog::textureVariableSet Change texture name attached to given point.
 * @param point Name of the attached point.
 * @param newTexture New texture name to set.
 */
void ProgramManagerDialog::textureVariableSet(QString point, QString newTexture)
{
    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    prog->setNewTexture(point, newTexture);
}

/**
 * @brief ProgramManagerDialog::texturePointVariableSet Change name of the attachment point for given texture row.
 * @param oldPoint Old name of this point.
 * @param newPoint New name of this point.
 */
void ProgramManagerDialog::texturePointVariableSet(QString oldPoint, QString newPoint)
{
    if(oldPoint == newPoint)
        return;

    MetaShaderProg* prog = getActiveShProgram();

    Q_ASSERT(prog != NULL);
    if(prog == NULL)
        return;

    ShProgTextureWidget* widget = static_cast<ShProgTextureWidget*>(sender());

    QStringList usedPoints = prog->getTexturePoints();

    if(usedPoints.contains(newPoint))
    {
        widget->setPointName(oldPoint);
        return;
    }

    QString texture = widget->getTexture();

    prog->removeTexture(oldPoint);
    Q_ASSERT(prog->addTexture(newPoint, texture));

}

/**
 * @brief ProgramManagerDialog::textureVariableRemoved Remove texture variable with attachment point from selected shader program.
 * @param name Name of the attachment point.
 */
void ProgramManagerDialog::textureVariableRemoved(const QString name)
{
    MetaShaderProg* prog = getActiveShProgram();

    Q_ASSERT(prog != NULL);
    if(prog == NULL)
        return;

    prog->removeTexture(name);
}

/**
 * @brief ProgramManagerDialog::testColorPosition Test if valid color position is used.
 * @param name Name of the color attachment point.
 * @param position Position of the color attachment point.
 */
void ProgramManagerDialog::testColorPosition(QString name, uint position)
{
    // if name is empty this row is not valid for attachment creation
    if(name.isEmpty())
    {
        QList<uint> list = getPositions(colorsWidgets);
        list.removeOne(position);

        if(list.contains(position))
        {
            uint newPos = firstFreePosition(list);

            ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());
            widget->setPosition(newPos);
        }

        return;
    }

    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    if(prog->isColor(position))
    {
        if(prog->getColor(position) != name) // same position twice == error
        {
            ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());

            widget->setPosition(prog->getColor(name));
        }
    }
    else // this position is free to use
    {
        prog->removeColor(name);
        prog->addColor(position, name);
    }
}

/**
 * @brief ProgramManagerDialog::testTexCoordPosition Test texture coordinate position.
 * @param name Name of this texture coordinate attachment point.
 * @param position Position od this attachment point in list.
 */
void ProgramManagerDialog::testTexCoordPosition(QString name, uint position)
{
    // if name is not valid we canno't create this attachment
    if(name.isEmpty())
    {
        QList<uint> list = getPositions(colorsWidgets);
        list.removeOne(position);

        if(list.contains(position))
        {
            uint newPos = firstFreePosition(list);

            ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());
            widget->setPosition(newPos);
        }

        return;
    }

    MetaShaderProg* prog = getActiveShProgram();

    if(prog == NULL)
        return;

    if(prog->isTexCoord(position))
    {
        if(prog->getTexCoord(position) != name) // same position twice == error
        {
            ShProgVariableWidget* widget = static_cast<ShProgVariableWidget*>(sender());

            widget->setPosition(prog->getTexCoord(name));
        }
    }
    else // this position is free to use
    {
        prog->removeTexCoord(name);
        prog->addTexCoord(position, name);
    }
}
