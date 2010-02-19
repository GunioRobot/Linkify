#pragma once


#include "Exception.h"
#include "Widget.h"
#include "Window.h"


namespace WM {
    class Label: public Widget {
    public:
        Label(ref<String> text, long width, long height):
            Widget(text, width, height)
        {
        }
    
    
    protected:
        virtual void onAddTo(ref<Window> owner, size_t left, size_t top) {
            HWND handle = CreateWindow(
                TEXT("STATIC"),
                getText()->c_str(),
                WS_CHILD + WS_TABSTOP + WS_VISIBLE,
                DRA::SCALEX(left),
                DRA::SCALEY(top),
                DRA::SCALEX(getWidth()),
                DRA::SCALEY(getHeight()),
                owner->getHandle(),
                NULL,
                GetModuleHandle(NULL), 
                NULL);

            if (handle == NULL) {
                Exception::throwLastError();
            }

            setHandle(handle);
        }
    };
}
