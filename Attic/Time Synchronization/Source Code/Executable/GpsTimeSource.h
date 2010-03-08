#pragma once


#include "TimeSource.h"
#include "Wm.h"


class GpsTimeSource:
    public TimeSource, public TimeListener, public Wm::Thread
{
private:
    ref<Wm::Gps> _device;
    ref<Wm::Event> _finalize;
    ref<TimeListener> _listener;


public:
    GpsTimeSource(): _finalize(new Wm::Event()) {
    }


    virtual void finalize(DWORD waitMs = INFINITE) {
        _device = NULL;
        _finalize->set();
        wait(waitMs);
        _listener = NULL;
    }


    virtual Wm::String getDescription() {
        return S("Uses the time provided by GPS satellites.");
    }


    virtual Wm::String getName() {
        return S("GPS");
    }


    virtual void initialize(ref<TimeListener> listener) {
        _listener = listener;
        _finalize->reset();
        start();
    }


    virtual void onRun() {
        try {
            _device = new Wm::Gps();
        }
        catch (Wm::Exception exception) {
            onStatusChange(S("Not available: ") + exception.getMessage());
            return;
        }

        for (GPS_POSITION pos; !_device.null(); _finalize->wait(1 * 1000)) {
            try {
                pos = _device->getPosition(1 * 1000)->getValue();
            }
            catch (Wm::Exception exc) {
                onStatusChange(S("Position query error: ") + exc.getMessage());
                continue;
            }
            
            if ((pos.dwValidFields & GPS_VALID_UTC_TIME) != 0) {
                onTimeChange(pos.stUTCTime);
            }
            else if (pos.FixQuality == GPS_FIX_QUALITY_UNKNOWN) {
                onStatusChange(S("Obtaining position fix..."));
            }
            else if ((pos.dwValidFields & GPS_VALID_SATELLITE_COUNT) != 0) {
                Wm::StringStream message;
                message << TEXT("Locked on to ") << pos.dwSatelliteCount
                    << TEXT(" satellites.");
                
                onStatusChange(message.str());
            }
            else {
                onStatusChange("No time information available.");
            }
        }
    }


    virtual void onStatusChange(Wm::String status) {
        if (!_listener.null()) {
            _listener->onStatusChange(status);
        }
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        if (!_listener.null()) {
            _listener->onTimeChange(time);
        }
    }
};
