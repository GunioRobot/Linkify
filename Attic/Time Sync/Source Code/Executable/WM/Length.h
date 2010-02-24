#pragma once


#include "Object.h"
#include "Unit.h"


namespace WM {
    class Length: public Object {
    private:
        size_t _value;
        Unit _unit;


    public:
        Length(size_t value, Unit unit = Pixel): _value(value), _unit(unit) {
        }


        size_t compute(size_t total) {
            return _unit(_value, total);
        }
    };
}
