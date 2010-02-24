#pragma once


#include "Length.h"
#include "Object.h"


namespace WM {
    class Size: public Object {
    private:
        Length _width;
        Length _height;


    public:
        Size(): _width(0), _height(0) {
        }


        Size(Length width, Length height): _width(width), _height(height) {
        }


        Length height() {
            return _height;
        }


        Length width() {
            return _width;
        }
    };
}
