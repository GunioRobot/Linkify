#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource: public TimeSource {
private:
    ref<Wm::CellularRadio> _cellularRadio;
    Wm::String _error;
    bool _nitzSupported;


public:
    CellularRadioTimeSource() {
        try {
            _cellularRadio = new Wm::CellularRadio();
        }
        catch (Wm::Exception exception) {
            _error = S("Phone not available: ") + exception.getMessage();
            return;
        }

        try {
            ref<Wm::Result> nitzSupport =
                _cellularRadio->queryFeatures(RIL_CAPSTYPE_NITZNOTIFICATION);

            switch (nitzSupport->getValue<DWORD>()) {
            case RIL_CAPS_NITZ_ENABLED:
                _nitzSupported = true;
                break;
            case RIL_CAPS_NITZ_DISABLED:
                _nitzSupported = false;
                break;
            default:
                throw Wm::Exception("Invalid RIL feature response.");
                break;
            }
        }
        catch (Wm::Exception exception) {
            _cellularRadio = NULL;
            _error = S("Unknown NITZ support: ") + exception.getMessage();
        }
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual Wm::String getStatus() {
        if (_cellularRadio.null()) {
            return _error;
        }
        else if (!_cellularRadio->isRadioPresent()) {
            return S("Radio module not detected.");
        }
        else if (!_nitzSupported) {
            return S("NITZ not supported.");
        }
        else {
            return S("Ready.");
        }
    }


    virtual SYSTEMTIME getTime() {
        return _cellularRadio->getSystemTime()->getValue<SYSTEMTIME>();
    }


    virtual bool isReady() {
        return !_cellularRadio.null()
            && _cellularRadio->isRadioPresent()
            && _nitzSupported;
    }
};
