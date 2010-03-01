#include "Exception.h"
#include "String.h"


namespace Wm {
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
