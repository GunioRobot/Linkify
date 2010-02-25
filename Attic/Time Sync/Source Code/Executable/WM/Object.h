#pragma once


// Order is significant:
#include "Core/stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "Core/ref.h"
#include "Core/rilWrapper.h"


namespace WM {
    class Object {
    public:
        virtual ~Object() {
        }
    };
}
