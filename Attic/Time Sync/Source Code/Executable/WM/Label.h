#pragma once


#include "Widget.h"


namespace WM {
    class Label: public Widget {
    public:
        Label(ref<String> text): Widget(text, S("STATIC")) {
        }
    };
}
