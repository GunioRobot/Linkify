#pragma once


#include "TimeSource.h"
#include "Wm.h"


class DeviceTimeSource: public TimeSource, public Wm::Timer {
public:
    virtual void finalize() {
        stop();
    }


    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }


    virtual void initialize() {
        onTimeout();
        start(1 * 1000);
    }


    virtual void onTimeout() {
        SYSTEMTIME time;
        
        GetLocalTime(&time);
        getListener()->onTimeChange(time);
    }
};
