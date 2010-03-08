#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource: public TimeSource, public Wm::Thread {
private:
    ref<Wm::CellularRadio> _device;
    ref<Wm::Event> _finalize;


public:
    CellularRadioTimeSource(): _finalize(new Wm::Event()) {
    }


    virtual void finalize(DWORD waitMs = INFINITE) {
        _device = NULL;
        _finalize->set();
        wait(waitMs);
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual void initialize() {
        _finalize->reset();
        start();
    }


    virtual void onRun() {
        if (!initializeDevice()) {
            return;
        }

        try {
            if (!isNitzEnabled()) {
                getListener()->onStatusChange(S("NITZ disabled."));
                return;
            }
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("NITZ unsupported: ")
                + exception.getMessage());
            return;
        }
        
        for (SYSTEMTIME time; !_device.null(); _finalize->wait(1 * 1000)) {
            try {
                time = _device->getSystemTime()->getValue();
                getListener()->onTimeChange(time);
            }
            catch (Wm::Exception exception) {
                getListener()->onStatusChange(S("Time query error: ")
                    + exception.getMessage());
            }
        }
    }


private:
    bool initializeDevice() {
        try {
            _device = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return false;
        }

        if (!_device->isRadioPresent()) {
            getListener()->onStatusChange(
                S("No radio module present, waiting..."));
            
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
