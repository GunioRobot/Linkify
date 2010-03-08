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
        Thread() {
            _handle = CreateThread(NULL, 0, genericRun, this,
                CREATE_SUSPENDED, NULL);

            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Thread() {
            CloseHandle(_handle);
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
            if (!_finished.null()) {
                _finished->reset();
            }
            if (ResumeThread(getHandle()) == 0xFFFFFFFF) {
                Exception::throwLastError();
            }
        }


        virtual void wait(size_t ms = SIZE_MAX) {
            if (_finished.null()) {
                _finished = new Event();
            }

            _finished->wait(ms);
        }


    private:
        int run() {
            bool success = true;

            try {
                onRun();
            }
            catch (Exception exception) {
                Application::exit(exception);
                success = false;
            }

            if (!_finished.null()) {
                try {
                    _finished->set();
                }
                catch (Exception exception) {
                    Application::exit(exception);
                    success = false;
                }
            }

            return success ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    };
}
