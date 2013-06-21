#ifndef SPECIALVARIABLEACTIONPRESSED_H
#define SPECIALVARIABLEACTIONPRESSED_H

#include "specialvariableabstract.h"

/**
 * Class for working with special variable, created in UniformCell where we use these to get result number.
 * This class will react on when user press button, then increment value trueValue to result number or
 * if button is not press increment falseValue. Increment is used with + operator every incTime.
 */
template <class T>
class SpecialVariableActionPressed : public SpecialVariableAbstract<T>
{
public:

    /**
     * @brief SpecialVariableActionPressed Create special variable
     * $ActionPressed{ButtonId, Refresh time, pressed increment, released increment, default = 0}
     * If user press button then increment base value with pressed trueValue else with falseValue.
     * Increment every incTime time.
     * @param buttonId Identifier of button.
     * @param incTime Time interval when increment.
     * @param trueValue When button pressed value.
     * @param falseValue When button released value.
     * @param defaultValue Default value, when we reset actual number.
     */
    SpecialVariableActionPressed(int buttonId, double incTime, T trueValue, T falseValue, T defaultValue = 0) :
        btnId(buttonId),
        isBtnPressed(false),
        trueInc(trueValue),
        falseInc(falseValue),
        time(incTime)
    {
        this->value = defaultValue;
        this->type = SpecialVariableAbstract<T>::ACTION_PRESSED;
        this->defaultNumber = defaultValue;
    }

    /**
     * @brief getButtonId Get identifier of button.
     * @return Button identifier.
     */
    int getButtonId() const {return btnId;}

    /**
     * @brief getRefresh Get refresh interval.
     * @return Refresh interval.
     */
    double getRefresh() const {return time;}

    /**
     * @brief isPressed Get if our button is pressed now.
     * @return Return true if our button is pressed, false otherwise.
     */
    bool isPressed() const {return isBtnPressed;}

    /**
     * @brief setPressed Set pressed when our button is pressed.
     * @param pressed True if button is pressed, false otherwise.
     */
    void setPressed(bool pressed)
    {
        isBtnPressed = pressed;
    }

    /**
     * @brief incValue Increment value + trueValue if button is pressed, + falseValue if released.
     */
    void incValue()
    {
        if(isBtnPressed)
        {
            this->value += trueInc;
        }
        else
        {
            this->value += falseInc;
        }
    }

    /**
     * @brief setDefault Set default value.
     */
    void setDefault()
    {
        this->value = this->defaultNumber;
    }

private:
    int btnId;
    bool isBtnPressed;
    T trueInc;
    T falseInc;
    double time;

};

#endif // SPECIALVARIABLEACTIONPRESSED_H
