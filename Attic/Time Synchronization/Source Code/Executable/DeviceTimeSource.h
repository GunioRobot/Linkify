#pragma once


#include "TimeSource.h"
#include "Wm.h"


class DeviceTimeSource: public TimeSource, public Wm::Timer {
private:
    ref<TimeListener> _listener;


public:
    virtual void finalize() {
        stop();
        _listener = NULL;
    }


    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }


    virtual void initialize(ref<TimeListener> listener) {
        _listener = listener;
        onTimeout();
        start(1 * 1000);
    }


    virtual void onTimeout() {
        SYSTEMTIME time;
        
        GetLocalTime(&time);
        _listener->onTimeChange(time);
    }
};
