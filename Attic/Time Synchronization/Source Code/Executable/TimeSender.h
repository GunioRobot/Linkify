#pragma once


#include "TimeInformation.h"
#include "TimeListeners.h"
#include "TimeSource.h"


class TimeSender: public TimeInformation, public TimeSource {
private:
    ref<TimeListeners> _listeners;


public:
    TimeSender(): _listeners(new TimeListeners()) {
    }


    virtual void addListener(ref<TimeListener> listener) {
        _listeners->addListener(listener);
    }


    virtual void start() = 0;
    virtual void stop() = 0;
    
    
    virtual void removeListener(ref<TimeListener> listener) {
        _listeners->removeListener(listener);
    }


protected:
    virtual ref<TimeListeners> getListeners() {
        return _listeners;
    }
};
