#pragma once


#include "AsynchronousResult.h"


namespace Wm {
    template<typename T>
    class Asynchronous: protected AsynchronousResult {
    public:
        Asynchronous(ref<Event> event): AsynchronousResult(event) {
        }


        virtual T getValue() {
            return getValueAs<T>();
        }
    };
}
