#pragma once


#include "Library/Label.h"
#include "Library/TextBox.h"
#include "Library/Time/Listener.h"
#include "Library/Time/Sender.h"


class TimeStatus: public Wm::Time::Listener {
private:
    ref<Wm::Time::Sender> _time;
    ref<Wm::Label> _label;
    ref<Wm::TextBox> _textBox;


public:
    TimeStatus(ref<Wm::Time::Sender> time): _time(time) {
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
        _time->onInitialize(automatic);
    }


    virtual void onStatusChange(Wm::String status) {
        getTextBox()->setText(status);
    }


    virtual void onTimeChange(Wm::DateTime time) {
        getTextBox()->setText(time.formatIso(S(" ")));
    }
};
