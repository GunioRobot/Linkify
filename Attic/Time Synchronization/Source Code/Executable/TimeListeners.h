#pragma once


#include <list>
#include "Library/Wm.h"
#include "TimeListener.h"
#include "TimeSource.h"


class TimeListeners: public TimeListener, public TimeSource {
private:
    std::list<ref<TimeListener>> _listeners;
    ref<Wm::Mutex> _modifications;


public:
    TimeListeners(): _modifications(new Wm::Mutex()) {
    }


    virtual void addListener(ref<TimeListener> listener) {
        _modifications->lock();
        _listeners.push_back(listener);
        _modifications->unlock();
    }


    virtual size_t getCount() {
        return _listeners.size();
    }


    virtual void onStatusChange(Wm::String status) {
        _modifications->lock();
        std::list<ref<TimeListener>>::iterator it;

        for (it = _listeners.begin(); it != _listeners.end(); ++it) {
            (*it)->onStatusChange(status);
        }

        _modifications->unlock();
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        _modifications->lock();
        std::list<ref<TimeListener>>::iterator it;

        for (it = _listeners.begin(); it != _listeners.end(); ++it) {
            (*it)->onTimeChange(time);
        }

        _modifications->unlock();
    }
    
    
    virtual void removeListener(ref<TimeListener> listener) {
        _modifications->lock();
        _listeners.remove(listener);
        _modifications->unlock();
    }
};
