#pragma once


#include "Exception.h"
#include "Object.h"


namespace WM {
    class Ril: public Object {
    private:
        static size_t _references;


    private:
        HRIL _handle;


    public:
        Ril(): _handle(NULL) {
            if ((_references == 0) && !RIL_Load()) {
                Exception::throwLastError();
            }
            
            ++_references;
        }


        virtual ~Ril() {
            if ((--_references == 0) && !RIL_Unload()) {
                Exception::throwLastError();
            }
        }
    };
}
