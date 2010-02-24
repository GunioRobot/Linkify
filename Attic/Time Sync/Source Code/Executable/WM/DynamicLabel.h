#pragma once


#include "Label.h"


namespace WM {
    class DynamicLabel: public Label {
    private:
        String _shortText;
        String _longText;


    public:
        DynamicLabel(String shortText, String longText):
            Label(S("-")), _shortText(shortText), _longText(longText)
        {
        }


        virtual void onParentResize() {
            String& intended = getIntendedText();
            
            if (getText() == intended) {
                Label::onParentResize();
            }
            else {
                setText(intended);
            }

        }


    private:
        String& getIntendedText() {
            if (DRA::GetDisplayMode() == DRA::Landscape) {
                return _longText;
            }
            else {
                return _shortText;
            }
        }
    };
}
