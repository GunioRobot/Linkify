#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Event: public Object {
    private:
        HANDLE _handle;


    public:
        Event(bool automatic = false) {
            _handle = CreateEvent(NULL, !automatic, false, NULL);

            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }
        
        
        virtual ~Event() {
            if (!CloseHandle(_handle)) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getHandle() {
            return _handle;
        }


        virtual DWORD getValue() {
            SetLastError(ERROR_SUCCESS);
            DWORD value = GetEventData(getHandle());

            if ((value == 0) && (GetLastError() != ERROR_SUCCESS)) {
                Exception::throwLastError();
            }

            return value;
        }


        virtual void reset() {
            if (!ResetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void set() {
            if (!SetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void setValue(DWORD value) {
            if (!SetEventData(getHandle(), value)) {
                Exception::throwLastError();
            }
        }


        virtual bool wait(DWORD ms = INFINITE) {
            DWORD result = WaitForSingleObject(getHandle(), ms);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }

            return result != WAIT_TIMEOUT;
        }
    };
}
