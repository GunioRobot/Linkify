#pragma once


#include <map>
#include "Application.h"
#include "Exception.h"
#include "Object.h"


namespace WM {
    class Timer: public Object {
    private:
        static std::map<UINT, Timer*> _timers;


        static void CALLBACK handler(
            HWND window,
            UINT message,
            UINT id,
            DWORD tickCount)
        {
            std::map<UINT, Timer*>::iterator it = _timers.find(id);
            
            if (it != _timers.end()) {
                Timer* timer = (*it).second;

                try {
                    timer->onTimeout();
                }
                catch (Exception exception) {
                    timer->stop();
                    Application::exit(exception);
                }
            }
        }


    private:
        UINT _id;


    public:
        Timer(): _id(0) {
        }


        ~Timer() {
            stop();
        }


        virtual void start(long seconds) {
            if (_id != 0) {
                throw Exception(S("Timer already started."));
            }

            _timers[_id = SetTimer(NULL, 0, seconds * 1000, handler)] = this;

            if (_id == 0) {
                _timers.erase(_id);
                Exception::throwLastError();
            }
        }


        virtual void stop() {
            if (_id == 0) {
                return;
            }

            if (!KillTimer(NULL, _id)) {
                Exception::throwLastError();
            }

            _timers.erase(_id);
            _id = 0;
        }


    protected:
        virtual void onTimeout() = 0;
    };
}
