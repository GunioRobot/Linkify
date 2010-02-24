#pragma once


#include "Exception.h"
#include "Margin.h"
#include "Object.h"
#include "Position.h"
#include "Size.h"
#include "String.h"


namespace WM {
    class Widget: public Object {
    private:
        HWND _handle;
        ref<Widget> _parent;
        ref<String> _text;
        Margin _margin;
        Position _position;
        Size _size;


    public:
        Widget(ref<String> className, ref<String> text = S(""), DWORD style = 0):
            _parent(NULL), _text(text)
        {
            HWND parent = ((style & WS_CHILD) != 0) ? GetDesktopWindow() : NULL;

            _handle = CreateWindow(
                className->c_str(),
                text->c_str(),
                style,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                CW_USEDEFAULT,
                parent,
                NULL,
                NULL,
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
        
        
        virtual ref<Widget> getParent() {
            return _parent;
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
        
        
        virtual void onParentResize() {
            if (getParent() == NULL) {
                return;
            }

            Margin margin = getMargin();
            Position position = getPosition();
            Size size = getSize();
            Size area = getParent()->getSize();

            size_t areaWidth = area.width().value();
            size_t areaHeight = area.height().value();

            size_t left = position.left().value(areaWidth)
                + margin.left().value(areaWidth);
            size_t top = position.top().value(areaHeight)
                + margin.top().value(areaHeight);
            
            size_t width = size.width().value(areaWidth - left)
                - margin.right().value(areaWidth);
            size_t height = size.height().value(areaHeight - top)
                - margin.bottom().value(areaHeight);

            BOOL success = SetWindowPos(getHandle(), NULL,
                DRA::SCALEX(left), DRA::SCALEY(top),
                DRA::SCALEX(width), DRA::SCALEY(height),
                SWP_NOZORDER);

            if (!success) {
                Exception::throwLastError();
            }
        }


        virtual void setMargin(Margin margin) {
            _margin = margin;
            onParentResize();
        }


        virtual void setParent(ref<Widget> parent) {
            if (SetParent(getHandle(), parent->getHandle()) == NULL) {
                Exception::throwLastError();
            }
            
            ShowWindow(getHandle(), SW_SHOWNORMAL);
            _parent = parent;
        }


        virtual void setPosition(Position position) {
            _position = position;
            onParentResize();
        }


        virtual void setSize(Size size) {
            _size = size;
            onParentResize();
        }


        virtual void setText(ref<String> text) {
            if (!SetWindowText(getHandle(), text->c_str())) {
                Exception::throwLastError();
            }
            
            _text = text;
        }


    protected:
        virtual void changeStyle(DWORD style) {
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
    };
}
