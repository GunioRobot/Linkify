#pragma once


#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Gps: public Object {
    private:
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


        virtual GPS_POSITION getPosition(size_t maxAgeMs) {
            GPS_POSITION position;

            position.dwVersion = GPS_VERSION_1;
            position.dwSize = sizeof(GPS_POSITION);

            DWORD result = Api::Gps::GPSGetPosition(getHandle(),
                &position, maxAgeMs, 0);

            if (result != ERROR_SUCCESS) {
                Exception::throwError(result);
            }

            return position;
        }


        virtual ref<Event> getPositionEvent() {
            return _locationChanged;
        }
    };
}
