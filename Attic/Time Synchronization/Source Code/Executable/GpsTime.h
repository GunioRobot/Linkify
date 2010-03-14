#pragma once


#include "TimeSender.h"
#include "Wm.h"


class GpsTime: public TimeSender, public Wm::Thread {
private:
    ref<Wm::Gps> _device;
    ref<Wm::Event> _stop;
    bool _automatic;


public:
    GpsTime(): _stop(new Wm::Event()) {
    }


    virtual void finalize() {
        _stop->notify();
        wait();
    }


    virtual Wm::String getDescription() {
        return S("Uses the time provided by GPS satellites.");
    }


    virtual Wm::String getName() {
        return S("GPS");
    }


    virtual void initialize(bool automatic) {
        _automatic = automatic;
        _stop->reset();
        Wm::Thread::start();
    }


    virtual void onRun() {
        try {
            _device = new Wm::Gps();
            bool isOn = (_device->getState().dwDeviceState == SERVICE_STATE_ON);

            if (!_automatic && !isOn) {
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

        updateLoop();
        _device->stop();
        _device = NULL;
    }


private:
    void updateLoop() {
        ref<Wm::WaitableManager> events = new Wm::WaitableManager();

        events->add(_stop);
        events->add(_device->getPositionEvent());

        for (GPS_POSITION pos; events->wait() != _stop;) {
            try {
                pos = _device->getPosition(1 * 1000);
            }
            catch (Wm::Exception exception) {
                getListeners()->onStatusChange(S("Position query error: ")
                    + exception.getMessage());
                continue;
            }
            
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
    }
};
