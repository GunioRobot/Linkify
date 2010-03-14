#pragma once


#include "Application.h"
#include "Event.h"
#include "Exception.h"
#include "Waitable.h"


namespace Wm {
    class Thread: public Waitable {
    private:
        static DWORD WINAPI genericRun(LPVOID userData) {
            return ((Thread*) userData)->run();
        }


    private:
        HANDLE _handle;
        ref<Event> _finished;


    public:
        Thread(): Waitable(NULL), _finished(new Event()) {
            _handle = CreateThread(NULL, 0, genericRun, this,
                CREATE_SUSPENDED, NULL);

            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Thread() {
            if (!CloseHandle(_handle)) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getThreadHandle() {
            return _handle;
        }


        virtual HANDLE getWaitableHandle() {
            return _finished->getWaitableHandle();
        }

        
        virtual void onRun() = 0;


        virtual void setPriority(int priority) {
            if (!SetThreadPriority(getThreadHandle(), priority)) {
                Exception::throwLastError();
            }
        }


        virtual void sleep(size_t ms) {
            Sleep(ms);
        }


        virtual void start() {
            _finished->reset();

            if (ResumeThread(getThreadHandle()) == 0xFFFFFFFF) {
                Exception::throwLastError();
            }
        }


    private:
        int run() {
            try {
                onRun();
                _finished->notify();
                return EXIT_SUCCESS;
            }
            catch (Exception exception) {
                Application::exit(exception);
                return EXIT_FAILURE;
            }
        }
    };
}
