#pragma once


#include "AsynchronousResult.h"


namespace Wm {
    template<typename T>
    class Asynchronous: public AsynchronousResult {
    public:
        Asynchronous(ref<Event> event = new Event()): AsynchronousResult(event) {
        }


        virtual T getValue() {
            return getValueAs<T>();
        }
    };
}
