#ifndef SPECIALVARIABLE_H
#define SPECIALVARIABLE_H

/**
 * Abastract class as parent of uniform special variables.
 */
template <class T>
class SpecialVariableAbstract
{
public:
    enum TYPE {TIME = 0, ACTION, ACTION_PRESSED, NONE};

    /**
     * @brief SpecialVariableAbstract Create abstract special variable. Set type NONE and defaultNumber to 0.
     */
    SpecialVariableAbstract() :
        defaultNumber(0),
        type(NONE)
    {
    }

    /**
     * @brief ~SpecialVariableAbstract Destructor, free used memory.
     */
    virtual ~SpecialVariableAbstract() {}

    /**
     * @brief getValue Get actual calculated value.
     * @return Actual calculated value.
     */
    virtual T getValue() const {return value;}

    /**
     * @brief getType Get type of these special variable.
     * @return Type of this variable.
     */
    TYPE getType() const {return type;}

protected:
    T defaultNumber;
    T value;
    TYPE type;
};

#endif // SPECIALVARIABLE_H
