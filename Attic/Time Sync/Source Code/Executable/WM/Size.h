#pragma once


#include "Object.h"


namespace WM {
    class Size: public Object {
    public:
        enum {
            EXPANDABLE = -1,
        };


    private:
        long _width;
        long _height;


    public:
        Size(long width, long height): _width(width), _height(height) {
        }


        virtual long getHeight() {
            return _height;
        }


        virtual long getWidth() {
            return _width;
        }
    };
}
