#pragma once


#include "Event.h"
#include "Object.h"


namespace Wm {
    class AsynchronousResult: public Object {
    private:
        ref<Event> _event;


    public:
        AsynchronousResult(ref<Event> event): _event(event) {
        }


        template<typename T>
        T getValueAs() {
            getEvent()->wait();
            return *(T*) getEvent()->getValue();
        }


    protected:
        virtual ref<Event> getEvent() {
            return _event;
        }
    };
}
