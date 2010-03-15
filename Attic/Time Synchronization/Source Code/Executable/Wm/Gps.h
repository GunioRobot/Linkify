#pragma once


#include <list>
#include "Event.h"
#include "Exception.h"
#include "GpsListener.h"
#include "Thread.h"


namespace Wm {
    class Gps: protected Thread {
    private:
        static size_t _references;


    private:
        HANDLE _handle;
        ref<Event> _positionChange;
        std::list<ref<GpsListener>> _listeners;
        bool _running;


    public:
        Gps(): _handle(NULL), _positionChange(new Event(true)), _running(true) {
            if ((_references == 0) && (Api::Gps::Load() == NULL)) {
                Exception::throwLastError();
            }

            ++_references;
            Thread::start();
        }


        virtual ~Gps() {
            stop();

            _running = false;
            _positionChange->notify();
            wait();

            if ((--_references == 0) && !Api::Gps::Unload()) {
                Exception::throwLastError();
            }
        }


        virtual void addListener(ref<GpsListener> listener) {
            _listeners.push_back(listener);
        }
        
        
        virtual HANDLE getGpsHandle() {
            return _handle;
        }


        virtual GPS_POSITION getPosition(size_t maxAgeMs = 1000) {
            GPS_POSITION position;

            position.dwVersion = GPS_VERSION_1;
            position.dwSize = sizeof(GPS_POSITION);

            DWORD result = Api::Gps::GPSGetPosition(getGpsHandle(),
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

            DWORD result = Api::Gps::GPSGetDeviceState(&state);

            if (result != ERROR_SUCCESS) {
                Wm::Exception::throwError(result);
            }

            return state;
        }


        virtual void removeListener(ref<GpsListener> listener) {
            _listeners.remove(listener);
        }
        
        
        virtual void start() {
            if (_handle == NULL) {
                _handle = Api::Gps::GPSOpenDevice(
                    _positionChange->getEventHandle(), NULL, NULL, 0);

                if (_handle == NULL) {
                    throw Exception(S("Failed to open GPS device."));
                }
            }
        }


        virtual void stop() {
            if (_handle != NULL) {
                DWORD result = Api::Gps::GPSCloseDevice(_handle);
                
                if (result != ERROR_SUCCESS) {
                    Exception::throwError(result);
                }

                _handle = NULL;
            }
        }


    protected:
        virtual void onRun() {
            while (_positionChange->wait()) {
                if (!_running) {
                    break;
                }
                
                std::list<ref<GpsListener>>::iterator it;
                GPS_POSITION position = getPosition();

                for (it = _listeners.begin(); it != _listeners.end(); ++it) {
                    (*it)->onPositionChange(position);
                }
            }
        }
    };
}
