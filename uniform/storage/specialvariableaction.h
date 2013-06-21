#ifndef SPECIALVARIABLEACTION_H
#define SPECIALVARIABLEACTION_H

#include "specialvariableabstract.h"

/**
 * Uniform special variable for reaction on button, toggle first and second value.
 */
template <class T>
class SpecialVariableAction : public SpecialVariableAbstract<T>
{
public:
    /**
     * @brief SpecialVariableAction Create new uniform special variable of type $Action{ButtonId, first, second}.
     * @param buttonId Identifier of button under OpenGL window.
     * @param firstValue First and default value.
     * @param secondValue Second value for toggle.
     */
    SpecialVariableAction(int buttonId, T firstValue, T secondValue) :
        buttonId(buttonId),
        firstValue(firstValue),
        secondValue(secondValue),
        isFirst(true)
    {
        this->value = firstValue;
        this->type = SpecialVariableAbstract<T>::ACTION;
        this->defaultNumber = firstValue;
    }

    /**
     * @brief toggle If first value is set, set second value, or otherwise.
     */
    void toggle()
    {
        if(isFirst)
        {
            this->isFirst = false;
            this->value = this->secondValue;
        }
        else
        {
            this->isFirst = true;
            this->value = this->firstValue;
        }
    }

    /**
     * @brief setDefault Set first value to result.
     */
    void setDefault()
    {
        this->value = this->defaultNumber;
    }

    /**
     * @brief getButtonId Return button identifier.
     * @return Button identifier number.
     */
    int getButtonId() {return buttonId;}

private:
    int buttonId;
    T firstValue;
    T secondValue;
    bool isFirst;

};

#endif // SPECIALVARIABLEACTION_H
