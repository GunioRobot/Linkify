#pragma once


#include "Exception.h"
#include "Object.h"
#include "String.h"


namespace Wm {
    class MenuItem: public Object {
    private:
        // Reserved for controls.
        static const UINT_PTR _RESERVED_ID = 0;


    private:
        String _caption;
        UINT_PTR _id;
        HWND _owner;


    public:
        MenuItem(String caption):
            _caption(caption), _id(_RESERVED_ID), _owner(NULL)
        {
        }


        // Must be a reference for the API functions to work, otherwise the
        // internal string data can become invalid since it's stack allocated.
        virtual String& getCaption() {
            return _caption;
        }


        virtual UINT_PTR getId() {
            static UINT_PTR counter = _RESERVED_ID;

            if (_id == _RESERVED_ID) {
                _id = ++counter;
            }

            return _id;
        }


        virtual HWND getOwner() {
            return _owner;
        }


        virtual UINT getType() {
            return MF_STRING;
        }


        virtual void setCaption(String caption) {
            if (getOwner() != NULL) {
                TBBUTTONINFO info;

                info.cbSize = sizeof(TBBUTTONINFO);
                info.dwMask = TBIF_TEXT;
                info.pszText = (LPWSTR) caption.c_str();

                BOOL success = SendMessage(getOwner(), TB_SETBUTTONINFO,
                    getId(), (LPARAM) &info);

                if (!success) {
                    Exception::throwLastError();
                }
            }

            _caption = caption;
        }


        virtual void setOwner(HWND owner) {
            _owner = owner;
        }
    };
}
