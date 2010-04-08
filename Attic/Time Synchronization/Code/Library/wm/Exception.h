#pragma once


#include <cctype>
#include <exception>
#include "Object.h"
#include "String.h"


namespace wm {
    class Exception: public Object, public std::exception {
    public:
        static void throwError(DWORD code, HINSTANCE module = NULL) {
            DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM;
            TCHAR* buffer;

            if (module != NULL) {
                flags += FORMAT_MESSAGE_FROM_HMODULE;
            }
            
            DWORD length = FormatMessage(flags, module, code, 0,
                (LPTSTR) &buffer, 0, NULL);
            
            if (length == 0) {
                throw Exception(String::format(TEXT("Error code %Xh."), code));
            }
            for (size_t i = length - 1; (i > 0) && isspace(buffer[i]); --i) {
                buffer[i] = '\0';
            }

            String message(buffer);
            LocalFree(buffer);
            throw Exception(message);
        }


        static void throwLastError(HINSTANCE module = NULL) {
            throwError(GetLastError(), module);
        }


    private:
        String _message;
        char* _cache;


    public:
        Exception(String message = S("")): _message(message), _cache(NULL) {
        }


        virtual ~Exception() {
            if (_cache != NULL) {
                delete _cache;
            }
        }


        virtual void die() {
            UINT type = MB_OK + MB_ICONERROR;
            const TCHAR* message = getMessage().c_str();

            MessageBox(GetForegroundWindow(), message, NULL, type);
            ExitProcess(EXIT_FAILURE);
        }


        virtual String getMessage() {
            return _message;
        }


        virtual const char* what() {
            if (_cache == NULL) {
                _cache = getMessage().toCharArray();
            }

            return _cache;
        }
    };
}
