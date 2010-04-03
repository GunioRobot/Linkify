#pragma once


#include <map>
#include "Application.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Timer: public Object {
    private:
        static const UINT _INVALID_ID = 0;
        static std::map<UINT, Timer*> _timers;


        static void CALLBACK genericHandler(
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
                catch (Exception& exception) {
                    timer->stop();
                    Application::exit(exception);
                }
            }
        }


    private:
        UINT _id;


    public:
        Timer(): _id(_INVALID_ID) {
        }


        ~Timer() {
            stop();
        }


        virtual void onTimeout() = 0;


        virtual void start(size_t ms) {
            if (_id != _INVALID_ID) {
                throw Exception(S("Timer already started."));
            }

            _timers[_id = SetTimer(NULL, 0, ms, genericHandler)] = this;

            if (_id == _INVALID_ID) {
                _timers.erase(_id);
                Exception::throwLastError();
            }
        }


        virtual void stop() {
            if (_id == _INVALID_ID) {
                return;
            }

            if (!KillTimer(NULL, _id)) {
                Exception::throwLastError();
            }

            _timers.erase(_id);
            _id = _INVALID_ID;
        }
    };
}
