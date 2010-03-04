#pragma once


#include "TimeSource.h"
#include "Wm.h"


class DeviceTimeSource: public TimeSource, public Wm::Timer {
public:
    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }


    virtual void onTimeout() {
        SYSTEMTIME time;
        
        GetLocalTime(&time);
        getListener()->onTimeChange(time);
    }

    
    virtual void setListener(ref<TimeSource::Listener> listener) {
        TimeSource::setListener(listener);
        
        if (getListener().null()) {
            stop();
        }
        else {
            onTimeout();
            start(1 * 1000);
        }
    }
};
