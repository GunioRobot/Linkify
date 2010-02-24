#pragma once


#include "Exception.h"
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


        size_t get(size_t total) {
            return (_unit == NULL) ? _value : _unit(_value, total);
        }


        size_t value() {
            if (_unit != NULL) {
                throw Exception(S("Length value is abstract."));
            }

            return _value;
        }
    };
}
