#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Waitable: public Object {
    private:
        HANDLE _handle;


    public:
        Waitable(HANDLE handle): _handle(handle) {
        }


        virtual ~Waitable() {
            if ((_handle != NULL) && !CloseHandle(_handle)) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getWaitableHandle() {
            return _handle;
        }


        virtual bool wait(DWORD ms = INFINITE) {
            DWORD result = WaitForSingleObject(getWaitableHandle(), ms);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }

            return result != WAIT_TIMEOUT;
        }
    };
}
