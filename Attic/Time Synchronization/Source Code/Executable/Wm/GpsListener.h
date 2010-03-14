#pragma once


#include "Object.h"


namespace Wm {
    class GpsListener: public Object {
    public:
        virtual void onPositionChange(GPS_POSITION position) = 0;
    };
}
