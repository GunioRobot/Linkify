#pragma once


#include "Exception.h"
#include "Widget.h"
#include "Window.h"


namespace WM {
    class TextBox: public Widget {
    private:
        bool _readOnly;


    public:
        TextBox(ref<String> text): Widget(text), _readOnly(false) {
        }


        virtual bool isReadOnly() {
            return _readOnly;
        }


        virtual void setReadOnly(bool readOnly) {
            _readOnly = readOnly;
        }


    protected:
        virtual void onAddTo(ref<Window> owner) {
            DWORD style = WS_BORDER + ES_AUTOHSCROLL;
            
            if (isReadOnly()) {
                style += ES_READONLY;
            }

            HWND handle = CreateWindow(
                TEXT("EDIT"),
                getText()->c_str(),
                WS_CHILD + WS_TABSTOP + WS_VISIBLE + style,
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
