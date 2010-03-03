#pragma once


#include "Label.h"


namespace Wm {
    class DynamicLabel: public Label {
    private:
        String _shortText;
        String _longText;


    public:
        DynamicLabel(String shortText, String longText):
            Label(), _shortText(shortText), _longText(longText)
        {
            setText(getIntendedText());
        }


        virtual void onResize() {
            String& intended = getIntendedText();
            
            if (getText() == intended) {
                Label::onResize();
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
