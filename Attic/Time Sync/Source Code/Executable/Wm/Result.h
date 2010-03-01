#pragma once


#include "Event.h"
#include "Exception.h"


namespace Wm {
    class Result: public Event {
    private:
        BYTE* _value;
        size_t _size;


    public:
        Result(): _value(NULL), _size(0) {
        }


        virtual ~Result() {
            if (_value != NULL) {
                delete[] _value;
            }
        }


        virtual void* getRawValue() {
            wait();
            return _value;
        }


        template<typename T>
        T getValue() {
            if (sizeof(T) != getSize()) {
                throw Exception(S("Result type size mismatch."));
            }

            return *(T*) getRawValue();
        }

        
        virtual size_t getSize() {
            wait();
            return _size;
        }
        
        
        virtual void setRawValue(const void* value, size_t size) {
            _value = new BYTE[size];
            _size = size;

            memcpy(_value, value, _size);
            set();
        }
    };
}
