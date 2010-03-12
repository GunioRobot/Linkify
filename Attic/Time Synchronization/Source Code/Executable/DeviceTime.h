#pragma once


#include "TimeReceiver.h"
#include "TimeSender.h"
#include "Wm.h"


class DeviceTime: public TimeReceiver, public TimeSender, public Wm::Timer {
public:
    virtual void finalize() {
        Wm::Timer::stop();
    }
    
    
    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }
    
    
    virtual void initialize(bool automatic) {
        onTimeout();
        Wm::Timer::start(1 * 1000);
    }


    virtual void onStatusChange(Wm::String status) {
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        if (!SetLocalTime(&time)) {
            Wm::Exception::throwLastError();
        }
    }


    virtual void onTimeout() {
        SYSTEMTIME time;
        
        GetLocalTime(&time);
        getListeners()->onTimeChange(time);
    }
};
