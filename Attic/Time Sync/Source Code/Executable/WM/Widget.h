#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    // TODO: Prevent adding a widget to multiple windows, and multiple times to
    // the same window?
    //
    // TODO: Include remaining margins.
    class Widget: public Object {
        friend class Window;

    public:
        enum {
            EXPANDABLE = -1,
        };


    private:
        HWND _handle;
        ref<String> _text;
        long _width, _height;
        long _left, _top;
        long _marginRight;


    public:
        Widget(ref<String> text): _text(text) {
            _handle = NULL;
            _width = _height = 0;
            _left = _top = 0;
            _marginRight = 0;
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual long getHeight() {
            return _height;
        }


        virtual long getLeft() {
            return _left;
        }


        virtual ref<String> getText() {
            return _text;
        }


        virtual long getTop() {
            return _top;
        }


        virtual long getWidth() {
            return _width;
        }


        virtual void setMarginRight(long marginRight) {
            _marginRight = marginRight;
        }


        virtual void setPosition(long left, long top) {
            _left = left;
            _top = top;
        }


        virtual void setSize(long width, long height) {
            _width = width;
            _height = height;
        }


        virtual void setSize(ref<Widget> widget) {
            _width = widget->getWidth();
            _height = widget->getHeight();
        }


    protected:
        virtual void onAddTo(ref<Window> owner) = 0;


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
                width - window.left - DRA::SCALEX(_marginRight),
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
