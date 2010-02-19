#pragma once


// Order is significant:
#include "stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "ref.h"


namespace WM {
    class Object {
    public:
        virtual ~Object() {
        }
    };
}
