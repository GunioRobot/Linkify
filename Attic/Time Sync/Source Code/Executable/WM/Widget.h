#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    // TODO: Prevent adding a widget to multiple windows, and multiple times to
    // the same window?
    //
    // TODO: Include all possible margins.
    class Widget: public Object {
        friend class Window;

    public:
        enum {
            EXPANDABLE = -1,
        };


    private:
        HWND _handle;
        ref<String> _text;
        long _width;
        long _height;
        long _rightMargin;


    public:
        Widget(ref<String> text, long width, long height):
            _handle(NULL), _text(text), _width(width), _height(height),
            _rightMargin(0)
        {
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual long getHeight() {
            return _height;
        }


        virtual ref<String> getText() {
            return _text;
        }


        virtual long getWidth() {
            return _width;
        }


        virtual void setRightMargin(long rightMargin) {
            _rightMargin = rightMargin;
        }


    protected:
        virtual void onAddTo(ref<Window> owner, size_t left, size_t top) = 0;


        virtual void onLayoutResize(long width, long height) {
            if (getWidth() != EXPANDABLE) {
                return;
            }

            RECT window;
            
            if (!GetWindowRect(getHandle(), &window)) {
                Exception::throwLastError();
            }

            BOOL success = SetWindowPos(
                getHandle(),
                NULL,
                0,
                0,
                width - window.left - DRA::SCALEX(_rightMargin),
                DRA::SCALEY(getHeight()),
                SWP_NOMOVE + SWP_NOZORDER);

            if (!success) {
                Exception::throwLastError();
            }
        }


        virtual void setHandle(HWND handle) {
            _handle = handle;
        }
    };
}
