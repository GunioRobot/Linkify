#pragma once


#include "Exception.h"
#include "Waitable.h"


namespace Wm {
    class Mutex: public Waitable {
    public:
        Mutex(): Waitable(CreateMutex(NULL, false, NULL)) {
            if (getHandle() == NULL) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getHandle() {
            return getWaitableHandle();
        }


        virtual void lock() {
            wait(INFINITE);
        }


        virtual void unlock() {
            if (!ReleaseMutex(getHandle())) {
                Exception::throwLastError();
            }
        }
    };
}
