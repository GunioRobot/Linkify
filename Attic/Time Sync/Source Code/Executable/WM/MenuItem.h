#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    class MenuItem: public Object {
    private:
        // Reserved for controls.
        static const UINT_PTR _RESERVED_ID = 0;


    private:
        String _caption;
        UINT_PTR _id;


    public:
        MenuItem(String caption): _caption(caption), _id(_RESERVED_ID) {
        }


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


        virtual UINT getType() {
            return MF_STRING;
        }
    };
}
