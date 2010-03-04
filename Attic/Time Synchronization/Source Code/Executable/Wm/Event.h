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


        virtual bool wait(DWORD ms = INFINITE) {
            DWORD result = WaitForSingleObject(getHandle(), ms);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }

            return result != WAIT_TIMEOUT;
        }
    };
}
