#pragma once


#include "Application.h"
#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Thread: public Object {
    private:
        static DWORD WINAPI genericRun(LPVOID userData) {
            return ((Thread*) userData)->run();
        }


    private:
        HANDLE _handle;
        ref<Event> _finished;


    public:
        Thread(): _finished(new Event()) {
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


        virtual HANDLE getHandle() {
            return _handle;
        }

        
        virtual void onRun() = 0;


        virtual void setPriority(int priority) {
            if (!SetThreadPriority(getHandle(), priority)) {
                Exception::throwLastError();
            }
        }


        virtual void sleep(size_t ms) {
            Sleep(ms);
        }


        virtual void start() {
            _finished->reset();

            if (ResumeThread(getHandle()) == 0xFFFFFFFF) {
                Exception::throwLastError();
            }
        }


        virtual void wait(DWORD ms = INFINITE) {
            _finished->wait(ms);
        }


    private:
        int run() {
            try {
                onRun();
                _finished->set();
                return EXIT_SUCCESS;
            }
            catch (Exception exception) {
                Application::exit(exception);
                return EXIT_FAILURE;
            }
        }
    };
}
