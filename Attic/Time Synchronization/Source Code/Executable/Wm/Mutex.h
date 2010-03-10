#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Mutex: public Object {
    private:
        HANDLE _handle;


    public:
        Mutex(): _handle(CreateMutex(NULL, false, NULL)) {
            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Mutex() {
            if (!CloseHandle(_handle)) {
                Exception::throwLastError();
            }
        }
        
        
        virtual HANDLE getHandle() {
            return _handle;
        }


        virtual void lock() {
            DWORD result = WaitForSingleObject(getHandle(), INFINITE);

            if ((result == WAIT_FAILED) || (result == INFINITE)) {
                Exception::throwLastError();
            }
        }


        virtual void unlock() {
            if (!ReleaseMutex(getHandle())) {
                Exception::throwLastError();
            }
        }
    };
}
