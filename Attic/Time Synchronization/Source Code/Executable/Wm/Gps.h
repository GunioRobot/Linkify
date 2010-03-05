#pragma once


#include "Event.h"
#include "Object.h"


namespace Wm {
    class Gps: public Object {
    private:
        ref<Event> _locationChange;
        ref<Event> _stateChange;
        HANDLE _handle;


    public:
        Gps(): _locationChange(new Event(true)), _stateChange(new Event(true)) {
        }


        virtual HANDLE getHandle() {
            return _handle;
        }
    };
}
