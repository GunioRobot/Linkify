#pragma once


#include "Exception.h"
#include "Waitable.h"


namespace Wm {
    class Event: public Waitable {
    private:
        bool _valueCopy;


    public:
        Event(bool automatic = false):
            Waitable(CreateEvent(NULL, !automatic, false, NULL)),
            _valueCopy(false)
        {
            if (getHandle() == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Event() {
            if (_valueCopy) {
                delete getValue();
            }
        }


        virtual HANDLE getHandle() {
            return getWaitableHandle();
        }


        virtual void* getValue() {
            SetLastError(ERROR_SUCCESS);
            DWORD value = GetEventData(getHandle());

            if ((value == 0) && (GetLastError() != ERROR_SUCCESS)) {
                Exception::throwLastError();
            }

            return (void*) value;
        }


        virtual void notify() {
            if (!SetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void reset() {
            if (!ResetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void setValue(void* value) {
            void* previous = _valueCopy ? getValue() : NULL;

            if (!SetEventData(getHandle(), (DWORD) value)) {
                Exception::throwLastError();
            }
            
            if (_valueCopy) {
                delete previous;
                _valueCopy = false;
            }
        }


        virtual void setValue(void* value, size_t size) {
            bool doCopy = (value != NULL) && (size > 0);
            void* copy = NULL;
            
            if (doCopy) {
                copy = new BYTE[size];
                memcpy(copy, value, size);
            }

            try {
                setValue(copy);
                _valueCopy = doCopy;
            }
            catch (Exception) {
                if (doCopy) {
                    delete copy;
                }
                throw;
            }
        }
    };
}
