#pragma once


#include <exception>
#include "Object.h"
#include "String.h"


namespace WM {
    class Exception: public Object, public std::exception {
    public:
        static void throwLastError() {
            DWORD code = GetLastError();
            TCHAR* buffer;
            
            DWORD length = FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                code,
                0,
                (LPTSTR) &buffer,
                0,
                NULL);

            ref<String> message = (length == 0) ? NULL : new String(buffer);
            LocalFree(buffer);
            
            throw Exception(message);
        }


    private:
        ref<String> _message;


    public:
        Exception(ref<String> message): _message(message) {
        }


        virtual ref<String> getMessage() {
            return _message;
        }
    };
}
