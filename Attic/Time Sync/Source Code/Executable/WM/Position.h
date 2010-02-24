#pragma once


#include "Length.h"
#include "Object.h"


namespace WM {
    class Position: public Object {
    private:
        Length _left;
        Length _top;


    public:
        Position(): _left(0), _top(0) {
        }
        
        
        Position(Length left, Length top): _left(left), _top(top) {
        }


        Length getLeft() {
            return _left;
        }


        Length getTop() {
            return _top;
        }
    };
}
