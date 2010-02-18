#pragma once


#include <string>
#include "Object.h"


#define S(string) WM::ToString(TEXT(string))


namespace WM {
    typedef std::basic_string<TCHAR> String;
    ref<String> ToString(const TCHAR* string);
}
