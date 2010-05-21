#pragma once


#include "Exception.h"
#include "Object.h"
#include "Unit.h"


namespace wm {
    class Length: public Object {
    private:
        size_t _value;
        Unit _unit;


    public:
        Length(size_t value = 0, Unit unit = NULL): _value(value), _unit(unit) {
        }


        virtual int operator >(Length length) {
            if (_unit != length._unit) {
                throw Exception(
                    S("Can't compare lengths with different units."));
            }

            return value() > length.value();
        }


        virtual Length operator +(Length length) {
            if (_unit != length._unit) {
                throw Exception(S("Can't add lengths with different units."));
            }

            return Length(_value + length._value, _unit);
        }


        virtual size_t value() {
            return _value;
        }


        virtual size_t value(size_t total) {
            return (_unit == NULL) ? _value : _unit(_value, total);
        }
    };
}
