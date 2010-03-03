#pragma once


#include <cctype>
#include <exception>
#include "Object.h"
#include "String.h"
#include "StringStream.h"


namespace Wm {
    class Exception: public Object, public std::exception {
    public:
        static void throwLastError(HINSTANCE module = NULL) {
            DWORD code = GetLastError();
            DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM;
            TCHAR* buffer;

            if (module != NULL) {
                flags += FORMAT_MESSAGE_FROM_HMODULE;
            }
            
            DWORD length = FormatMessage(flags, module, code, 0,
                (LPTSTR) &buffer, 0, NULL);
            
            if (length == 0) {
                StringStream message;
                
                message << TEXT("Error code ") << std::hex << code << TEXT("h.");
                throw Exception(message.str());
            }
            else {
                for (size_t i = length - 1; (i > 0) && isspace(buffer[i]); --i) {
                    buffer[i] = '\0';
                }

                String message(buffer);
                LocalFree(buffer);
                throw Exception(message);
            }
        }


    private:
        String _message;


    public:
        Exception(String message = S("")): _message(message) {
        }


        virtual String getMessage() {
            return _message;
        }
    };
}
