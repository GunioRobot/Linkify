#pragma once


#include "Application.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Thread: public Object {
    private:
        static DWORD WINAPI genericRun(LPVOID userData) {
            Thread* self = (Thread*) userData;

            try {
                self->run();
                CloseHandle(self->getHandle());
                return EXIT_SUCCESS;
            }
            catch (Exception exception) {
                Application::exit(exception);
                return EXIT_FAILURE;
            }
        }


    private:
        HANDLE _handle;


    public:
        Thread() {
            _handle = CreateThread(NULL, 0, genericRun, this,
                CREATE_SUSPENDED, NULL);

            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getHandle() {
            return _handle;
        }

        
        virtual void run() = 0;


        virtual void setPriority(int priority) {
            if (!SetThreadPriority(getHandle(), priority)) {
                Exception::throwLastError();
            }
        }


        virtual void start() {
            if (ResumeThread(getHandle()) == 0xFFFFFFFF) {
                Exception::throwLastError();
            }
        }
    };
}
