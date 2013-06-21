#ifndef UNIFORMVARIABLE_H
#define UNIFORMVARIABLE_H

#include <QVariant>
#include <QList>
#include <QStringList>
#include <QHash>
#include <QString>
#include <QGenericMatrix>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include "uniformcell.h"

namespace UniformTypes{

    enum UNIFORM_TYPES{
            //FLOAT, DOUBLE, INT, UINT, SCALAR,
            FLOAT, INT, UINT, SCALAR,
            MAT2, MAT3, MAT4,
            MAT2X3, MAT2X4,
            MAT3X2, MAT3X4,
            MAT4X2, MAT4X3,
            //DMAT2, DMAT3, DMAT4,
            //DMAT2X3, DMAT2X4,
            //DMAT3X2, DMAT3X4,
            //DMAT4X2, DMAT4X3,
            VEC2, VEC3, VEC4,
            IVEC2, IVEC3, IVEC4,
            //BVEC2, BVEC3, BVEC4,
            //DVEC2, DVEC3, DVEC4,
            UVEC2, UVEC3, UVEC4
    };

    struct Attachment {
        QString programName;
        QString variableName;
    };
}

/**
 * @brief The UniformVariable class Uniform variable class. Storage uniform variable in this class.
 */
class UniformVariable
{
public:
    explicit UniformVariable();
    explicit UniformVariable(QString name, QList<QVariant> values, UniformTypes::UNIFORM_TYPES scalarType,
                    UniformTypes::UNIFORM_TYPES uniformSize, UniformTypes::UNIFORM_TYPES uniformType,
                    bool multiplyMode = false);

    ~UniformVariable();

    void loadVariableData(QString name, QList<QVariant> values, UniformTypes::UNIFORM_TYPES scalarType,
                          UniformTypes::UNIFORM_TYPES uniformSize, UniformTypes::UNIFORM_TYPES uniformType,
                          bool multiplyMode = false);

    //setters and getters
    QString getName() const;
    QList<QVariant> getValues() const;
    UniformTypes::UNIFORM_TYPES getScalarType() const;
    UniformTypes::UNIFORM_TYPES getUniformSize() const;
    UniformTypes::UNIFORM_TYPES getUniformType() const;
    QString getUniformTypeAsString() const;
    int getRowsCount() const;
    int getColumnCount() const;
    int getVarCount() const;

    bool isArray() const;
    void setMultiplyMode(bool multiply);
    bool isMultiplyMode() const;

    //attachments to variables
    bool isAttached() const;
    void addAttachment(QString programName, QString variableName);
    bool removeAttachment(QString programName, QString variableName);
    QStringList getAttachedVariables(QString programName) const;
    QList<UniformTypes::Attachment> getAttached() const;

    //return values as
    int getValueInt() const;
    uint getValueUInt() const;
    float getValueFloat() const;

    int *getValuesInt() const;
    uint *getValuesUInt() const;
    float *getValuesFloat() const;

    //double getValueDouble() const;
    QMatrix2x2 getValueMat2x2(int start = 0) const;
    QMatrix3x3 getValueMat3x3(int start = 0) const;
    QMatrix4x4 getValueMat4x4(int start = 0) const;

    QMatrix2x2 *getValuesMat2x2() const;
    QMatrix3x3 *getValuesMat3x3() const;
    QMatrix4x4 *getValuesMat4x4() const;

    QVector2D getValueVec2D(int start = 0) const;
    QVector3D getValueVec3D(int start = 0) const;
    QVector4D getValueVec4D(int start = 0) const;

    QVector2D *getValuesVec2D() const;
    QVector3D *getValuesVec3D() const;
    QVector4D *getValuesVec4D() const;

    QList<long> getTimeTimers();
    void incrementTimeTimers(long time);
    QList<long> getActionPressedTimers();
    void incrementActionPressedTimers(long time, bool *buttonPressed);
    void toggleAction(int buttonId);

    void resetToDefaults();

    //for serializing
    friend QDataStream & operator<< (QDataStream& stream, const UniformVariable& uniform);
    friend QDataStream & operator>> (QDataStream& stream, UniformVariable& uniform);
    friend QDataStream & operator<< (QDataStream& stream, const UniformVariable* uniform);
    friend QDataStream & operator>> (QDataStream& stream, UniformVariable*& uniform);

private:
    void removeOldSettings();
    void createCells();
    void findTimers();
    static void saveUniform(QDataStream& stream, const UniformVariable& variable);
    static void loadUniform(QDataStream& stream, UniformVariable& variable);

