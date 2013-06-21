#include "uniformvariable.h"
#include <QDebug>

using namespace UniformTypes;

/**
 * @brief UniformVariable::UniformVariable Create empty object for store information about uniform variable.
 */
UniformVariable::UniformVariable()
{
}

/**
 * @brief UniformVariable::UniformVariable Create object for store information about uniform variable.
 * @param name Name of this uniform variable.
 * @param values Values what we want to store in this uniform variable.
 * @param scalarType If variable is float, int uint etc.
 * @param uniformSize Size of the uniform (mat2x2, vec2 etc.).
 * @param uniformType Type of the uniform it is scalar and type in one (float mat2x2 etc.).
 */
UniformVariable::UniformVariable(QString name, QList<QVariant> values,
                                 UniformTypes::UNIFORM_TYPES scalarType,
                                 UniformTypes::UNIFORM_TYPES uniformSize,
                                 UniformTypes::UNIFORM_TYPES uniformType, bool multiplyMode)
{
    loadVariableData(name, values, scalarType, uniformSize, uniformType, multiplyMode);
}

/**
 * @brief UniformVariable::~UniformVariable Free all alocated space.
 */
UniformVariable::~UniformVariable()
{
    qDeleteAll(cellsInt);
    qDeleteAll(cellsUInt);
    qDeleteAll(cellsFloat);
    //qDeleteAll(cellsDouble);

    qDeleteAll(cellTimerTime);
    qDeleteAll(cellTimerActionPressed);
    qDeleteAll(cellAction);
}

/**
 * @brief UniformVariable::loadVariableData Load data to this uniform variable same as create new variable.
 * @param name Name of this uniform variable.
 * @param values Values what we want to store in this uniform variable.
 * @param scalarType If variable is float, int uint etc.
 * @param uniformSize Size of the uniform (mat2x2, vec2 etc.).
 * @param uniformType Type of the uniform it is scalar and type in one (float mat2x2 etc.).
 */
void UniformVariable::loadVariableData(QString name,
                                       QList<QVariant> values,
                                       UNIFORM_TYPES scalarType, UNIFORM_TYPES uniformSize,
                                       UNIFORM_TYPES uniformType, bool multiplyMode)
{
    removeOldSettings();

    this->name = name;
    this->values = values;
    this->scalarType = scalarType;
    this->uniformSize = uniformSize;
    this->uniformType = uniformType;
    this->isMultiply = multiplyMode;

    switch(uniformSize)
    {
    case SCALAR:
        rowCount = 1;
        columnCount = 1;
        break;

    case VEC2:
        rowCount = 1;
        columnCount = 2;
        break;

    case VEC3:
        rowCount = 1;
        columnCount = 3;
        break;

    case VEC4:
        rowCount = 1;
        columnCount = 4;
        break;

    case MAT2:
        rowCount = 2;
        columnCount = 2;
        break;

    case MAT3:
        rowCount = 3;
        columnCount = 3;
        break;

    case MAT4:
        rowCount = 4;
        columnCount = 4;
        break;

    case MAT2X3:
        rowCount = 3;
        columnCount = 2;
        break;

    case MAT2X4:
        rowCount = 4;
        columnCount = 2;
        break;

    case MAT3X2:
        rowCount = 2;
        columnCount = 3;
        break;

    case MAT3X4:
        rowCount = 4;
        columnCount = 3;
        break;

    case MAT4X2:
        rowCount = 2;
        columnCount = 4;
        break;

    case MAT4X3:
        rowCount = 3;
        columnCount = 4;
        break;

    default:
        rowCount = 1;
        columnCount = 1;
        break;
    }

    if(!multiplyMode)
    {
        //save this variables as uniform cells
        createCells();

        findTimers();
    }
}

/**
 * @brief UniformVariable::getName Get name of this uniform variable.
 * @return Return name of this uniform variable.
 */
QString UniformVariable::getName() const
{
    return name;
}

/**
 * @brief UniformVariable::getValues Get values of this uniform variable as QVariant strings.
 * @return Return values of this uniform variable.
 */
QList<QVariant> UniformVariable::getValues() const
{
    return values;
}

/**
 * @brief UniformVariable::getScalarType Get float, int, uint etc. of this uniform variable.
 * @return Return scalar type of this variable.
 */
UNIFORM_TYPES UniformVariable::getScalarType() const
{
    return scalarType;
}

