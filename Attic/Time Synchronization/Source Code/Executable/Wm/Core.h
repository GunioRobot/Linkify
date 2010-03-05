#pragma once


// Order is significant:
#include "Core/stdafx.h"
#include <commctrl.h>

// Order is not significant:
#include "Core/ref.h"


// Private kernel functions.
extern "C" {
    DWORD GetEventData(HANDLE);
    BOOL SetEventData(HANDLE, DWORD);
}
