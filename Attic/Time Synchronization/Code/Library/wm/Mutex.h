#pragma once


#include "Exception.h"
#include "Waitable.h"


namespace wm {
    class Mutex: public Waitable {
    public:
        Mutex(): Waitable(CreateMutex(NULL, false, NULL)) {
            if (getMutexHandle() == NULL) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getMutexHandle() {
            return getWaitableHandle();
        }


        virtual void lock() {
            wait(INFINITE);
        }


        virtual void unlock() {
            if (!ReleaseMutex(getMutexHandle())) {
                Exception::throwLastError();
            }
        }
    };
}
