#pragma once


#include "TimeSource.h"
#include "Wm.h"


class TimeInformation: public Wm::Timer {
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
        _label = new Wm::DynamicLabel(
            _timeSource->getName() + S(":"),
            _timeSource->getName() + S(" status:"));

        _textBox = new Wm::TextBox(S("Starting..."));
    }


    virtual ref<Wm::Label> getLabel() {
        return _label;
    }


    virtual ref<Wm::TextBox> getTextBox() {
        return _textBox;
    }


    virtual void onTimeout() {
        if (_timeSource->isReady()) {
            SYSTEMTIME time = _timeSource->getTime();
            
            _sntprintf(_buffer, _LENGTH, TEXT("%d-%02d-%02d %02d:%02d:%02d"),
                time.wYear, time.wMonth, time.wDay,
                time.wHour, time.wMinute, time.wSecond);

            getTextBox()->setText(_buffer);
        }
        else {
            getTextBox()->setText(_timeSource->getStatus());
        }
    }


    virtual void start() {
        onTimeout();
        Wm::Timer::start(1 * 1000);
    }
};
