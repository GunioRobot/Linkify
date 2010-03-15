#pragma once


#include "TimeSender.h"
#include "Wm.h"


class PhoneTime: public TimeSender, protected Wm::Thread {
private:
    ref<Wm::Ril> _device;
    ref<Wm::Event> _stop;
    ref<Wm::WaitableManager> _events;
    bool _automatic;
    bool _isOn;
    DWORD _originalEquipmentState;


public:
    PhoneTime(): _stop(new Wm::Event()), _events(new Wm::WaitableManager()) {
        _events->add(_stop);
    }


    virtual Wm::String getDescription() {
        return S("Uses NITZ data sent by the cellular network.");
    }


    virtual Wm::String getName() {
        return S("Phone");
    }


    virtual void onFinalize() {
        _stop->notify();
        wait();
    }


    virtual void onInitialize(bool automatic) {
        _automatic = automatic;
        _stop->reset();
        start();
    }


protected:
    virtual void onRun() {
        if (initializeDevice()) {
            updateLoop();
            finalizeDevice();
        }
    }


private:
    bool checkEquipmentState() {
        ref<Wm::Result<RILEQUIPMENTSTATE>> state = _device->getEquipmentState();
        _events->add(state);

        if (_events->wait() == _stop) {
            _events->remove(state);
            return false;
        }

        _events->remove(state);
        _isOn = (state->getValue().dwRadioSupport == RIL_RADIOSUPPORT_ON);

        if (!_isOn) {
            if (!_automatic) {
                throw Wm::Exception("Device is off.");
            }

            _originalEquipmentState = state->getValue().dwEqState;
            _device->setEquipmentState(RIL_EQSTATE_FULL)->wait();
        }

        return true;
    }


    bool checkNitzSupport() {
        try {
            ref<Wm::Result<DWORD>> support
                = _device->getCapabilities<DWORD>(RIL_CAPSTYPE_NITZNOTIFICATION);
            _events->add(support);

            if (_events->wait() == _stop) {
                _events->remove(support);
                return false;
            }
            
            _events->remove(support);

            switch (support->getValue()) {
            case RIL_CAPS_NITZ_ENABLED:
                return true;
            case RIL_CAPS_NITZ_DISABLED:
                getListeners()->onStatusChange(S("NITZ disabled."));
                return false;
            default:
                throw Wm::Exception(S("Invalid feature query response."));
            }
        }
        catch (Wm::Exception& exception) {
            getListeners()->onStatusChange(S("Unknown NITZ support: ")
                + exception.getMessage());
            return false;
        }
    }


    void finalizeDevice() {
        if (!_isOn && _automatic) {
            _device->setEquipmentState(_originalEquipmentState);
        }

        _device = NULL;
    }


    bool initializeDevice() {
        try {
            _device = new Wm::Ril();
            getListeners()->onStatusChange(S("Starting..."));

            if (!checkNitzSupport() || !checkEquipmentState()) {
                _device = NULL;
                return false;
            }
        }
        catch (Wm::Exception& exception) {
            _device = NULL;
            getListeners()->onStatusChange(S("Not available: ")
                + exception.getMessage());
            return false;
        }

        return true;
    }


    void updateLoop() {
        ref<Wm::Result<SYSTEMTIME>> time = NULL;
        
        do {
            if (!time.null()) {
                getListeners()->onTimeChange(time->getValue());
                _events->remove(time);
                time = NULL;
            }

            try {
                time = _device->getSystemTime();
                _events->add(time);
            }
            catch (Wm::Exception& exception) {
                getListeners()->onStatusChange(S("Time query error: ")
                    + exception.getMessage());
            }
        }
        while (_events->wait(time.null() ? 1 * 1000 : INFINITE) != _stop);
    }
};
