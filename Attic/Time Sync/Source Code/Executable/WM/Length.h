#pragma once


#include "Object.h"
#include "Unit.h"


namespace WM {
    class Length: public Object {
    private:
        size_t _value;
        Unit _unit;


    public:
        Length(size_t value, Unit unit = NULL): _value(value), _unit(unit) {
        }


        size_t value() {
            return _value;
        }


        size_t value(size_t total) {
            return (_unit == NULL) ? _value : _unit(_value, total);
        }
    };
}
