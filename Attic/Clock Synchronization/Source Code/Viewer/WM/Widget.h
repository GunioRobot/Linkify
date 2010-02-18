#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    // TODO: Prevent adding a widget to multiple windows, and multiple times to
    // the same window?
    class Widget: public Object {
        friend class Window;

    private:
        HWND _handle;
        ref<String> _text;
        size_t _width;
        size_t _height;


    public:
        Widget(ref<String> text, size_t width, size_t height):
            _handle(NULL), _text(text), _width(width), _height(height)
        {
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual size_t getHeight() {
            return _height;
        }


        virtual ref<String> getText() {
            return _text;
        }


        virtual size_t getWidth() {
            return _width;
        }


    protected:
        virtual void onAddTo(ref<Window> owner, size_t left, size_t top) = 0;


        virtual void setHandle(HWND handle) {
            _handle = handle;
        }
    };
}
