#pragma once


#include "TimeSender.h"
#include "Wm.h"


class GpsTime: public TimeSender, public Wm::GpsListener {
private:
    ref<Wm::Gps> _device;


public:
    virtual void finalize() {
        if (!_device.null()) {
            _device->removeListener(noref this);
            _device->stop();
            _device = NULL;
        }
    }


    virtual Wm::String getDescription() {
        return S("Uses the time provided by GPS satellites.");
    }


    virtual Wm::String getName() {
        return S("GPS");
    }


    virtual void initialize(bool automatic) {
        try {
            _device = new Wm::Gps();
            bool isOn = (_device->getState().dwDeviceState == SERVICE_STATE_ON);

            if (!automatic && !isOn) {
                throw Wm::Exception("Device is off.");
            }

            getListeners()->onStatusChange(S("Starting..."));
            _device->start();
        }
        catch (Wm::Exception exception) {
            _device = NULL;
            getListeners()->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return;
        }

        _device->addListener(noref this);
    }


    virtual void onPositionChange(GPS_POSITION pos) {
        if ((pos.dwValidFields & GPS_VALID_UTC_TIME) != 0) {
            getListeners()->onTimeChange(pos.stUTCTime);
        }
        else if (pos.FixQuality == GPS_FIX_QUALITY_UNKNOWN) {
            getListeners()->onStatusChange(S("Obtaining position fix..."));
        }
        else if ((pos.dwValidFields & GPS_VALID_SATELLITE_COUNT) != 0) {
            getListeners()->onStatusChange(Wm::String::format(
                TEXT("Locked on to %u satellites."), pos.dwSatelliteCount));
        }
        else {
            getListeners()->onStatusChange("No time information available.");
        }
    }
};
