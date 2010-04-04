#pragma once


#include "Library/all.h"
#include "TimeReceiver.h"
#include "TimeSender.h"


class DeviceTime: public TimeReceiver, public TimeSender, protected Wm::Timer {
public:
    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }
    
    
    virtual void onFinalize() {
        stop();
    }
    
    
    virtual void onInitialize(bool automatic) {
        onTimeout();
        start(1 * 1000);
    }


    virtual void onStatusChange(Wm::String status) {
    }


    virtual void onTimeChange(Wm::DateTime time) {
        if (!SetLocalTime(&time)) {
            Wm::Exception::throwLastError();
        }
    }


protected:
    virtual void onTimeout() {
        Wm::DateTime time;
        
        GetLocalTime(&time);
        getListeners()->onTimeChange(time);
    }
};
