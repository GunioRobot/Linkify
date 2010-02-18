#pragma once


#include <string>
#include "Object.h"


#define S(string) \
    ref<WM::String>(new WM::String(TEXT(string)))


namespace WM {
    class String: public std::basic_string<TCHAR> {
    public:
        String(const TCHAR* string): std::basic_string<TCHAR>(string) {
        }
    };
}
