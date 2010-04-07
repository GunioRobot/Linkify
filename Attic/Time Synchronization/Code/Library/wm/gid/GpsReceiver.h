#pragma once


#include <list>
#include "../Event.h"
#include "../Exception.h"
#include "../Thread.h"
#include "interface.h"


namespace wm {
namespace gid {
    class GpsReceiver: protected Thread {
    public:
        class Listener: public Object {
        public:
            virtual void onPositionChange(GPS_POSITION position) = 0;
        };
    
    
    private:
        static size_t _references;


    private:
        HANDLE _handle;
        ref<Event> _positionChange;
        std::list<ref<Listener>> _listeners;
        bool _running;


    public:
        GpsReceiver():
            _handle(NULL),
            _positionChange(new Event(true)),
            _running(true)
        {
            if ((_references == 0) && (Load() == NULL)) {
                Exception::throwLastError();
            }

            ++_references;
            Thread::start();
        }


        virtual ~GpsReceiver() {
            stop();

            _running = false;
            _positionChange->notify();
            wait();

            if ((--_references == 0) && !Unload()) {
                Exception::throwLastError();
            }
        }


        virtual void addListener(ref<Listener> listener) {
            _listeners.push_back(listener);
        }
        
        
        virtual HANDLE getGpsHandle() {
            return _handle;
        }


        virtual GPS_POSITION getPosition(size_t maxAgeMs = 1000) {
            GPS_POSITION position;

            position.dwVersion = GPS_VERSION_1;
            position.dwSize = sizeof(GPS_POSITION);

            DWORD result = GPSGetPosition(getGpsHandle(),
                &position, maxAgeMs, 0);

            if (result != ERROR_SUCCESS) {
                Exception::throwError(result);
            }

            return position;
        }
        
        
        virtual GPS_DEVICE getState() {
            GPS_DEVICE state;

            state.dwVersion = GPS_VERSION_1;
            state.dwSize = sizeof(GPS_DEVICE);

            DWORD result = GPSGetDeviceState(&state);

            if (result != ERROR_SUCCESS) {
                Exception::throwError(result);
            }

            return state;
        }


        virtual void removeListener(ref<Listener> listener) {
            _listeners.remove(listener);
        }
        
        
        virtual void start() {
            if (_handle == NULL) {
                _handle = GPSOpenDevice(_positionChange->getEventHandle(),
                    NULL, NULL, 0);

                if (_handle == NULL) {
                    throw Exception(S("Failed to open GPS device."));
                }
            }
        }


        virtual void stop() {
            if (_handle != NULL) {
                DWORD result = GPSCloseDevice(_handle);
                
                if (result != ERROR_SUCCESS) {
                    Exception::throwError(result);
                }

                _handle = NULL;
            }
        }


    protected:
        virtual void onRun() {
            while (_positionChange->wait() && _running) {
                std::list<ref<Listener>>::iterator it;
                GPS_POSITION position = getPosition();

                for (it = _listeners.begin(); it != _listeners.end(); ++it) {
                    (*it)->onPositionChange(position);
                }
            }
        }
    };
}}
