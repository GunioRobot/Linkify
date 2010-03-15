#pragma once


#include "TimeListener.h"
#include "TimeSender.h"
#include "Wm.h"


class TimeStatus: public TimeListener {
private:
    ref<TimeSender> _time;
    ref<Wm::Label> _label;
    ref<Wm::TextBox> _textBox;


public:
    TimeStatus(ref<TimeSender> time): _time(time) {
        _label = new Wm::Label(_time->getName() + S(":"));
        _textBox = new Wm::TextBox(S("Initializing..."));
    }


    virtual void finalize() {
        _time->onFinalize();
        _time->removeListener(noref this);
    }


    virtual ref<Wm::Label> getLabel() {
        return _label;
    }


    virtual ref<Wm::TextBox> getTextBox() {
        return _textBox;
    }


    virtual void initialize(bool automatic) {
        _time->addListener(noref this);
        _time->onInitialize(true);
    }


    virtual void onStatusChange(Wm::String status) {
        getTextBox()->setText(status);
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        const TCHAR* spec = TEXT("%d-%02d-%02d %02d:%02d:%02d");

        getTextBox()->setText(Wm::String::format(spec,
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond));
    }
};
