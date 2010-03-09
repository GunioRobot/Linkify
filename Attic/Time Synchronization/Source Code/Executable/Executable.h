#include <vector>
#include "DeviceTimeSource.h"
#include "GpsTimeSource.h"
#include "PhoneTimeSource.h"
#include "TimeInformation.h"
#include "Wm.h"


class Executable: public Wm::Application, public Wm::Window {
public:
    static const Wm::String TITLE;
    static const Wm::String WINDOW_CLASS;


private:
    ref<Wm::MenuItem> _exitOption;
    ref<Wm::MenuItem> _updateOption;
    std::vector<ref<TimeInformation>> _timeItems;


public:
    Executable(HINSTANCE handle):
        Wm::Application(handle),
        Wm::Window(WINDOW_CLASS, TITLE),
        _exitOption(new Wm::MenuItem(S("Exit"))),
        _updateOption(new Wm::MenuItem(S("Update")))
    {
        ref<Wm::Menu> menuBar = new Wm::Menu(S("Menu"));

        menuBar->add(_updateOption);
        menuBar->add(_exitOption);

        setMenuBar(menuBar);
        setupTimeSources();
    }


    virtual void onChoose(ref<Wm::MenuItem> item) {
        if (item == _exitOption) {
            for (size_t i = 0; i < _timeItems.size(); ++i) {
                _timeItems[i]->finalize(0);
            }
            
            close();
        }
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        
        for (size_t i = 0; i < _timeItems.size(); ++i) {
            _timeItems[i]->initialize();
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
        
        _timeItems.push_back(new TimeInformation(new DeviceTimeSource()));
        _timeItems.push_back(new TimeInformation(new PhoneTimeSource()));
        _timeItems.push_back(new TimeInformation(new GpsTimeSource()));

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeInformation> timeItem = _timeItems[i];
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
        
        ref<TimeInformation> previous;

        for (size_t i = 0; i < _timeItems.size(); ++i) {
            ref<TimeInformation> current = _timeItems[i];
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
