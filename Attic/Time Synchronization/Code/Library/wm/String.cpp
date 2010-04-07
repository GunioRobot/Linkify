#include <cstdarg>
#include <new>
#include "Exception.h"
#include "String.h"


namespace wm {
    String String::format(const TCHAR* spec, ...) {
        size_t length = BUFSIZ;
        TCHAR* buffer = NULL;
        int count = -1;

        for (; count < 0; length *= 2) {
            TCHAR* tmp = (TCHAR*) realloc(buffer, (length + 1) * sizeof(TCHAR));

            if (tmp == NULL) {
                free(buffer);
                throw std::bad_alloc("Out of memory.");
            }

            buffer = tmp;
            va_list args;
            
            va_start(args, spec);
            count = _vsntprintf(buffer, length, spec, args);
            va_end(args);
        }

        buffer[count] = '\0';
        String string(buffer);
        free(buffer);

        return string;
    }


#ifdef UNICODE
    String::String(const char* string) {
        size_t length = strlen(string);
        
        wchar_t* buffer = new wchar_t[length + 1];
        buffer[0] = '\0';

        int result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
            string, -1, buffer, length + 1);

        assign(buffer);
        delete[] buffer;

        if (result == 0) {
            Exception::throwLastError();
        }
    }
#endif


    char* String::toCharArray() {
        char* buffer = new char[length() + 1];

#ifdef UNICODE
        int result = WideCharToMultiByte(CP_UTF8, 0, c_str(), -1, buffer,
            (length() + 1) * sizeof(char), NULL, NULL);

        if (result == 0) {
            delete[] buffer;
            Exception::throwLastError();
        }
#else
        strcpy(buffer, c_str());
#endif
        
        return buffer;
    }
}
