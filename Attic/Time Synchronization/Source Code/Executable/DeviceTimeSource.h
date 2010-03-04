#pragma once


#include "TimeSource.h"
#include "Wm.h"


class DeviceTimeSource: public TimeSource {
public:
    virtual Wm::String getDescription() {
        return S("Uses the local time from the internal clock.");
    }


    virtual Wm::String getName() {
        return S("Device");
    }


    virtual Wm::String getStatus() {
        return S("Ready.");
    }


    virtual SYSTEMTIME getTime() {
        SYSTEMTIME time;
        
        GetLocalTime(&time);
        return time;
    }


    virtual bool isReady() {
        return true;
    }
};
