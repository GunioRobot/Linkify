#pragma once


#include "../DateTime.h"
#include "../Exception.h"
#include "../gid/GpsReceiver.h"
#include "../String.h"
#include "Sender.h"


namespace wm {
namespace time {
    class Gps: public Sender, public GpsReceiver::Listener {
    private:
        ref<GpsReceiver> _device;


    public:
        virtual String getDescription() {
            return S("Uses the time provided by GPS satellites.");
        }


        virtual String getName() {
            return S("GPS");
        }


        virtual void onFinalize() {
            if (!_device.null()) {
                _device->removeListener(noref this);
                _device->stop();
                _device = NULL;
            }
        }


        virtual void onInitialize(bool automatic) {
            try {
                _device = new GpsReceiver();
                bool isOn = (_device->getState().dwDeviceState == SERVICE_STATE_ON);

                if (!automatic && !isOn) {
                    throw Exception("Device is off.");
                }

                getListeners()->onStatusChange(S("Starting..."));
                _device->start();
            }
            catch (Exception& exception) {
                _device = NULL;
                getListeners()->onStatusChange(S("Not available: ")
                    + exception.getMessage());
                return;
            }

            _device->addListener(noref this);
        }


    protected:
        virtual void onPositionChange(GPS_POSITION pos) {
            if ((pos.dwValidFields & GPS_VALID_UTC_TIME) != 0) {
                getListeners()->onTimeChange(
                    DateTime::utcToLocal(pos.stUTCTime));
            }
            else if (pos.FixQuality == GPS_FIX_QUALITY_UNKNOWN) {
                getListeners()->onStatusChange(S("Obtaining position fix..."));
            }
            else if ((pos.dwValidFields & GPS_VALID_SATELLITE_COUNT) != 0) {
                getListeners()->onStatusChange(String::format(
                    TEXT("Locked on to %u satellites."), pos.dwSatelliteCount));
            }
            else {
                getListeners()->onStatusChange("No time information available.");
            }
        }
    };
}}
