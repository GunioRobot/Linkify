#pragma once


#include "TimeSource.h"
#include "Wm.h"


class CellularRadioTimeSource: public TimeSource, public Wm::Timer {
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
            _error = S("Unable to query NITZ support: ") + exception.getMessage();
        }
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Cellular Radio");
    }


    virtual void onTimeout() {
        ref<Wm::Result> time;
        
        try {
            time = _cellularRadio->getSystemTime();
        }
        catch (Wm::Exception exception) {
            getListener()->onStatusChange(S("Unable to query time: ")
                + exception.getMessage());
            return;
        }

        getListener()->onTimeChange(time->getValue<SYSTEMTIME>());
    }


    virtual void setListener(ref<TimeSource::Listener> listener) {
        TimeSource::setListener(listener);
        
        if (getListener().null()) {
            stop();
        }
        else if (isReady()) {
            onTimeout();
            start(1 * 1000);
        }
        else {
            getListener()->onStatusChange(getStatus());
        }
    }
    
    
private:
    Wm::String getStatus() {
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
    
    
    bool isReady() {
        return !_cellularRadio.null()
            && _cellularRadio->isRadioPresent()
            && _nitzSupported;
    }
};
