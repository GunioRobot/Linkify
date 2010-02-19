#pragma once


#include "Exception.h"
#include "Widget.h"
#include "Window.h"


namespace WM {
    class TextBox: public Widget {
    public:
        TextBox(ref<String> text): Widget(text) {
        }


    protected:
        virtual void onAddTo(ref<Window> owner) {
            HWND handle = CreateWindow(
                TEXT("EDIT"),
                getText()->c_str(),
                WS_CHILD + WS_TABSTOP + WS_VISIBLE + WS_BORDER + ES_AUTOHSCROLL,
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
