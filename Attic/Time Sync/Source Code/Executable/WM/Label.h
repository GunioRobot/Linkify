#pragma once


#include "Exception.h"
#include "Widget.h"
#include "Window.h"


namespace WM {
    class Label: public Widget {
    public:
        Label(ref<String> text): Widget(text) {
        }
    
    
    protected:
        virtual void onAddTo(ref<Window> owner) {
            HWND handle = CreateWindow(
                TEXT("STATIC"),
                getText()->c_str(),
                WS_CHILD + WS_TABSTOP + WS_VISIBLE,
                DRA::SCALEX(getLeft()),
                DRA::SCALEY(getTop()),
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
