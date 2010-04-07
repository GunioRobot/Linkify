#pragma once


#include "Exception.h"
#include "Margin.h"
#include "Object.h"
#include "Position.h"
#include "Size.h"
#include "String.h"


namespace wm {
    class Widget: public Object {
    private:
        HWND _handle;
        ref<Widget> _parent;
        String _text;
        Margin _margin;
        Position _position;
        Size _size;


    public:
        Widget(String className, String text = S(""), DWORD style = 0):
            _parent(NULL), _text(text)
        {
            HWND parent = ((style & WS_CHILD) != 0) ? GetDesktopWindow() : NULL;

            _handle = CreateWindow(
                className.c_str(),
                text.c_str(),
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


        virtual String getText() {
            return _text;
        }
        
        
        virtual HWND getWidgetHandle() {
            return _handle;
        }
        
        
        virtual void onResize() {
            if (getParent().null()) {
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

            BOOL success = SetWindowPos(getWidgetHandle(), NULL,
                DRA::SCALEX(left), DRA::SCALEY(top),
                DRA::SCALEX(width), DRA::SCALEY(height),
                SWP_NOZORDER);

            if (!success) {
                Exception::throwLastError();
            }
        }


        virtual void setMargin(Margin margin) {
            _margin = margin;
            onResize();
        }


        virtual void setParent(ref<Widget> p) {
            if (SetParent(getWidgetHandle(), p->getWidgetHandle()) == NULL) {
                Exception::throwLastError();
            }
            
            ShowWindow(getWidgetHandle(), SW_SHOWNORMAL);
            _parent = p;
            onResize();
        }


        virtual void setPosition(Position position) {
            _position = position;
            onResize();
        }


        virtual void setSize(Size size) {
            _size = size;
            onResize();
        }


        virtual void setText(String text) {
            if (!SetWindowText(getWidgetHandle(), text.c_str())) {
                Exception::throwLastError();
            }
            
            _text = text;
        }


    protected:
        virtual void changeStyle(DWORD style) {
            SetLastError(ERROR_SUCCESS);
            LONG value = SetWindowLong(getWidgetHandle(), GWL_STYLE, style);

            if ((value == 0) && (GetLastError() != ERROR_SUCCESS)) {
                Exception::throwLastError();
            }

            // Update cached data.
            BOOL success = SetWindowPos(getWidgetHandle(), NULL, 0, 0, 0, 0,
                SWP_NOMOVE + SWP_NOSIZE + SWP_NOZORDER + SWP_FRAMECHANGED);
            
            if (!success) {
                Exception::throwLastError();
            }
        }
    };
}
