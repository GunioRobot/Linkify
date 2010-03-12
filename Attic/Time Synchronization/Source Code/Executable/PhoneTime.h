#pragma once


#include "TimeSender.h"
#include "Wm.h"


class PhoneTime: public TimeSender, public Wm::Thread {
private:
    ref<Wm::Ril> _device;
    ref<Wm::Event> _stop;


public:
    PhoneTime(): _stop(new Wm::Event()) {
    }


    virtual void finalize() {
        _stop->set();
        wait();
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Phone");
    }


    virtual void initialize(bool automatic) {
        _stop->reset();
        Wm::Thread::start();
    }


    virtual void onRun() {
        if (!initializeDevice()) {
            return;
        }

        try {
            if (!isNitzEnabled()) {
                getListeners()->onStatusChange(S("NITZ disabled."));
                _device = NULL;
                return;
            }
        }
        catch (Wm::Exception exception) {
            getListeners()->onStatusChange(S("NITZ unsupported: ")
                + exception.getMessage());
            _device = NULL;
            return;
        }
        
        updateLoop();
        _device = NULL;
    }


private:
    bool initializeDevice() {
        try {
            _device = new Wm::Ril();
        }
        catch (Wm::Exception exception) {
            getListeners()->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return false;
        }

        if (!_device->isRadioPresent()) {
            getListeners()->onStatusChange(
                S("No radio module present, waiting..."));
            
            do {
                if (_stop->wait(1 * 1000)) {
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
        
        events->add(_stop);
        
        do {
            if (!time.null()) {
                getListeners()->onTimeChange(time->getValue());
                events->remove(time->getEvent());
                time = NULL;
            }

            try {
                time = _device->getSystemTime();
                events->add(time->getEvent());
            }
            catch (Wm::Exception exception) {
                getListeners()->onStatusChange(S("Time query error: ")
                    + exception.getMessage());
            }
        }
        while (events->wait(time.null() ? 1 * 1000 : INFINITE) != _stop);
    }
};
