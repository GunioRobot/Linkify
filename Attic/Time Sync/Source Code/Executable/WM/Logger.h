#pragma once


#include <cstdarg>
#include "Object.h"


namespace WM {
    class Logger: public Object {
    private:
        static const size_t _MAXIMUM_LENGTH = 128;


        static void print(const TCHAR* format, va_list arguments) {
            TCHAR message[_MAXIMUM_LENGTH];

            _vsntprintf(message, _MAXIMUM_LENGTH, format, arguments);
            message[_MAXIMUM_LENGTH] = '\0';
            DEBUGMSG(true, (TEXT("%s"), message));
        }


    public:
        static void debug(const TCHAR* format, ...) {
            va_list arguments;

            va_start(arguments, format);
            print(format, arguments);
            va_end(arguments);
        }


        static void error(const TCHAR* format, ...) {
            va_list arguments;

            DEBUGMSG(true, (TEXT("Error: ")));
            va_start(arguments, format);
            print(format, arguments);
            va_end(arguments);
        }


    private:
        Logger() {
        }
    };
}