/**
 * @brief UniformVariable::getUniformSize Get size of this uniform variable.
 * @return Return size of the uniform variable. If it is mat4, vec3 etc.
 */
UNIFORM_TYPES UniformVariable::getUniformSize() const
{
    return uniformSize;
}

/**
 * @brief UniformVariable::getUniformType Get exact type of this uniform variable.
 * @return Return type of this uniform variable. Float mat4 etc.
 */
UNIFORM_TYPES UniformVariable::getUniformType() const
{
    return uniformType;
}

/**
 * @brief UniformVariable::getUniformTypeAsString Get uniform variable type as string.
 * @return String with type of this uniform variable.
 */
QString UniformVariable::getUniformTypeAsString() const
{
    QString ret;

    switch(uniformType)
    {
    case FLOAT:
        ret = "FLOAT";
        break;
    //case DOUBLE:
    //    ret = "DOUBLE";
    //    break;
    case INT:
        ret = "INT";
        break;
    case UINT:
        ret = "UINT";
        break;
    //case BOOL:
    //    ret = "BOOL";
    //    break;
    case SCALAR:
        ret = "SCALAR";
        break;
    case MAT2:
        ret = "MAT2";
        break;
    case MAT3:
        ret = "MAT3";
        break;
    case MAT4:
        ret = "MAT4";
        break;
    case MAT2X3:
        ret = "MAT2X3";
        break;
    case MAT2X4:
        ret = "MAT2X4";
        break;
    case MAT3X2:
        ret = "MAT3X2";
        break;
    case MAT3X4:
        ret = "MAT3X4";
        break;
    case MAT4X2:
        ret = "MAT4X2";
        break;
    case MAT4X3:
        ret = "MAT4X3";
        break;
    /*
    case DMAT2:
        ret = "DMAT2";
        break;
    case DMAT3:
        ret = "DMAT3";
        break;
    case DMAT4:
        ret = "DMAT4";
        break;
    case DMAT2X3:
        ret = "DMAT2X3";
        break;
    case DMAT2X4:
        ret = "DMAT2X4";
        break;
    case DMAT3X2:
        ret = "DMAT3X2";
        break;
    case DMAT3X4:
        ret = "DMAT3X4";
        break;
    case DMAT4X2:
        ret = "DMAT4X2";
        break;
    case DMAT4X3:
        ret = "DMAT4X3";
        break;
    */
    case VEC2:
        ret = "VEC2";
        break;
    case VEC3:
        ret = "VEC3";
        break;
    case VEC4:
        ret = "VEC4";
        break;
    /*
    case IVEC2:
        ret = "IVEC2";
        break;
    case IVEC3:
        ret = "IVEC3";
        break;
    case IVEC4:
        ret = "IVEC4";
        break;

    case BVEC2:
        ret = "BVEC2";
        break;
    case BVEC3:
        ret = "BVEC3";
        break;
    case BVEC4:
        ret = "BVEC4";
        break;
    case DVEC2:
        ret = "DVEC2";
        break;
    case DVEC3:
        ret = "DVEC3";
        break;
    case DVEC4:
        ret = "DVEC4";
        break;
    */
    /*
    case UVEC2:
        ret = "UVEC2";
        break;
    case UVEC3:
        ret = "UVEC3";
        break;
    case UVEC4:
        ret = "UVEC4";
        break;
    */

    default:
        ret = "";
    }

    return ret;
}

/**
 * @brief UniformVariable::getRowsCount Get number of rows in this uniform variable. Uniform variable can be matrix.
 * @return Return number of rows of this uniform variable.
 */
int UniformVariable::getRowsCount() const
{
    return rowCount;
}

/**
 * @brief UniformVariable::getColumnCount Get number of columns in this uniform variable. Uniform variable can be matrix.
 * @return Return number of columns of this uniform variable.
 */
int UniformVariable::getColumnCount() const
{
    return columnCount;
}

/**
 * @brief UniformVariable::getVarCount Get number of variables of setted type.
 * @return Number of variables of setted type (mat4, vec2...)
 */
int UniformVariable::getVarCount() const
{
    int count = rowCount * columnCount;

    return values.size() / count;
}

/**
 * @brief UniformVariable::isArray Test if this variable is array.
 * @return Return true if this uniform variable is array, false otherwise.
 */
bool UniformVariable::isArray() const
{
    if(isMultiply)
        return false;

    int one = rowCount * columnCount;

    return one < values.count();
}

