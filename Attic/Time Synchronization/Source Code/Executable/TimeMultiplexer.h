#pragma once


#include "TimeListener.h"
#include "TimeListeners.h"
#include "TimeSource.h"


class TimeMultiplexer: public TimeListener, public TimeSource {
private:
    ref<TimeListeners> _listeners;


public:
    TimeMultiplexer(): _listeners(new TimeListeners()) {
    }


    virtual void addListener(ref<TimeListener> listener) {
        _listeners->addListener(listener);
    }


    virtual void onStatusChange(Wm::String status) {
        _listeners->onStatusChange(status);
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        _listeners->onTimeChange(time);
    }


    virtual void removeListener(ref<TimeListener> listener) {
        _listeners->removeListener(listener);
    }
};
