#pragma once


#include <list>
#include "../Mutex.h"
#include "Listener.h"
#include "Source.h"


namespace wm {
namespace time {
    class Listeners: public Listener, public Source {
    private:
        std::list<ref<Listener>> _listeners;
        ref<Mutex> _modifications;


    public:
        Listeners(): _modifications(new Mutex()) {
        }


        virtual void addListener(ref<Listener> listener) {
            _modifications->lock();
            _listeners.push_back(listener);
            _modifications->unlock();
        }


        virtual void onStatusChange(String status) {
            _modifications->lock();
            std::list<ref<Listener>>::iterator it;

            for (it = _listeners.begin(); it != _listeners.end(); ++it) {
                (*it)->onStatusChange(status);
            }

            _modifications->unlock();
        }


        virtual void onTimeChange(DateTime time) {
            _modifications->lock();
            std::list<ref<Listener>>::iterator it;

            for (it = _listeners.begin(); it != _listeners.end(); ++it) {
                (*it)->onTimeChange(time);
            }

            _modifications->unlock();
        }
        
        
        virtual void removeListener(ref<Listener> listener) {
            _modifications->lock();
            _listeners.remove(listener);
            _modifications->unlock();
        }
    };
}}
