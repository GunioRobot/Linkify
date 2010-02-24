#pragma once


#include "Control.h"


namespace WM {
    class Label: public Control {
    public:
        Label(ref<String> text): Control(S("STATIC"), text) {
        }
    };
}
