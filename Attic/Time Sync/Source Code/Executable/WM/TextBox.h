#pragma once


#include "Exception.h"
#include "Widget.h"


namespace WM {
    class TextBox: public Widget {
    public:
        enum TextAlignment {
            ALIGN_CENTER = ES_CENTER,
            ALIGN_LEFT = ES_LEFT,
            ALIGN_RIGHT = ES_RIGHT,
        };


    private:
        static const long DEFAULT_STYLE = WS_BORDER + ES_AUTOHSCROLL
            + ES_MULTILINE;


    public:
        TextBox(TextAlignment textAlignment, ref<String> text = S("")):
            Widget(S("EDIT"), text, DEFAULT_STYLE + (DWORD) textAlignment)
        {
        }


        virtual bool isReadOnly() {
            LONG style = GetWindowLong(getHandle(), GWL_STYLE);

            if (style == 0) {
                Exception::throwLastError();
            }

            return (style & ES_READONLY) != 0;
        }


        virtual void setReadOnly(bool readOnly) {
            if (!SendMessage(getHandle(), EM_SETREADONLY, readOnly, 0)) {
                Exception::throwLastError();
            }
        }
    };
}
