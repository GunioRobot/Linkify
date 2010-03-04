#pragma once


#include "TimeSource.h"
#include "Wm.h"


class TimeInformation: public TimeSource::Listener {
private:
    // Store the time in the following format: "YYYY-MM-DD HH:MM:SS".
    static const size_t _LENGTH = 4 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1 + 2 + 1;


private:
    TCHAR _buffer[_LENGTH];
    ref<TimeSource> _timeSource;
    ref<Wm::Label> _label;
    ref<Wm::TextBox> _textBox;


public:
    TimeInformation(ref<TimeSource> timeSource): _timeSource(timeSource) {
        _label = new Wm::Label(_timeSource->getName() + S(":"));
        _textBox = new Wm::TextBox(S("Starting..."));

        timeSource->setListener(noref this);
    }


    virtual ref<Wm::Label> getLabel() {
        return _label;
    }


    virtual ref<Wm::TextBox> getTextBox() {
        return _textBox;
    }


    virtual void onStatusChange(Wm::String status) {
        getTextBox()->setText(status);
    }


    virtual void onTimeChange(SYSTEMTIME time) {
        _sntprintf(_buffer, _LENGTH, TEXT("%d-%02d-%02d %02d:%02d:%02d"),
            time.wYear, time.wMonth, time.wDay,
            time.wHour, time.wMinute, time.wSecond);

        getTextBox()->setText(_buffer);
    }
};
