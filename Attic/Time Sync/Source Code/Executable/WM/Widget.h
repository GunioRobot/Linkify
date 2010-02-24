#pragma once


#include "Exception.h"
#include "Margin.h"
#include "Object.h"
#include "Position.h"
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
        Margin _margin;
        Position _position;
        Size _size;


    public:
        Widget(ref<String> className, ref<String> text = S(""), DWORD style = 0):
            _text(text)
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
        }


        virtual HWND getHandle() {
            return _handle;
        }


        virtual Margin getMargin() {
            return _margin;
        }


        virtual Position getPosition() {
            return _position;
        }


        virtual Size getSize() {
            return _size;
        }


        virtual ref<String> getText() {
            return _text;
        }


        virtual void setMargin(Margin margin) {
            _margin = margin;
        }


        virtual void setPosition(Position position) {
            _position = position;
        }


        virtual void setSize(Size size) {
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
        
        
        virtual void onOwnerResize(size_t totalWidth, size_t totalHeight) {
            Margin margin = getMargin();
            Position position = getPosition();
            Size size = getSize();

            size_t left = position.left().compute(totalWidth) + margin.left().compute(totalWidth);
            size_t top = position.top().compute(totalHeight) + margin.top().compute(totalHeight);
            
            size_t width = size.width().compute(totalWidth - left) - margin.right().compute(totalWidth);
            size_t height = size.height().compute(totalHeight - top) - margin.bottom().compute(totalHeight);

            BOOL success = SetWindowPos(getHandle(), NULL,
                DRA::SCALEX(left), DRA::SCALEY(top),
                DRA::SCALEX(width), DRA::SCALEY(height),
                SWP_NOZORDER);

            if (!success) {
                Exception::throwLastError();
            }
        }
    };
}
