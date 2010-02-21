#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    // TODO: Prevent adding a widget to multiple windows, and multiple times to
    // the same window?
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
        RECT _margin;


    public:
        Widget(ref<String> text): _text(text) {
            _handle = NULL;
            
            _width = _height = 0;
            _left = _top = 0;
            
            _margin.bottom = _margin.top = 0;
            _margin.left = _margin.right = 0;
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


        virtual RECT getMargin() {
            return _margin;
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


        virtual void setMargin(RECT margin) {
            _margin = margin;
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


        virtual void setText(ref<String> text) {
            if ((getHandle() != NULL)
                && !SetWindowText(getHandle(), text->c_str()))
            {
                Exception::throwLastError();
            }
            
            _text = text;
        }


    protected:
        virtual void onAddTo(ref<Window> owner) = 0;


        virtual void onContainerResize(long areaWidth, long areaHeight) {
            RECT margin = getMargin();
            long left = DRA::SCALEX(getLeft() + margin.left);
            long top = DRA::SCALEY(getTop() + margin.top);
            long width = getWidth();
            long height = getHeight();

            if (width == EXPANDABLE) {
                width = areaWidth - left - DRA::SCALEX(margin.right);
            }
            else {
                width = DRA::SCALEX(width - margin.right);
            }

            if (height == EXPANDABLE) {
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


        virtual void setHandle(HWND handle) {
            _handle = handle;
        }
    };
}
