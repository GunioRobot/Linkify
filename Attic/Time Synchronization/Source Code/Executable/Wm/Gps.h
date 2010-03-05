#pragma once


#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Gps: public Object {
    private:
        static size_t _references;


    private:
        ref<Event> _locationChange;
        ref<Event> _stateChange;
        HANDLE _handle;


    public:
        Gps(): _locationChange(new Event(true)), _stateChange(new Event(true)) {
            if ((_references == 0) && (Api::Gps::Load() == NULL)) {
                Exception::throwLastError();
            }
            
            ++_references;
            
            _handle = Api::Gps::GPSOpenDevice(
                _locationChange->getHandle(),
                _stateChange->getHandle(),
                NULL,
                0);

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
    };
}
