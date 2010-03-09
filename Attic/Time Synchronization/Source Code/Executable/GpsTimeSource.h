#pragma once


#include "TimeSource.h"
#include "Wm.h"


class GpsTimeSource: public TimeSource, public Wm::Thread {
private:
    ref<Wm::Gps> _device;
    ref<Wm::Event> _finalize;
    ref<TimeListener> _listener;


public:
    GpsTimeSource(): _finalize(new Wm::Event()) {
    }


    virtual void finalize() {
        _finalize->set();
        _listener = NULL;
        wait();
    }


    virtual Wm::String getDescription() {
        return S("Uses the time provided by GPS satellites.");
    }


    virtual Wm::String getName() {
        return S("GPS");
    }


    virtual void initialize(ref<TimeListener> listener) {
        _finalize->reset();
        _listener = listener;
        start();
    }


    virtual void onRun() {
        try {
            _device = new Wm::Gps();
        }
        catch (Wm::Exception exception) {
            _listener->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return;
        }

        updateLoop();
        _device = NULL;
    }


private:
    void updateLoop() {
        ref<Wm::EventManager> events = new Wm::EventManager();

        events->add(_finalize);
        events->add(_device->getPositionEvent());

        for (GPS_POSITION pos; events->wait() != _finalize;) {
            try {
                pos = _device->getPosition(1 * 1000);
            }
            catch (Wm::Exception exception) {
                _listener->onStatusChange(S("Position query error: ")
                    + exception.getMessage());
                continue;
            }
            
            if ((pos.dwValidFields & GPS_VALID_UTC_TIME) != 0) {
                _listener->onTimeChange(pos.stUTCTime);
            }
            else if (pos.FixQuality == GPS_FIX_QUALITY_UNKNOWN) {
                _listener->onStatusChange(S("Obtaining position fix..."));
            }
            else if ((pos.dwValidFields & GPS_VALID_SATELLITE_COUNT) != 0) {
                _listener->onStatusChange(Wm::String::format(
                    TEXT("Locked on to %u satellites."), pos.dwSatelliteCount));
            }
            else {
                _listener->onStatusChange("No time information available.");
            }
        }
    }
};
