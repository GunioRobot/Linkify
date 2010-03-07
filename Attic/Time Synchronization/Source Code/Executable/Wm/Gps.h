#pragma once


#include "Asynchronous.h"
#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Gps: public Object {
    private:
        class AsynchronousPosition: public Asynchronous<GPS_POSITION> {
        private:
            ref<Gps> _gps;
            size_t _maxAgeMs;
            GPS_POSITION _position;


        public:
            AsynchronousPosition(ref<Gps> gps, size_t maxAgeMs):
                Asynchronous(gps->_locationChanged),
                _gps(gps),
                _maxAgeMs(maxAgeMs)
            {
                _position.dwSize = 0;
            }


            virtual GPS_POSITION getValue() {
                if (_position.dwSize == 0) {
                    getEvent()->wait();
                    _gps->getPosition(_position, _maxAgeMs);
                }

                return _position;
            }
        };


        static size_t _references;


    private:
        HANDLE _handle;
        ref<Event> _locationChanged;


    public:
        Gps(): _locationChanged(new Event(true)) {
            if ((_references == 0) && (Api::Gps::Load() == NULL)) {
                Exception::throwLastError();
            }
            
            ++_references;
            
            _handle = Api::Gps::GPSOpenDevice(_locationChanged->getHandle(),
                NULL, NULL, 0);

            if (_handle == NULL) {
                throw Exception(S("Failed to open GPS device."));
            }
        }


        virtual ~Gps() {
            DWORD result = Api::Gps::GPSCloseDevice(_handle);

            if ((--_references == 0) && !Api::Gps::Unload()) {
                Exception::throwLastError();
            }

            if (result != ERROR_SUCCESS) {
                Exception::throwError(result);
            }
        }
        
        
        virtual HANDLE getHandle() {
            return _handle;
        }


        virtual ref<Asynchronous<GPS_POSITION>> getPosition(size_t maxAgeMs) {
            return new AsynchronousPosition(noref this, maxAgeMs);
        }


    private:
        void getPosition(GPS_POSITION& position, size_t maxAgeMs) {
            position.dwVersion = GPS_VERSION_1;
            position.dwSize = sizeof(GPS_POSITION);

            DWORD result = Api::Gps::GPSGetPosition(getHandle(),
                &position, maxAgeMs, 0);

            if (result != ERROR_SUCCESS) {
                Exception::throwError(result);
            }
        }
    };
}
