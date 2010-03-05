#pragma once


#include "AsynchronousResult.h"
#include "Event.h"
#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Gps: public Object {
    private:
        class PositionResult: public AsynchronousResult {
        private:
            ref<Gps> _gps;


        public:
            PositionResult(ref<Gps> gps):
                AsynchronousResult(gps->_locationChanged), _gps(gps)
            {
            }


            virtual void* getRawValue() {
                void* value = AsynchronousResult::getRawValue();

                if (value == NULL) {
                    GPS_POSITION position;

                    position.dwVersion = GPS_VERSION_1;
                    position.dwSize = sizeof(GPS_POSITION);
                    
                    DWORD result = Api::Gps::GPSGetPosition(_gps->getHandle(),
                        &position, 1 * 1000, 0);

                    if (result != ERROR_SUCCESS) {
                        Exception::throwError(result);
                    }

                    return setRawValue(&position, sizeof(GPS_POSITION));
                }

                return value;
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


        virtual ref<AsynchronousResult> getPosition() {
            return new PositionResult(noref this);
        }
    };
}
