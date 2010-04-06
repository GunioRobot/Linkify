#pragma once


// Order is significant:
#include "core/stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include <COMMON/SDK/INC/service.h>
#include "core/ref.h"


// Private kernel functions.
extern "C" {
    DWORD GetEventData(HANDLE);
    BOOL SetEventData(HANDLE, DWORD);
}


namespace Wm {
    class Object {
    public:
        virtual ~Object() {
        }
    };
}
