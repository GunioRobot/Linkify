#pragma once


#include "Event.h"
#include "Waitable.h"


namespace Wm {
    template<typename T>
    class Result: public Waitable {
    private:
        ref<Event> _event;


    public:
        Result(ref<Event> event): Waitable(NULL), _event(event) {
        }


        virtual HANDLE getWaitableHandle() {
            return _event->getWaitableHandle();
        }


        virtual T getValue() {
            _event->wait();
            return *(T*) _event->getValue();
        }
    };
}
