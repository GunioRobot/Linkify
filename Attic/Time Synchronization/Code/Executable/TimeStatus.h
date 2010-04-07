#pragma once


#include <wm/Label.h>
#include <wm/TextBox.h>
#include <wm/time/Listener.h>
#include <wm/time/Sender.h>


class TimeStatus: public wm::time::Listener {
private:
    ref<wm::time::Sender> _time;
    ref<wm::Label> _label;
    ref<wm::TextBox> _textBox;


public:
    TimeStatus(ref<wm::time::Sender> time): _time(time) {
        _label = new wm::Label(_time->getName() + S(":"));
        _textBox = new wm::TextBox(S("Initializing..."));
    }


    virtual void finalize() {
        _time->onFinalize();
        _time->removeListener(noref this);
    }


    virtual ref<wm::Label> getLabel() {
        return _label;
    }


    virtual ref<wm::TextBox> getTextBox() {
        return _textBox;
    }


    virtual void initialize(bool automatic) {
        _time->addListener(noref this);
        _time->onInitialize(automatic);
    }


    virtual void onStatusChange(wm::String status) {
        getTextBox()->setText(status);
    }


    virtual void onTimeChange(wm::DateTime time) {
        getTextBox()->setText(time.formatIso(S(" ")));
    }
};
