#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Waitable: public Object {
    private:
        HANDLE _handle;


    public:
        Waitable(HANDLE handle = NULL): _handle(handle) {
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
            if (getWaitableHandle() == NULL) {
                throw Exception(S("Waitable handle is null."));
            }

            DWORD result = WaitForSingleObject(getWaitableHandle(), ms);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }

            return result != WAIT_TIMEOUT;
        }
    };
}
