#pragma once


#include "TimeSender.h"
#include "Wm.h"


class GpsTime: public TimeSender, public Wm::Thread {
private:
    ref<Wm::Gps> _device;
    ref<Wm::Event> _stop;


public:
    GpsTime(): _stop(new Wm::Event()) {
    }


    virtual Wm::String getDescription() {
        return S("Uses the time provided by GPS satellites.");
    }


    virtual Wm::String getName() {
        return S("GPS");
    }


    virtual void onRun() {
        try {
            _device = new Wm::Gps();
        }
        catch (Wm::Exception exception) {
            getListeners()->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return;
        }

        updateLoop();
        _device = NULL;
    }


    virtual void start() {
        _stop->reset();
        Wm::Thread::start();
    }


    virtual void stop() {
        _stop->set();
        wait();
    }


private:
    void updateLoop() {
        ref<Wm::EventManager> events = new Wm::EventManager();

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
