#include "modelvaluesviewer.h"
#include "ui_modelvaluesviewer.h"
#include <QStandardItemModel>

ModelValuesViewer::ModelValuesViewer(Mesh *mesh, QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModelValuesViewer),
    mesh(mesh),
    name(name)
{
    ui->setupUi(this);

    ui->label->setText(name);

    initTable();
}

ModelValuesViewer::~ModelValuesViewer()
{
    delete ui;
}

/**
 * @brief ModelValuesViewer::initTable Initialize QTableView model
 */
void ModelValuesViewer::initTable()
{
    QStandardItemModel* model = new QStandardItemModel(this);
    QStandardItem* root = model->invisibleRootItem();
    float* data;
    unsigned int* dataUI;
    unsigned int maxNumber = 0;

    QStringList headerNames;

    QList<QStandardItem*> items;

    if(mesh->hasVertices())
    {
        data = mesh->getVertices();

        for(unsigned int i = 0; i < mesh->getNumberVertices(); ++i)
        {
            items.append(new QStandardItem(QString("%1").arg(data[i])));
        }

        headerNames << "Vertices";
        root->appendRows(items);
        items.clear();

        maxNumber = mesh->getNumberVertices();
    }

    if(mesh->hasNormals())
    {
        data = mesh->getNormals();

        for(unsigned int i = 0; i < mesh->getNumberNormals(); ++i)
        {
            items.append(new QStandardItem(QString("%1").arg(data[i])));
        }

        headerNames << "Normals";
        root->appendColumn(items);
        items.clear();

        maxNumber = qMax(maxNumber, mesh->getNumberNormals());
    }

    if(mesh->hasColors())
    {
        for(unsigned int y = 0; y < mesh->getColorBuffersCount(); ++y)
        {
            data = mesh->getColors(y);

            for(unsigned int i = 0; i < mesh->getNumberColors(y); ++i)
            {
                items.append(new QStandardItem(QString("%1").arg(data[i])));
            }

            headerNames << QString("Colors %1").arg(y);
            root->appendColumn(items);
            items.clear();

            maxNumber = qMax(maxNumber, mesh->getNumberColors(y));
        }
    }

    if(mesh->hasTexCoords())
    {
        for(unsigned int i = 0; i < mesh->getTexCoordBuffersCount(); ++i)
        {
            data = mesh->getTexCoords(i);

            for(unsigned int y = 0; y < mesh->getNumberTexCoords(i); ++y)
            {
                items.append(new QStandardItem(QString("%1").arg(data[y])));
            }

            headerNames << QString("Coordinates %1").arg(i);
            root->appendColumn(items);
            items.clear();

            maxNumber = qMax(maxNumber, mesh->getNumberTexCoords(i));
        }
    }

    if(mesh->hasIndices())
    {
        dataUI = mesh->getIndices();

        for(unsigned int i = 0; i < mesh->getNumberIndices(); ++i)
        {
            items.append(new QStandardItem(QString("%1").arg(dataUI[i])));
        }

        headerNames << "Indices";
        root->appendColumn(items);

        maxNumber = qMax(maxNumber, mesh->getNumberIndices());
    }

    QStringList vertHeader;

    // create vertical header labels (countering from 0)
    for(unsigned int i = 0; i < maxNumber; ++i)
    {
        vertHeader << QString("%1.").arg(i);
    }

    model->setHorizontalHeaderLabels(headerNames);
    model->setVerticalHeaderLabels(vertHeader);

    ui->dataTable->setModel(model);

}
