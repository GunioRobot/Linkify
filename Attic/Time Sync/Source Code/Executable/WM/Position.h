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


        virtual Length left() {
            return _left;
        }


        virtual Length top() {
            return _top;
        }
    };
}
