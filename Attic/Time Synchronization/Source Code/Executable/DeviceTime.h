#pragma once


#include "TimeReceiver.h"
#include "TimeSender.h"
#include "Wm.h"


class DeviceTime: public TimeReceiver, public TimeSender, public Wm::Timer {
public:
    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
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
    
    
    virtual void start() {
        onTimeout();
        Wm::Timer::start(1 * 1000);
    }


    virtual void stop() {
        Wm::Timer::stop();
    }
};