/**
 * @brief UniformVariable::setMultiplyMode Set this uniform variable to multiply mode, where we only multiply other uniform variables.
 * If setted as true, then this uniform will not have uniform cells for calculation, but values are uniform variable names.
 * @param multiply If true, then set this uniform to multiply mode. if false use this variable as usual.
 */
void UniformVariable::setMultiplyMode(bool multiply)
{
    isMultiply = multiply;
}

/**
 * @brief UniformVariable::isMultiplyMode Test if this variable is it in multiply mode, where we multiply other uniforms.
 * @return True if this variable is in multiply mode, false otherwise.
 */
bool UniformVariable::isMultiplyMode() const
{
    return isMultiply;
}

/**
 * @brief UniformVariable::isAttached Return true if this uniform variable is attached somewhere
 * @return True if is attached or False
 */
bool UniformVariable::isAttached() const
{
    return !(attach.isEmpty());
}

/**
 * @brief UniformVariable::addAttachment Add attachment of this uniform variable to uniform variable point in shader in given shader program.
 * @param programName Shader program name, where we want use this variable.
 * @param variableName Name of the uniform variable attachment point.
 */
void UniformVariable::addAttachment(QString programName, QString variableName)
{
    foreach(Attachment a, attach)
    {
        if(a.programName == programName && a.variableName == variableName)
            return;
    }

    Attachment a;

    a.programName  = programName;
    a.variableName = variableName;

    attach.append(a);
}

/**
 * @brief UniformVariable::removeAttachment Remove attachment of this uniform variable to uniform variable point in shader in given shader program.
 * @param programName Shader program name, where we want use this variable.
 * @param variableName Name of the uniform variable attachment point.
 * @return True if given attachment was removed, false if attachment was not found.
 */
