#include <vector>
#include "DeviceTime.h"
#include "GpsTime.h"
#include "PhoneTime.h"
#include "TimeMultiplexer.h"
#include "TimeStatus.h"
#include "Wm.h"


class Executable: public Wm::Application, public Wm::Thread, public Wm::Window {
public:
    static const Wm::String TITLE;
    static const Wm::String WINDOW_CLASS;


private:
    ref<Wm::MenuItem> _exitOption;
    std::vector<ref<TimeStatus>> _timeItems;


public:
    Executable(HINSTANCE handle):
        Wm::Application(handle),
        Wm::Window(WINDOW_CLASS, TITLE),
        _exitOption(new Wm::MenuItem(S("Exit")))
    {
        ref<Wm::Menu> menuBar = new Wm::Menu(S("Menu"));
        
        menuBar->add(_exitOption);
        setMenuBar(menuBar);
        setupTimeSources();
    }


    virtual void onChoose(ref<Wm::MenuItem> item) {
        if (item == _exitOption) {
            Wm::Thread::start();
        }
    }


    virtual void onRun() {
        for (size_t i = 0; i < _timeItems.size(); ++i) {
            _timeItems[i]->stop();
        }
        
        close();
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        
        for (size_t i = 0; i < _timeItems.size(); ++i) {
            _timeItems[i]->start();
        }
        
        return Wm::Application::start(windowShowMode);
    }


private:
    void setupTimeSources() {
        size_t margin = 8, padding = 3;
        Wm::Length largestLabel = 0;
        
        Wm::Margin labelMargin(margin, margin + padding, 0, margin + padding);
        Wm::Margin boxMargin(margin + margin, margin, margin, margin);
        Wm::Size labelSize(0, 20 + margin);
        Wm::Size boxSize(Wm::Length(100, Wm::Percent), labelSize.height());
        
        ref<TimeMultiplexer> multiplexer = new TimeMultiplexer(5);
        ref<DeviceTime> deviceTime = new DeviceTime();
        ref<PhoneTime> phoneTime = new PhoneTime();
        ref<GpsTime> gpsTime = new GpsTime();

        multiplexer->addListener(deviceTime);
        phoneTime->addListener(multiplexer);
        gpsTime->addListener(multiplexer);

        _timeItems.push_back(new TimeStatus(deviceTime));
        _timeItems.push_back(new TimeStatus(phoneTime));
        _timeItems.push_back(new TimeStatus(gpsTime));

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeStatus> timeItem = _timeItems[i];
            ref<Wm::Label> label = timeItem->getLabel();
            ref<Wm::TextBox> box = timeItem->getTextBox();

            label->setFitToWidth(true);
            label->setMargin(labelMargin);
            label->setSize(labelSize);

            box->setMargin(boxMargin);
            box->setReadOnly(true);
            box->setSize(boxSize);

            add(label);
            add(box);

            Wm::Length width = label->getSize().width();

            if (width > largestLabel) {
                largestLabel = width;
            }
        }
        
        ref<TimeStatus> previous;

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeStatus> current = _timeItems[i];
            ref<Wm::Label> label = current->getLabel();
            ref<Wm::TextBox> box = current->getTextBox();

            if (!previous.null()) {
                Wm::Position position = previous->getLabel()->getPosition();
                Wm::Size size = previous->getLabel()->getSize();

                label->setPosition(Wm::Position(position.left(),
                    position.top() + size.height()));
            }

            box->setPosition(Wm::Position(largestLabel,
                label->getPosition().top()));

            previous = current;
        }
    }
};
