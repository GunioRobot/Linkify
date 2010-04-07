#include <vector>
#include <wm/Application.h>
#include <wm/Menu.h>
#include <wm/MenuItem.h>
#include <wm/Thread.h>
#include <wm/time/Gps.h>
#include <wm/time/MeanMultiplexer.h>
#include <wm/time/Multiplexer.h>
#include <wm/time/Phone.h>
#include <wm/time/System.h>
#include <wm/Window.h>
#include "TimeStatus.h"


class Executable: public wm::Application, public wm::Thread, public wm::Window {
public:
    static const wm::String TITLE;
    static const wm::String WINDOW_CLASS;


private:
    wm::String _startCaption;
    wm::String _stopCaption;
    ref<wm::MenuItem> _pauseOption;
    ref<wm::MenuItem> _exitOption;
    ref<wm::time::Multiplexer> _timeMultiplexer;
    std::vector<ref<TimeStatus>> _timeItems;


public:
    Executable(HINSTANCE handle):
        wm::Application(handle),
        wm::Window(WINDOW_CLASS, TITLE),
        _startCaption(S("Start")),
        _stopCaption(S("Stop")),
        _pauseOption(new wm::MenuItem(_startCaption)),
        _exitOption(new wm::MenuItem(S("Exit"))),
        _timeMultiplexer(new wm::time::MeanMultiplexer())
    {
        ref<wm::Menu> menuBar = new wm::Menu(S("Menu"));
        
        menuBar->add(_pauseOption);
        menuBar->add(_exitOption);
        setMenuBar(menuBar);
        setupTimeSources();
    }


    virtual void onChoose(ref<wm::MenuItem> item) {
        if (item == _pauseOption) {
            if (_pauseOption->getCaption() == _startCaption) {
                _timeMultiplexer->start();
                _pauseOption->setCaption(_stopCaption);
            }
            else {
                _timeMultiplexer->stop();
                _pauseOption->setCaption(_startCaption);
            }
        }
        else if (item == _exitOption) {
            wm::Thread::start();
        }
    }


    virtual void onRun() {
        for (size_t i = 0; i < _timeItems.size(); ++i) {
            _timeItems[i]->finalize();
        }
        
        close();
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        
        for (size_t i = 0; i < _timeItems.size(); ++i) {
            _timeItems[i]->initialize(true);
        }
        
        return wm::Application::start(windowShowMode);
    }


private:
    void setupTimeSources() {
        size_t margin = 8, padding = 3;
        wm::Length largestLabel = 0;
        
        wm::Margin labelMargin(margin, margin + padding, 0, margin + padding);
        wm::Margin boxMargin(margin + margin, margin, margin, margin);
        wm::Size labelSize(0, 20 + margin);
        wm::Size boxSize(wm::Length(100, wm::Percent), labelSize.height());
        
        ref<wm::time::System> systemTime = new wm::time::System();
        ref<wm::time::Phone> phoneTime = new wm::time::Phone();
        ref<wm::time::Gps> gpsTime = new wm::time::Gps();

        _timeMultiplexer->addListener(systemTime);
        phoneTime->addListener(_timeMultiplexer);
        gpsTime->addListener(_timeMultiplexer);

        _timeItems.push_back(new TimeStatus(systemTime));
        _timeItems.push_back(new TimeStatus(phoneTime));
        _timeItems.push_back(new TimeStatus(gpsTime));

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeStatus> status = _timeItems[i];
            ref<wm::Label> label = status->getLabel();
            ref<wm::TextBox> box = status->getTextBox();

            label->setFitToWidth(true);
            label->setMargin(labelMargin);
            label->setSize(labelSize);

            box->setMargin(boxMargin);
            box->setReadOnly(true);
            box->setSize(boxSize);

            add(label);
            add(box);

            wm::Length width = label->getSize().width();

            if (width > largestLabel) {
                largestLabel = width;
            }
        }
        
        ref<TimeStatus> previous;

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeStatus> current = _timeItems[i];
            ref<wm::Label> label = current->getLabel();
            ref<wm::TextBox> box = current->getTextBox();

            if (!previous.null()) {
                wm::Position position = previous->getLabel()->getPosition();
                wm::Size size = previous->getLabel()->getSize();

                label->setPosition(wm::Position(position.left(),
                    position.top() + size.height()));
            }

            box->setPosition(wm::Position(largestLabel,
                label->getPosition().top()));

            previous = current;
        }
    }
};