bool UniformVariable::removeAttachment(QString programName, QString variableName)
{
    QMutableListIterator<Attachment> it(attach);

    while(it.hasNext())
    {
        if(it.peekNext().programName == programName)
        {
            if(it.next().variableName == variableName)
            {
                it.remove();
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief UniformVariable::getAttachedVariables Get attachment variable point for given shader program.
 * @param programName Name of the shader program, where this attachment point is used.
 * @return Return list of attachment points in this shader program.
 */
QStringList UniformVariable::getAttachedVariables(QString programName) const
{
    QListIterator<Attachment> it(attach);
    QStringList ret;

    while(it.hasNext())
    {
        Attachment a = it.next();
        if(a.programName == programName)
        {
            ret.append(a.variableName);
        }
    }

    return ret;
}

/**
 * @brief UniformVariable::getAttached Get list of attachment.
 * @return Structure of attachment points.
 */
QList<Attachment> UniformVariable::getAttached() const
{
    return attach;
}

/**
 * @brief UniformVariable::getValueInt Get calculated value as integer value.
 * @return Result as integer.
 */
int UniformVariable::getValueInt() const
{
    UniformCell<int>* cell = cellsInt.value(0);

    if(cell == NULL)
        return 0;

    return cell->getResult();
}

/**
 * @brief UniformVariable::getValueUInt Get calculated value as unsigned integer value.
 * @return Result as unsigned integer.
 */
uint UniformVariable::getValueUInt() const
{
    UniformCell<uint>* cell = cellsUInt.value(0);

    if(cell == NULL)
        return 0;

    return cell->getResult();
}

/**
 * @brief UniformVariable::getValueFloat Get calculated value as float value.
 * @return Result as float.
 */
float UniformVariable::getValueFloat() const
{
    UniformCell<float>* cell = cellsFloat.value(0);

    if(cell == NULL)
        return 0.0;

    return cell->getResult();
}

/**
 * @brief UniformVariable::getValuesInt Get calculated values as ints.
 * @return Result as ints.
 */
int *UniformVariable::getValuesInt() const
{
    int count = getVarCount();
    int* ret = new int[count];
    UniformCell<int>* cell;

    for(int i = 0; i < count; ++i)
    {
        cell = cellsInt.value(i);

        if(cell == NULL)
            ret[i] = 0;
        else
            ret[i] = cell->getResult();
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesUInt Get calculated values as uints.
 * @return Result as uints.
 */
uint *UniformVariable::getValuesUInt() const
{
    int count = getVarCount();
    uint* ret = new uint[count];
    UniformCell<uint>* cell;

    for(int i = 0; i < count; ++i)
    {
        cell = cellsUInt.value(i);

        if(cell == NULL)
            ret[i] = 0;
        else
            ret[i] = cell->getResult();
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesFloat Get calculated values as floats.
 * @return Result as floats.
 */
float *UniformVariable::getValuesFloat() const
{
    int count = getVarCount();
    float* ret = new float[count];
    UniformCell<float>* cell;

    for(int i = 0; i < count; ++i)
    {
        cell = cellsFloat.value(i);

        if(cell == NULL)
            ret[i] = 0;
        else
            ret[i] = cell->getResult();
    }

    return ret;
}

/*
double UniformVariable::getValueDouble() const
{
    UniformCell<double>* cell = cellsDouble.value(0);

    if(cell == NULL)
        return 0;

    return cell->getResult();
}
*/

/**
 * @brief UniformVariable::getValueMat2x2 Get value of this uniform variable as QMatrix2x2.
 * @param start From what number in array we want to get this matrix.
 * @return Return values as matrix 2x2.
 */
QMatrix2x2 UniformVariable::getValueMat2x2(int start) const
{
    if(cellsFloat.size() <= start + 3)
        return QMatrix2x2();

    float array[4];

    for(int i = start; i < start+4; ++i)
    {
        //array[i] = values.value(i).toReal();
        array[i] = cellsFloat.value(i)->getResult();
    }

    return QMatrix2x2(array);
}

/**
 * @brief UniformVariable::getValueMat3x3 Get value of this uniform variable as QMatrix 3x3.
 * @param start From what number in array we want to get this matrix.
 * @return Return values as matrix 3x3.
 */
QMatrix3x3 UniformVariable::getValueMat3x3(int start) const
{
    if(cellsFloat.size() <= start + 8)
        return QMatrix3x3();

    float array[9];

    for(int i = start; i < start+9; ++i)
    {
        //array[i] = values.value(i).toReal()
        array[i] = static_cast<float>(cellsFloat.value(i)->getResult());
    }

    return QMatrix3x3(array);
}

/**
 * @brief UniformVariable::getValueMat4x4 Get value of this shader uniform variable as QMatrix 4x4.
 * @param start From what number in array we want to get this matrix.
 * @return Return values as matrix 4x4.
 */
QMatrix4x4 UniformVariable::getValueMat4x4(int start) const
{
    if(cellsFloat.size() <= start + 15)
        return QMatrix4x4();


    float array[16];

    for(int i = start; i < start+16; ++i)
    {
        //array[i] = values.value(i).toReal();
        array[i] = cellsFloat.value(i)->getResult();
    }

    return QMatrix4x4(array);
}

/**
 * @brief UniformVariable::getValuesMat2x2 Get values of this shader uniform variable as QMatrix 2x2.
 * @return Return values as matrix 2x2
 */
QMatrix2x2 *UniformVariable::getValuesMat2x2() const
{
    int count = getVarCount();
    QMatrix2x2 *ret = new QMatrix2x2[count];
    const int varCount = 2 * 2;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueMat2x2(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesMat3x3 Get values of this shader uniform variable as QMatrix 3x3.
 * @return Return values as matrix 3x3.
 */
QMatrix3x3 *UniformVariable::getValuesMat3x3() const
{
    int count = getVarCount();
    QMatrix3x3 *ret = new QMatrix3x3[count];
    const int varCount = 3 * 3;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueMat3x3(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesMat4x4 Get values of this shader uniform variable as QMatrix 4x4.
 * @return Return values as matrix 4x4.
 */
QMatrix4x4 *UniformVariable::getValuesMat4x4() const
{
    int count = getVarCount();
    QMatrix4x4 *ret = new QMatrix4x4[count];
    const int varCount = 4 * 4;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueMat4x4(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getValueVec2D Get value of this shader uniform variable as QVector 2D.
 * @param start From what number in array we want to get this vector.
 * @return Return value as 2D vector.
 */
QVector2D UniformVariable::getValueVec2D(int start) const
{
    if(cellsFloat.size() <= start + 1)
        return QVector2D();

    QVector2D vec;

    //vec.setX(values.value(start).toReal());
    //vec.setY(values.value(start+1).toReal());

    vec.setX(cellsFloat.value(start)->getResult());
    vec.setY(cellsFloat.value(start)->getResult());

    return vec;
}

/**
 * @brief UniformVariable::getValueVec3D Get value of this shader uniform variable as QVector 3D.
 * @param start From what number in array we want to get this vector.
 * @return Return value as 3D vector.
 */
QVector3D UniformVariable::getValueVec3D(int start) const
{
    if(cellsFloat.size() <= start + 2)
        return QVector3D();

    QVector3D vec;

    //vec.setX(values.value(start).toReal());
    //vec.setY(values.value(start+1).toReal());
    //vec.setZ(values.value(start+2).toReal());
    vec.setX(cellsFloat.value(start)->getResult());
    vec.setY(cellsFloat.value(start+1)->getResult());
    vec.setZ(cellsFloat.value(start+2)->getResult());

    return vec;
}

/**
 * @brief UniformVariable::getValueVec4D Get value of this shader uniform variable as QVector 4D.
 * @param start From what number in array we want to get this vector.
 * @return Return value as 4D vector.
 */
QVector4D UniformVariable::getValueVec4D(int start) const
{
    if(cellsFloat.size() <= start + 3)
        return QVector4D();

    QVector4D vec;

    //vec.setX(values.value(start).toReal());
    //vec.setY(values.value(start+1).toReal());
    //vec.setZ(values.value(start+2).toReal());
    //vec.setW(values.value(start+3).toReal());

    vec.setX(cellsFloat.value(start)->getResult());
    vec.setY(cellsFloat.value(start+1)->getResult());
    vec.setZ(cellsFloat.value(start+2)->getResult());
    vec.setW(cellsFloat.value(start+3)->getResult());

    return vec;
}

/**
 * @brief UniformVariable::getValuesVec2D Get values of this shader uniform variable as QVector 2D.
 * @return Return values as 2D vector.
 */
QVector2D *UniformVariable::getValuesVec2D() const
{
    int count = getVarCount();
    QVector2D *ret = new QVector2D[count];
    const int varCount = 2;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueVec2D(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesVec3D Get values of this shader uniform variable as QVector 3D.
 * @return Return values as 3D vector.
 */
QVector3D *UniformVariable::getValuesVec3D() const
{
    int count = getVarCount();
    QVector3D *ret = new QVector3D[count];
    const int varCount = 3;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueVec3D(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getValuesVec4D Get values of this shader uniform variable as QVector 4D.
 * @return Return values as 4D vector.
 */
QVector4D *UniformVariable::getValuesVec4D() const
{
    int count = getVarCount();
    QVector4D *ret = new QVector4D[count];
    const int varCount = 4;

    for(int i = 0; i < count; ++i)
    {
        ret[i] = getValueVec4D(i*varCount);
    }

    return ret;
}

/**
 * @brief UniformVariable::getTimeTimers Get all unique refresh time for special variable $Time.
 * @return All refresh intervals for Time special variable.
 */
QList<long> UniformVariable::getTimeTimers()
{
    QList<long> timers;

    foreach(long t, cellTimerTime.keys())
    {
        if(t > 0)
            timers.append(t);
    }

    return timers;
}

/**
 * @brief UniformVariable::incrementTimeTimers Increment time variables with given refresh time.
 * @param time Refresh time of variables.
 */
void UniformVariable::incrementTimeTimers(long time)
{
    QList<QPair<int,int> >* timers = cellTimerTime.value(time);
    QListIterator<QPair<int,int> > it(*timers);

    while(it.hasNext())
    {
        if(scalarType == INT)
        {
            SpecialVariableTime<int>* time = static_cast<SpecialVariableTime<int>*>(
                        cellsInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));
            time->incValue();
            cellsInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == UINT)
        {
            SpecialVariableTime<uint>* time = static_cast<SpecialVariableTime<uint>*>(
                        cellsUInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));
            time->incValue();
            cellsUInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == FLOAT)
        {
            SpecialVariableTime<float>* time = static_cast<SpecialVariableTime<float>*>(
                        cellsFloat.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));
            time->incValue();
            cellsFloat.value(it.peekNext().first)->calculate();
        }
        /*
        else if(scalarType == DOUBLE)
        {
            SpecialVariableTime<double>* time = static_cast<SpecialVariableTime<double>*>(
                        cellsDouble.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));
            time->incValue();
            cellsDouble.value(it.peekNext().first)->calculate();
        }
        */

        it.next();
    }
}

/**
 * @brief UniformVariable::getActionPressedTimers Get all unique refresh time for special variable $ActionPressed.
 * @return All refresh time for $ActionPressed.
 */
QList<long> UniformVariable::getActionPressedTimers()
{
    QList<long> timers;

    foreach (long t, cellTimerActionPressed.keys())
    {
        if(t > 0)
            timers.append(t);
    }

    return timers;
}

/**
 * @brief UniformVariable::incrementActionPressedTimers Increment all special variable $ActionPressed with given refresh time.
 * Given field buttonPressed is used for testing button if pressed.
 * @param time Refresh time.
 * @param buttonPressed Position in field is equal to buttonId. If true then button is pressed, if false released.
 */
void UniformVariable::incrementActionPressedTimers(long time, bool* buttonPressed)
{
    QList<QPair<int,int> >* timers = cellTimerActionPressed.value(time);

    if(timers == NULL)
        return;

    QListIterator<QPair<int,int> > it(*timers);

    while(it.hasNext())
    {
        if(scalarType == INT)
        {
            SpecialVariableActionPressed<int>* pressed = static_cast<SpecialVariableActionPressed<int>*>(
                        cellsInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            bool isPressed = buttonPressed[pressed->getButtonId()];
            pressed->setPressed(isPressed);

            pressed->incValue();
            cellsInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == UINT)
        {
            SpecialVariableActionPressed<uint>* pressed = static_cast<SpecialVariableActionPressed<uint>*>(
                        cellsUInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            bool isPressed = buttonPressed[pressed->getButtonId()];
            pressed->setPressed(isPressed);

            pressed->incValue();
            cellsUInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == FLOAT)
        {
            SpecialVariableActionPressed<float>* pressed = static_cast<SpecialVariableActionPressed<float>*>(
                        cellsFloat.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            bool isPressed = buttonPressed[pressed->getButtonId()];
            pressed->setPressed(isPressed);

            pressed->incValue();
            cellsFloat.value(it.peekNext().first)->calculate();
        }
        /*
        else if(scalarType == DOUBLE)
        {
            SpecialVariableTime<double>* time = static_cast<SpecialVariableTime<double>*>(
                        cellsDouble.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));
            time->incValue();
            cellsDouble.value(it.peekNext().first)->calculate();
        }
        */

        it.next();
    }
}

/**
 * @brief UniformVariable::toggleAction Toggle all $Action special variables if given button is button they use.
 * @param buttonId Id of pressed button.
 */
void UniformVariable::toggleAction(int buttonId)
{
    if(!cellAction.contains(buttonId))
        return;

    QList<QPair<int,int> >* cells = cellAction.value(buttonId);

    for(int i = 0; i < cells->size(); ++i)
    {
        QPair<int,int> p = cells->value(i);

        if(scalarType == INT)
        {
            SpecialVariableAction<int>* action = static_cast<SpecialVariableAction<int>*>(
                        cellsInt.value(p.first)->getSpecialVariable(p.second));

            action->toggle();
            cellsInt.value(p.first)->calculate();
        }
        else if(scalarType == UINT)
        {
            SpecialVariableAction<uint>* action = static_cast<SpecialVariableAction<uint>*>(
                        cellsUInt.value(p.first)->getSpecialVariable(p.second));

            action->toggle();
            cellsUInt.value(p.first)->calculate();
        }
        else if(scalarType == FLOAT)
        {
            SpecialVariableAction<float>* action = static_cast<SpecialVariableAction<float>*>(
                        cellsFloat.value(p.first)->getSpecialVariable(p.second));

            action->toggle();
            cellsFloat.value(p.first)->calculate();
        }
    }
}

/**
 * @brief UniformVariable::resetToDefaults Reset all special variables in this uniform to default values.
 */
void UniformVariable::resetToDefaults()
{
    QList<QList<QPair<int,int> >*> timersPressed = cellTimerActionPressed.values();
    QList<QList<QPair<int,int> >*> timersTime = cellTimerTime.values();
    QList<QList<QPair<int,int> >*> timersAction = cellAction.values();

    // get all timers
    QList<QPair<int,int> > alltimers;

    for(int i = 0; i < timersPressed.size(); ++i)
    {
        alltimers.append(*timersPressed.value(i));
    }

    for(int i = 0; i < timersTime.size(); ++i)
    {
        alltimers.append(*timersTime.value(i));
    }

    for(int i = 0; i < timersAction.size(); ++i)
    {
        alltimers.append(*timersAction.value(i));
    }

    // iterate and default values in all timers
    QListIterator<QPair<int,int> > it(alltimers);

    while(it.hasNext())
    {
        if(scalarType == INT)
        {
            resetTimer(cellsInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            cellsInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == UINT)
        {
            resetTimer(cellsUInt.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            cellsUInt.value(it.peekNext().first)->calculate();
        }
        else if(scalarType == FLOAT)
        {
            resetTimer(cellsFloat.value(it.peekNext().first)->getSpecialVariable(it.peekNext().second));

            cellsFloat.value(it.peekNext().first)->calculate();
        }

        it.next();
    }
}



/** for serialization **/

/**
 * @brief operator << Operator for serialization of object UniformVariable.
 * @param stream Stream where we want to serialize object.
 * @param uniform Uniform variable what we want to serialize.
 * @return Return stream for another serialization.
 */
QDataStream & operator<< (QDataStream& stream, const UniformVariable& uniform)
{
    qDebug() << "Saving uniform variable " << uniform.name;

    /*
    stream << uniform.name;
    stream << uniform.values;
    stream << static_cast<quint8>(uniform.scalarType);
    stream << static_cast<quint8>(uniform.uniformSize);
    stream << static_cast<quint8>(uniform.uniformType);
    stream << static_cast<qint32>(uniform.rowCount);
    stream << static_cast<qint32>(uniform.columnCount);


    stream << static_cast<qint32>(uniform.attach.count());

    foreach(Attachment at, uniform.attach)
    {
        stream << at.programName;
        stream << at.variableName;
    }
    */

    uniform.saveUniform(stream, uniform);

    return stream;
}

/**
 * @brief operator >> Operator for deserialization of object UniformVariable.
 * @param stream Stream from where we read our object.
 * @param uniform Uniform variable what we get.
 * @return Return stream for another deserialization.
 */
QDataStream & operator>> (QDataStream& stream, UniformVariable& uniform)
{
    /*
    stream >> uniform.name;
    stream >> uniform.values;
    quint8 in;
    stream >> in;
    uniform.scalarType = static_cast<UNIFORM_TYPES>(in);
    stream >> in;
    uniform.uniformSize = static_cast<UNIFORM_TYPES>(in);
    stream >> in;
    uniform.uniformType = static_cast<UNIFORM_TYPES>(in);

    qint32 inInt;
    stream >> inInt;
    uniform.rowCount = inInt;
    stream >> inInt;
    uniform.columnCount = inInt;

    //stream >> uniform.attach;

    stream >> inInt;

    for(int i = 0; i < inInt; ++i)
    {
        Attachment at;

        stream >> at.programName;
        stream >> at.variableName;

        uniform.attach.append(at);
    }
    */

    uniform.loadUniform(stream, uniform);

    qDebug() << "Loading uniform variable " << uniform.name;

    return stream;
}

/**
 * @brief operator << Save uniform variable.
 * @param stream Serialize to this stream.
 * @param uniform Save this uniform variable
 * @return Return datastream.
 */
QDataStream & operator<< (QDataStream& stream, const UniformVariable* uniform)
{
    uniform->saveUniform(stream, *uniform);
    qDebug() << "Saving uniform variable " << uniform->name;

    return stream;
}

/**
 * @brief operator >> Load uniform variable.
 * @param stream Lad from this stream.
 * @param uniform Save to this uniform variable.
 * @return Return datastream.
 */
QDataStream & operator>> (QDataStream& stream, UniformVariable*& uniform)
{
    uniform = new UniformVariable();

    uniform->loadUniform(stream, *uniform);

    qDebug() << "Loading uniform variable " << uniform->name;

    return stream;
}

/**
 * @brief UniformVariable::findTimers Find all special variables in this uniform.
 * Save these to lists internal list for easy use and save refresh times for ActionPressed and Time.
 */
void UniformVariable::findTimers()
{
    if(scalarType == INT)
    {
        //foreach(UniformCell<int>* cell, cellsInt)
        for(int c = 0; c < cellsInt.size(); ++c)
        {
            UniformCell<int>* cell = cellsInt.value(c);
            int specVars = cell->getSpecVarCount();

            for(int i = 0; i < specVars; ++i)
            {
                setNewTimer(c, i, cell->getSpecialVariable(i));
            }
        }
    }
    else if(scalarType == UINT)
    {
        for(int c = 0; c < cellsUInt.size(); ++c)
        {
            UniformCell<uint>* cell = cellsUInt.value(c);
            int specVars = cell->getSpecVarCount();

            for(int i = 0; i < specVars; ++i)
            {
                setNewTimer(c, i, cell->getSpecialVariable(i));
            }
        }
    }
    else if(scalarType == FLOAT)
    {
        for(int c = 0; c < cellsFloat.size(); ++c)
        {
            UniformCell<float>* cell = cellsFloat.value(c);
            int specVars = cell->getSpecVarCount();

            for(int i = 0; i < specVars; ++i)
            {
                setNewTimer(c, i, cell->getSpecialVariable(i));
            }
        }
    }
}

/**
 * @brief UniformVariable::createCells Create uniform cell.
 */
void UniformVariable::createCells()
{
    foreach(QVariant val, values)
    {
        if(scalarType == INT)
        {
            UniformCell<int> *cell = new UniformCell<int>(val.toString());
            if(cell->testValidity())
            {
                cellsInt.append(cell);
                qDebug() << "Cell result: " << cell->getResult();
            }
        }
        else if(scalarType == UINT)
        {
            UniformCell<uint> *cell = new UniformCell<uint>(val.toString());
            if(cell->testValidity())
            {
                cellsUInt.append(cell);
                qDebug() << "Cell result: " << cell->getResult();
            }
        }
        else if(scalarType == FLOAT)
        {
            UniformCell<float> *cell = new UniformCell<float>(val.toString());
            if(cell->testValidity())
            {
                cellsFloat.append(cell);
                qDebug() << "Cell result: " << cell->getResult();
            }
        }
        /*
        else if(scalarType == DOUBLE)
        {
            UniformCell<double> *cell = new UniformCell<double>(val.toString());
            if(cell->testValidity())
                cellsDouble.append(cell);
        }
        */
    }
}

/**
 * @brief UniformVariable::saveUniform Serialize uniform variable to stream.
 * @param stream Where to serialize.
 * @param variable What to serialize.
 */
void UniformVariable::saveUniform(QDataStream &stream, const UniformVariable &variable)
{
    stream << variable.name;
    stream << variable.values;

    stream << variable.isMultiply;

    stream << (qint32)variable.scalarType;
    stream << (qint32)variable.uniformSize;
    stream << (qint32)variable.uniformType;
    stream << (qint32)variable.rowCount;
    stream << (qint32)variable.columnCount;

    QList<QString> progNameAttach;
    QList<QString> varNameAttach;

    foreach(Attachment ch, variable.attach)
    {
        progNameAttach.append(ch.programName);
        varNameAttach.append(ch.variableName);
    }

    stream << progNameAttach;
    stream << varNameAttach;

}

/**
 * @brief UniformVariable::loadUniform Load veriable from steam.
 * @param stream From where to serialize.
 * @param variable Save variable.
 */
void UniformVariable::loadUniform(QDataStream &stream, UniformVariable &variable)
{
    stream >> variable.name;
    stream >> variable.values;

    stream >> variable.isMultiply;

    qint32 v;

    stream >> v;
    variable.scalarType = static_cast<UNIFORM_TYPES>(v);

    stream >> v;
    variable.uniformSize = static_cast<UNIFORM_TYPES>(v);

    stream >> v;
    variable.uniformType = static_cast<UNIFORM_TYPES>(v);

    stream >> variable.rowCount;

    stream >> variable.columnCount;

    //stream >> variable.attach;

    QList<QString> progNameAttach;
    QList<QString> varNameAttach;

    stream >> progNameAttach;
    stream >> varNameAttach;

    for(int i = 0; i < progNameAttach.size(); ++i)
    {
        Attachment attach;
        attach.programName = progNameAttach.value(i);
        attach.variableName = varNameAttach.value(i);

        variable.attach.append(attach);
    }

    variable.createCells();
    variable.findTimers();
}


void UniformVariable::removeOldSettings()
{
    qDeleteAll(cellsInt);
    qDeleteAll(cellsUInt);
    qDeleteAll(cellsFloat);
    //qDeleteAll(cellsDouble);

    qDeleteAll(cellTimerTime);
    qDeleteAll(cellTimerActionPressed);
    qDeleteAll(cellAction);

    cellsInt.clear();
    cellsUInt.clear();
    cellsFloat.clear();\

    cellTimerTime.clear();
    cellTimerActionPressed.clear();
    cellAction.clear();
}
