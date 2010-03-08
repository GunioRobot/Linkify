#pragma once


#include "Wm.h"


class TimeSource: public Wm::Object {
public:
    class Listener: public Wm::Object {
    public:
        virtual void onStatusChange(Wm::String status) = 0;
        virtual void onTimeChange(SYSTEMTIME time) = 0;
    };


private:
    ref<TimeSource::Listener> _listener;


public:
    virtual void finalize(DWORD waitMs = INFINITE) = 0;
    virtual Wm::String getDescription() = 0;
    
    
    virtual ref<TimeSource::Listener> getListener() {
        return _listener;
    }
    
    
    virtual Wm::String getName() = 0;
    virtual void initialize() = 0;


    virtual void setListener(ref<TimeSource::Listener> listener) {
        _listener = listener;
    }
};
