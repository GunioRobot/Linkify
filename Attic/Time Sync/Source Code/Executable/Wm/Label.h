#pragma once


#include "Exception.h"
#include "Control.h"


namespace Wm {
    class Label: public Control {
    private:
        bool _fitToWidth;


    public:
        Label(String text = S("")):
            Control(S("STATIC"), text), _fitToWidth(false)
        {
        }


        virtual Size getSize() {
            if (!isFitToWidth()) {
                return Wm::Control::getSize();
            }

            return Size(getTextSize().width(), Control::getSize().height());
        }


        virtual bool isFitToWidth() {
            return _fitToWidth;
        }


        virtual void setFitToWidth(bool fitToWidth) {
            _fitToWidth = fitToWidth;
            
            if (isFitToWidth()) {
                onResize();
            }
        }


        virtual void setText(String text) {
            Control::setText(text);

            if (isFitToWidth()) {
                onResize();
            }
        }


    private:
        Size getTextSize() {
            HWND window = (getParent() == NULL) ? NULL : getParent()->getHandle();
            HDC display = GetDC(window);
            SIZE text;

            if (display == NULL) {
                Exception::throwLastError();
            }

            BOOL success = GetTextExtentPoint(display,
                getText().c_str(),
                getText().length(),
                &text);

            ReleaseDC(window, display);

            if (!success) {
                Exception::throwLastError();
            }

            return Size(DRA::UNSCALEX(text.cx) + DRA::SCALEX(2), DRA::UNSCALEY(text.cy));
        }
    };
}
