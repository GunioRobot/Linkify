#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource: public TimeSource, public Wm::Thread {
private:
    ref<Wm::CellularRadio> _cellularRadio;


public:
    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual void onFinalize() {
        _cellularRadio = NULL;
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
        
        for (; !_cellularRadio.null(); sleep(1 * 1000)) {
            try {
                ref<Wm::Result> time = _cellularRadio->getSystemTime();
                getListener()->onTimeChange(time->getValue<SYSTEMTIME>());
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
            _cellularRadio = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("Phone unavailable: ")
                + exception.getMessage());
            return false;
        }

        if (!_cellularRadio->isRadioPresent()) {
            getListener()->onStatusChange(S("Phone is off, waiting..."));
            
            do {
                sleep(1 * 1000);
                if (_cellularRadio.null()) {
                    return false;
                }
            }
            while (!_cellularRadio->isRadioPresent());
        }

        return true;
    }


    bool isNitzEnabled() {
        ref<Wm::Result> nitzSupport =
            _cellularRadio->queryFeatures(RIL_CAPSTYPE_NITZNOTIFICATION);

        switch (nitzSupport->getValue<DWORD>()) {
        case RIL_CAPS_NITZ_ENABLED:
            return true;
        case RIL_CAPS_NITZ_DISABLED:
            return false;
        default:
            throw Wm::Exception("Invalid feature query response.");
        }
    }
};
