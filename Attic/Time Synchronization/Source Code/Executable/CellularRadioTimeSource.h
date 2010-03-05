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
        bool nitzEnabled;
        
        try {
            getListener()->onStatusChange(S("Starting..."));
            _cellularRadio = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("Phone unavailable: ")
                + exception.getMessage());
            return;
        }

        if (!_cellularRadio->isRadioPresent()) {
            getListener()->onStatusChange(S("Phone is off, waiting..."));
            
            do {
                sleep(1 * 1000);
                if (_cellularRadio.null()) {
                    return;
                }
            }
            while (!_cellularRadio->isRadioPresent());
        }

        try {
            ref<Wm::Result> nitzSupport =
                _cellularRadio->queryFeatures(RIL_CAPSTYPE_NITZNOTIFICATION);

            switch (nitzSupport->getValue<DWORD>()) {
            case RIL_CAPS_NITZ_ENABLED:
                nitzEnabled = true;
                break;
            case RIL_CAPS_NITZ_DISABLED:
                nitzEnabled = false;
                break;
            default:
                throw Wm::Exception("Invalid feature query response.");
                break;
            }
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("NITZ unsupported: ")
                + exception.getMessage());
            return;
        }

        if (!nitzEnabled) {
            getListener()->onStatusChange(S("NITZ disabled."));
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
};
