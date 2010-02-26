#pragma once


#include <string>
#include "Object.h"


#define S(string) \
    WM::String(TEXT(string))


namespace WM {
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
    };
}
