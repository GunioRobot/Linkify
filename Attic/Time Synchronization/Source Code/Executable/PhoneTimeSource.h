#pragma once


#include "TimeSource.h"
#include "Wm.h"


class PhoneTimeSource: public TimeSource, public Wm::Thread {
private:
    ref<Wm::Ril> _device;
    ref<Wm::Event> _finalize;
    ref<TimeListener> _listener;


public:
    PhoneTimeSource(): _finalize(new Wm::Event()) {
    }


    virtual void finalize() {
        _finalize->set();
        _listener = NULL;
        wait();
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Phone");
    }


    virtual void initialize(ref<TimeListener> listener) {
        _finalize->reset();
        _listener = listener;
        start();
    }


    virtual void onRun() {
        if (!initializeDevice()) {
            return;
        }

        try {
            if (!isNitzEnabled()) {
                _listener->onStatusChange(S("NITZ disabled."));
                _device = NULL;
                return;
            }
        }
        catch (Wm::Exception exception) {
            _listener->onStatusChange(S("NITZ unsupported: ")
                + exception.getMessage());
            _device = NULL;
            return;
        }
        
        updateLoop();
        _device = NULL;
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        _listener->onTimeChange(time);
    }


private:
    bool initializeDevice() {
        try {
            _device = new Wm::Ril();
        }
        catch (Wm::Exception exception) {
            _listener->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return false;
        }

        if (!_device->isRadioPresent()) {
            _listener->onStatusChange(S("No radio module present, waiting..."));
            
            do {
                if (_finalize->wait(1 * 1000)) {
                    _device = NULL;
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


    void updateLoop() {
        ref<Wm::EventManager> events = new Wm::EventManager();
        ref<Wm::Asynchronous<SYSTEMTIME>> time = NULL;
        
        events->add(_finalize);
        
        do {
            if (!time.null()) {
                _listener->onTimeChange(time->getValue());
                events->remove(time->getEvent());
                time = NULL;
            }

            try {
                time = _device->getSystemTime();
                events->add(time->getEvent());
            }
            catch (Wm::Exception exception) {
                _listener->onStatusChange(S("Time query error: ")
                    + exception.getMessage());
            }
        }
        while (events->wait(time.null() ? 1 * 1000 : INFINITE) != _finalize);
    }
};
