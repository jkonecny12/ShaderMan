#include "standardmodelunifvalidity.h"
#include "uniform/storage/uniformcell.h"

/**
 * @brief StandardModelUnifValidity::StandardModelUnifValidity Create new model for uniform variables and
 * for test validity of this variables.
 * @param type Type of this variable.
 * @param parent Parent of this model.
 */
StandardModelUnifValidity::StandardModelUnifValidity(UniformTypes::UNIFORM_TYPES type, QObject *parent) :
    QStandardItemModel(parent)
{
    this->type = type;

    connect(this,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(testItem(QStandardItem*)));
}

/**
 * @brief StandardModelUnifValidity::isValid Test if this model variables is valid.
 * @param mark Mark invalid cells with red color.
 * @return True if model is valid.
 */
bool StandardModelUnifValidity::isValid(bool mark)
{
    int rows = rowCount();
    int columns = columnCount();
    bool valid = true;

    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < columns; ++c)
        {
            if(mark)
            {
                if(testValidity(item(r, c)))
                {
                    valid = false;
                    setValid(item(r, c), false);
                }
                else
                    setValid(item(r, c), true);

            } else
            {
                if(!testValidity(item(r, c)))
                    valid = false;
            }
        }
    }

    return valid;
}

/**
 * @brief StandardModelUnifValidity::setType Set scalar type of setted uniform variable.
 * @param type Scalar type of variable.
 */
void StandardModelUnifValidity::setType(UniformTypes::UNIFORM_TYPES type)
{
    this->type = type;
}

/**
 * @brief StandardModelUnifValidity::testValidity  Test item for validity.
 * @param item Item of uniform variable.
 * @return True if item is valid.
 */
bool StandardModelUnifValidity::testValidity(QStandardItem *item)
{
    QString text = item->text();

    if(type == UniformTypes::INT)
    {
        UniformCell<int> cell(text);
        return cell.testValidity();
    }
    else if(type == UniformTypes::UINT)
    {
        UniformCell<uint> cell(text);
        return cell.testValidity();
    }
    else if(type == UniformTypes::FLOAT)
    {
        UniformCell<float> cell(text);
        return cell.testValidity();
    }
    /*
    else if(type == UniformTypes::DOUBLE)
    {
        UniformCell<double> cell(text);
        return cell.testValidity();
    }
    */

    return false;
}

/**
 * @brief StandardModelUnifValidity::setValid Set red color if valid variable is false.
 * @param item Item where we setting this.
 * @param valid Is this variable valid.
 */
void StandardModelUnifValidity::setValid(QStandardItem *item, bool valid)
{
    if(!valid)
    {
        QColor color(Qt::red);
        item->setForeground(color);
    }
    else
    {
        QColor color(Qt::black);
        item->setForeground(color);
    }
}

/**
 * @brief StandardModelUnifValidity::testItem Test item for validity and mark with red color not valid cells.
 * @param item Item of this model.
 */
void StandardModelUnifValidity::testItem(QStandardItem *item)
{
    bool isValid = testValidity(item);

    setValid(item, isValid);
}
