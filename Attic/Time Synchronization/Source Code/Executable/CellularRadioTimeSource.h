#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource: public TimeSource, public Wm::Thread {
private:
    ref<Wm::CellularRadio> _radio;


public:
    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual void onFinalize() {
        _radio = NULL;
    }


    virtual void onInitialize() {
        start();
    }


    virtual void run() {
        if (!initialize()) {
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
        
        for (; !_radio.null(); sleep(1 * 1000)) {
            try {
                SYSTEMTIME time = _radio->getSystemTime()->getValue();
                getListener()->onTimeChange(time);
            }
            catch (Wm::Exception exception) {
                getListener()->onStatusChange(S("Time query error: ")
                    + exception.getMessage());
            }
        }
    }


private:
    bool initialize() {
        try {
            getListener()->onStatusChange(S("Starting..."));
            _radio = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("Phone unavailable: ")
                + exception.getMessage());
            return false;
        }

        if (!_radio->isRadioPresent()) {
            getListener()->onStatusChange(S("Phone is off, waiting..."));
            
            do {
                sleep(1 * 1000);
                if (_radio.null()) {
                    return false;
                }
            }
            while (!_radio->isRadioPresent());
        }

        return true;
    }


    bool isNitzEnabled() {
        ref<Wm::AsynchronousResult> nitzSupport =
            _radio->queryFeatures(RIL_CAPSTYPE_NITZNOTIFICATION);

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
