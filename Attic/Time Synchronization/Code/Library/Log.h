#pragma once


#include <cstdarg>
#include "Object.h"
#include "String.h"


namespace Wm {
    class Log: public Object {
    private:
        static const size_t _MAXIMUM_LENGTH = 128;


    public:
        static Log error;


    private:
        Log() {
        }
        
        
    public:
        virtual void operator ()(const TCHAR* format, ...) {
            TCHAR message[_MAXIMUM_LENGTH];
            va_list arguments;

            va_start(arguments, format);
            _vsntprintf(message, _MAXIMUM_LENGTH, format, arguments);
            va_end(arguments);

            message[_MAXIMUM_LENGTH - 1] = '\0';
            DEBUGMSG(true, (TEXT("%s"), message));
        }


        virtual Log& operator <<(String string) {
            DEBUGMSG(true, (TEXT("%s"), string.c_str()));
            return *this;
        }
    };
}