    template <typename T>
    /**
     * @brief resetTimer Reset special variables of cell to default values.
     * @param cell Reset special variables in this UniformCell.
     */
    void resetTimer(SpecialVariableAbstract<T>* cell)
    {
        int  type = cell->getType();

        if(type == SpecialVariableAbstract<T>::TIME)
        {
            SpecialVariableTime<T>* time = static_cast<SpecialVariableTime<T>*>(cell);
            time->setDefault();
        }
        else if(type == SpecialVariableAbstract<T>::ACTION_PRESSED)
        {
            SpecialVariableActionPressed<T>* pressed = static_cast<SpecialVariableActionPressed<T>*>(cell);
            pressed->setDefault();
        }
        else if(type == SpecialVariableAbstract<T>::ACTION)
        {
            SpecialVariableAction<T>* action = static_cast<SpecialVariableAction<T>*>(cell);
            action->setDefault();
        }
    }

    template <typename T>
    /**
     * @brief setNewTimer Set new special variable refresh time to internal list. And variables to lists sorted with type.
     * @param cellId Identifier of cell.
     * @param specVarId Identifier of special variable in cell.
     * @param cellS Special variable for save.
     */
    void setNewTimer(int cellId, int specVarId, SpecialVariableAbstract<T>* cellS)
    {
        int type = cellS->getType();

        if(type == SpecialVariableAbstract<T>::TIME)
        {
            SpecialVariableTime<T>* temp = static_cast<SpecialVariableTime<T>* >(cellS);
            if(cellTimerTime.contains(temp->getRefreshTime()))
            {
                cellTimerTime.value(temp->getRefreshTime())->append(qMakePair(cellId, specVarId));
            }
            else
            {
                QList<QPair<int, int> >* newList = new QList<QPair<int, int> >();
                newList->append(qMakePair(cellId, specVarId));
                cellTimerTime.insert(temp->getRefreshTime(), newList);
            }
        }
        else if(type == SpecialVariableAbstract<T>::ACTION_PRESSED)
        {
            SpecialVariableActionPressed<T>* temp = static_cast<SpecialVariableActionPressed<T>* >(cellS);
            if(cellTimerActionPressed.contains(temp->getRefresh()))
            {
                cellTimerActionPressed.value(temp->getRefresh())->append(qMakePair(cellId,specVarId));
            }
            else
            {
                QList<QPair<int, int> >* newList = new QList<QPair<int, int> >();
                newList->append(qMakePair(cellId,specVarId));
                cellTimerActionPressed.insert(temp->getRefresh(), newList);
            }
        }
        else if(type == SpecialVariableAbstract<T>::ACTION)
        {
            SpecialVariableAction<T>* temp = static_cast<SpecialVariableAction<T>* >(cellS);

            if(cellAction.contains(temp->getButtonId()))
            {
                cellAction.value(temp->getButtonId())->append(qMakePair(cellId,specVarId));
            }
            else
            {
                QList<QPair<int,int> >* newList = new QList<QPair<int,int> >();
                newList->append(qMakePair(cellId, specVarId));
                cellAction.insert(temp->getButtonId(), newList);
            }
        }
    }

private:
    QString name;
    QList<QVariant> values;
    UniformTypes::UNIFORM_TYPES scalarType;
    UniformTypes::UNIFORM_TYPES uniformSize;
    UniformTypes::UNIFORM_TYPES uniformType;
    int rowCount;
    int columnCount;

    bool isMultiply;

    QList<UniformTypes::Attachment> attach;
    QList<UniformCell<int>*> cellsInt;
    QList<UniformCell<uint>*> cellsUInt;
    QList<UniformCell<float>*> cellsFloat;
    //QList<UniformCell<double>*> cellsDouble;
    QHash<long, QList<QPair<int,int> >* > cellTimerTime; // QHash<id of timer, QList<id in cell list> > can be setted more cell to one timer
    QHash<long, QList<QPair<int,int> >* > cellTimerActionPressed;
    QHash<int, QList<QPair<int,int> >* > cellAction;
};

// for serializing
QDataStream & operator<< (QDataStream& stream, const UniformVariable& uniform);
QDataStream & operator>> (QDataStream& stream, UniformVariable& uniform);

QDataStream & operator<< (QDataStream& stream, const UniformVariable* uniform);
QDataStream & operator>> (QDataStream& stream, UniformVariable*& uniform);

#endif // UNIFORMVARIABLE_H
