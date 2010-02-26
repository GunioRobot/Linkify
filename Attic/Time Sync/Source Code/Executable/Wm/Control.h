#pragma once


#include "Widget.h"


namespace WM {
    class Control: public Widget {
    public:
        Control(String className, String text = S(""), DWORD style = 0):
            Widget(className, text, WS_CHILD + WS_TABSTOP + style)
        {
        }
    };
}
