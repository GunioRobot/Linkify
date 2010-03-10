#pragma once


#include <list>
#include "TimeListener.h"
#include "TimeSource.h"
#include "Wm.h"


class TimeListeners: public TimeListener, public TimeSource {
private:
    std::list<ref<TimeListener>> _listeners;
    ref<Wm::Mutex> _mutex;


public:
    TimeListeners(): _mutex(new Wm::Mutex()) {
    }


    virtual void addListener(ref<TimeListener> listener) {
        _mutex->lock();
        _listeners.push_back(listener);
        _mutex->unlock();
    }


    virtual size_t getCount() {
        return _listeners.size();
    }


    virtual void onStatusChange(Wm::String status) {
        _mutex->lock();
        std::list<ref<TimeListener>>::iterator it;

        for (it = _listeners.begin(); it != _listeners.end(); ++it) {
            (*it)->onStatusChange(status);
        }

        _mutex->unlock();
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        _mutex->lock();
        std::list<ref<TimeListener>>::iterator it;

        for (it = _listeners.begin(); it != _listeners.end(); ++it) {
            (*it)->onTimeChange(time);
        }

        _mutex->unlock();
    }
    
    
    virtual void removeListener(ref<TimeListener> listener) {
        _mutex->lock();
        _listeners.remove(listener);
        _mutex->unlock();
    }
};
