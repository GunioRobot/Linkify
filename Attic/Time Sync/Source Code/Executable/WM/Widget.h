#pragma once


#include "Exception.h"
#include "Object.h"
#include "Size.h"
#include "String.h"


namespace WM {
    // TODO: Prevent adding a widget to multiple windows, and multiple times to
    // the same window?
    class Widget: public Object {
        friend class Window;

    private:
        static const DWORD DEFAULT_STYLE = WS_TABSTOP;


    private:
        HWND _handle;
        ref<String> _text;
        ref<Size> _size;
        long _left, _top;
        RECT _margin;


    public:
        Widget(ref<String> className, ref<String> text = S(""), DWORD style = 0):
            _text(text), _size(new Size(0, 0)), _left(0), _top(0)
        {
            _handle = CreateWindow(
                className->c_str(),
                text->c_str(),
                WS_CHILD + DEFAULT_STYLE + style,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                GetDesktopWindow(),
                NULL,
                GetModuleHandle(NULL),
                NULL);

            if (_handle == NULL) {
                Exception::throwLastError();
            }
            
            _margin.bottom = _margin.top = 0;
            _margin.left = _margin.right = 0;
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual long getLeft() {
            return _left;
        }


        virtual RECT getMargin() {
            return _margin;
        }


        virtual ref<Size> getSize() {
            return _size;
        }


        virtual ref<String> getText() {
            return _text;
        }


        virtual long getTop() {
            return _top;
        }


        virtual void setMargin(RECT margin) {
            _margin = margin;
        }


        virtual void setPosition(long left, long top) {
            _left = left;
            _top = top;
        }


        virtual void setSize(ref<Size> size) {
            _size = size;
        }


        virtual void setText(ref<String> text) {
            if (!SetWindowText(getHandle(), text->c_str())) {
                Exception::throwLastError();
            }
            
            _text = text;
        }


    protected:
        virtual void changeStyle(DWORD style) {
            style += DEFAULT_STYLE;

            // Distinguish between a previous value of zero and zero as an
            // indicator of an error.
            SetLastError(0);

            if (SetWindowLong(getHandle(), GWL_STYLE, style) == 0) {
                Exception::throwLastError();
            }

            // Update cached data.
            BOOL success = SetWindowPos(getHandle(), NULL, 0, 0, 0, 0,
                SWP_NOMOVE + SWP_NOSIZE + SWP_NOZORDER + SWP_FRAMECHANGED);
            
            if (!success) {
                Exception::throwLastError();
            }
        }
        
        
        virtual void onContainerResize(long areaWidth, long areaHeight) {
            RECT margin = getMargin();
            long left = DRA::SCALEX(getLeft() + margin.left);
            long top = DRA::SCALEY(getTop() + margin.top);
            long width = getSize()->getWidth();
            long height = getSize()->getHeight();

            if (width == Size::EXPANDABLE) {
                width = areaWidth - left - DRA::SCALEX(margin.right);
            }
            else {
                width = DRA::SCALEX(width - margin.right);
            }

            if (height == Size::EXPANDABLE) {
                height = areaHeight - top - DRA::SCALEY(margin.bottom);
            }
            else {
                height = DRA::SCALEY(height - margin.bottom);
            }

            BOOL success = SetWindowPos(getHandle(), NULL,
                left, top, width, height, SWP_NOZORDER);

            if (!success) {
                Exception::throwLastError();
            }
        }
    };
}
