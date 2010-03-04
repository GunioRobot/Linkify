#pragma once


#include <string>
#include "Object.h"


#define S(string) \
    (::Wm::String(TEXT(string)))


namespace Wm {
    class String: public std::basic_string<TCHAR> {
    public:
        String(): std::basic_string<TCHAR>(TEXT("")) {
        }


        String(std::basic_string<TCHAR> string):
            std::basic_string<TCHAR>(string)
        {
        }


        String(const TCHAR* string):
            std::basic_string<TCHAR>(string)
        {
        }


#ifdef UNICODE
        String(const char* string);
#endif
        
        
        virtual char* toCharArray();
    };
}
