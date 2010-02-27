#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Result: public Object {
    private:
        HANDLE _ready;
        BYTE* _value;
        size_t _size;


    public:
        Result(): _value(NULL), _size(0) {
            _ready = CreateEvent(NULL, true, false, NULL);

            if (_ready == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Result() {
            if (_value != NULL) {
                delete[] _value;
            }
            if (!CloseHandle(_ready)) {
                Exception::throwLastError();
            }
        }


        virtual void* getRawValue() {
            waitReady();
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
            waitReady();
            return _size;
        }
        
        
        virtual void setRawValue(const void* value, size_t size) {
            _value = new BYTE[size];
            _size = size;

            memcpy(_value, value, _size);
            
            if (!SetEvent(_ready)) {
                Exception::throwLastError();
            }
        }


    private:
        void waitReady() {
            DWORD result = WaitForSingleObject(_ready, INFINITE);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }
        }
    };
}
