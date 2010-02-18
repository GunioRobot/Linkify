#pragma once


#include "Object.h"
#include "String.h"


namespace WM {
    class MenuItem: public Object {
        friend class Menu;

    private:
        ref<String> _caption;
        bool _hasId;
        UINT_PTR _id;


    public:
        MenuItem(ref<String> caption): _caption(caption), _hasId(false) {
        }


        virtual ref<String> getCaption() {
            return _caption;
        }


    protected:
        virtual UINT_PTR getId() {
            // Zero is reserved for controls.
            static UINT_PTR counter = 1;

            if (!_hasId) {
                _id = counter++;
                _hasId = true;
            }

            return _id;
        }


        virtual UINT getType() {
            return MF_STRING;
        }
    };
}
