#pragma once


#include "Exception.h"
#include "Widget.h"
#include "Window.h"


namespace WM {
    // TODO: Simulate a single-line text box with text alignment, by using the
    // multi-line style.
    class TextBox: public Widget {
    public:
        enum TextAlignment {
            ALIGN_CENTER = ES_CENTER,
            ALIGN_LEFT = ES_LEFT,
            ALIGN_RIGHT = ES_RIGHT,
        };


    private:
        DWORD _readWriteStyle;
        TextAlignment _textAlignment;


    public:
        TextBox(ref<String> text):
            Widget(text), _readWriteStyle(0), _textAlignment(ALIGN_LEFT)
        {
        }


        virtual void setReadOnly(bool readOnly) {
            _readWriteStyle = readOnly ? ES_READONLY : 0;
        }


        virtual void setTextAlignment(TextAlignment textAlignment) {
            _textAlignment = textAlignment;
        }


    protected:
        virtual void onAddTo(ref<Window> owner) {
            DWORD style = WS_BORDER + ES_AUTOHSCROLL + ES_MULTILINE
                + _readWriteStyle + (DWORD) _textAlignment;
            
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
