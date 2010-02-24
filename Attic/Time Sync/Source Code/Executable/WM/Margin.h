#pragma once


#include "Length.h"
#include "Object.h"


namespace WM {
    class Margin: public Object {
    private:
        Length _left;
        Length _top;
        Length _right;
        Length _bottom;


    public:
        Margin(): _left(0), _top(0), _right(0), _bottom(0) {
        }
        
        
        Margin(Length left, Length top, Length right, Length bottom):
            _left(left), _top(top), _right(right), _bottom(bottom)
        {
        }


        Length bottom() {
            return _bottom;
        }


        Length left() {
            return _left;
        }


        Length right() {
            return _right;
        }


        Length top() {
            return _top;
        }
    };
}
