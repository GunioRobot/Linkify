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
        DWORD _readWriteStyle;
        TextAlignment _textAlignment;


    public:
        TextBox(ref<String> text): Widget(text, S("EDIT")) {
            _readWriteStyle = 0;
            _textAlignment = ALIGN_LEFT;

            updateStyle(_readWriteStyle + (DWORD) _textAlignment);
        }


        virtual TextAlignment getTextAlignment() {
            return _textAlignment;
        }


        virtual bool isReadOnly() {
            return _readWriteStyle == ES_READONLY;
        }


        virtual void setReadOnly(bool readOnly) {
            if (!SendMessage(getHandle(), EM_SETREADONLY, readOnly, 0)) {
                Exception::throwLastError();
            }
            
            _readWriteStyle = readOnly ? ES_READONLY : 0;
        }


        virtual void setTextAlignment(TextAlignment textAlignment) {
            updateStyle(_readWriteStyle + (DWORD) textAlignment);
            _textAlignment = textAlignment;
        }


    protected:
        virtual void updateStyle(DWORD style) {
            Widget::updateStyle(WS_BORDER + ES_AUTOHSCROLL + ES_MULTILINE
                + style);
        }
    };
}
