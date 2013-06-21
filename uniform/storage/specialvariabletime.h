#ifndef SPECIALVARIABLETIME_H
#define SPECIALVARIABLETIME_H

#include "specialvariableabstract.h"
#include <cmath>

// variable $Time{increment, čas v ms, default = 0, max = 0}

template <class T>
class SpecialVariableTime : public  SpecialVariableAbstract<T>
{
public:
    SpecialVariableTime(T increment, double refresh, T defaultValue = 0, T max = 0) :
        increment(increment),
        time(refresh),
        maxNumber(max)
    {
        this->defaultNumber = defaultValue;
        this->value = defaultValue;
        this->type = SpecialVariableAbstract<T>::TIME;
    }

    double getRefreshTime() const {return time;}

    void incValue() {
        this->value += increment;

        if(maxNumber != 0) {
            if(abs(this->value) > abs(maxNumber))
                setDefault();
        }
    }

    void setDefault() {this->value = this->defaultNumber;}

private:
    T increment;
    double time;
    T maxNumber;
};

#endif // SPECIALVARIABLETIME_H
