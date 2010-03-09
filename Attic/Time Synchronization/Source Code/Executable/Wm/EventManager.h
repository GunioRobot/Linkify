#pragma once


#include <vector>
#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class EventManager: public Object {
    private:
        std::vector<ref<Event>> _events;


    public:
        virtual void add(ref<Event> event) {
            if (!event.null()) {
                _events.push_back(event);
            }
        }


        virtual void remove(ref<Event> event) {
            if (!event.null()) {
                std::vector<ref<Event>>::iterator it = _events.begin();

                for (; it != _events.end(); ++it) {
                    if (*it == event) {
                        _events.erase(it);
                        break;
                    }
                }
            }
        }


        virtual ref<Event> wait(DWORD ms = INFINITE) {
            if (_events.empty()) {
                return NULL;
            }

            HANDLE* _handles = (HANDLE*)
                _alloca(_events.size() * sizeof(HANDLE));

            for (size_t i = 0; i < _events.size(); ++i) {
                _handles[i] = _events[i]->getHandle();
            }

            DWORD result = WaitForMultipleObjects(_events.size(), _handles,
                false, ms);

            switch (result) {
            case WAIT_FAILED:
                Exception::throwLastError();
            case WAIT_TIMEOUT:
                return NULL;
            default:
                return _events[result - WAIT_OBJECT_0];
            }
        }
    };
}
