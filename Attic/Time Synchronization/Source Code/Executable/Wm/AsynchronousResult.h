#pragma once


#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class AsynchronousResult: public Object {
    private:
        ref<Event> _event;
        bool _ownEvent;
        void* _value;


    public:
        AsynchronousResult():
            _event(new Event()), _ownEvent(true), _value(NULL)
        {
        }


        AsynchronousResult(ref<Event> event):
            _event(event), _ownEvent(false), _value(NULL)
        {
        }


        virtual ~AsynchronousResult() {
            if (_value != NULL) {
                delete[] _value;
            }
        }


        virtual void* getRawValue() {
            if (!_event.null()) {
                _event->wait();
                _event = NULL;
            }

            return _value;
        }


        template<typename T>
        T getValue() {
            return *(T*) getRawValue();
        }

        
        virtual void* setRawValue(void* value, size_t size = 0) {
            _value = (size > 0) ? new BYTE[size] : value;

            if (size > 0) {
                memcpy(_value, value, size);
            }
            if (_ownEvent && !_event.null()) {
                _event->set();
            }

            return _value;
        }
    };
}
