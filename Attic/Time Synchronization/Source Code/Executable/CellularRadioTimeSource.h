#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource:
    public TimeSource, public TimeListener, public Wm::Thread
{
private:
    ref<Wm::CellularRadio> _device;
    ref<Wm::Event> _finalize;
    ref<TimeListener> _listener;


public:
    CellularRadioTimeSource(): _finalize(new Wm::Event()) {
    }


    virtual void finalize(DWORD waitMs = INFINITE) {
        _device = NULL;
        _finalize->set();
        wait(waitMs);
        _listener = NULL;
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual void initialize(ref<TimeListener> listener) {
        _listener = listener;
        _finalize->reset();
        start();
    }


    virtual void onRun() {
        if (!initializeDevice()) {
            return;
        }

        try {
            if (!isNitzEnabled()) {
                onStatusChange(S("NITZ disabled."));
                return;
            }
        }
        catch (Wm::Exception exception) {
            onStatusChange(S("NITZ unsupported: ") + exception.getMessage());
            return;
        }
        
        for (; !_device.null(); _finalize->wait(1 * 1000)) {
            try {
                onTimeChange(_device->getSystemTime()->getValue());
            }
            catch (Wm::Exception exc) {
                onStatusChange(S("Time query error: ") + exc.getMessage());
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


private:
    bool initializeDevice() {
        try {
            _device = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            onStatusChange(S("Not available: ") + exception.getMessage());
            return false;
        }

        if (!_device->isRadioPresent()) {
            onStatusChange(S("No radio module present, waiting..."));
            
            do {
                _finalize->wait(1 * 1000);
                if (_device.null()) {
                    return false;
                }
            }
            while (!_device->isRadioPresent());
        }

        return true;
    }


    bool isNitzEnabled() {
        ref<Wm::AsynchronousResult> nitzSupport =
            _device->queryFeatures(RIL_CAPSTYPE_NITZNOTIFICATION);

        switch (nitzSupport->getValueAs<DWORD>()) {
        case RIL_CAPS_NITZ_ENABLED:
            return true;
        case RIL_CAPS_NITZ_DISABLED:
            return false;
        default:
            throw Wm::Exception("Invalid feature query response.");
        }
    }
};
