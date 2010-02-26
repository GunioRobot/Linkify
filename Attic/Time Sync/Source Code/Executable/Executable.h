#include "TimeTextBox.h"
#include "Wm.h"


#define LABEL_LANDSCAPE S(" time")
#define LABEL_SUFFIX S(":")

#define LABEL_DEVICE S("Device")
#define LABEL_DEVICE_LONG (LABEL_DEVICE + LABEL_LANDSCAPE + LABEL_SUFFIX)
#define LABEL_DEVICE_SHORT (LABEL_DEVICE + LABEL_SUFFIX)

#define LABEL_GPS S("GPS")
#define LABEL_GPS_LONG (LABEL_GPS + LABEL_LANDSCAPE + LABEL_SUFFIX)
#define LABEL_GPS_SHORT (LABEL_GPS + LABEL_SUFFIX)

#define MENU_BAR S("Menu")
#define OPTION_EXIT S("Exit")
#define OPTION_UPDATE S("Update")

#define TITLE S("Time Sync")
#define WINDOW_CLASS S("TIMESYNC")


class Executable: public Wm::Application, Wm::Window {
private:
    ref<Wm::MenuItem> _exitOption;
    ref<Wm::MenuItem> _updateOption;

    ref<Wm::Label> _deviceLabel;
    ref<Wm::Label> _gpsLabel;
    
    ref<Wm::TextBox> _deviceBox;
    ref<Wm::TextBox> _gpsBox;


public:
    Executable(HINSTANCE handle):
        Wm::Application(handle),
        Wm::Window(WINDOW_CLASS, TITLE),
        _exitOption(new Wm::MenuItem(OPTION_EXIT)),
        _updateOption(new Wm::MenuItem(OPTION_UPDATE)),
        _deviceLabel(new Wm::DynamicLabel(LABEL_DEVICE_SHORT, LABEL_DEVICE_LONG)),
        _gpsLabel(new Wm::DynamicLabel(LABEL_GPS_SHORT, LABEL_GPS_LONG)),
        _deviceBox(new TimeTextBox(GetLocalTime)),
        _gpsBox(new Wm::TextBox())
    {
        ref<Wm::Menu> menuBar = new Wm::Menu(MENU_BAR);

        long margin = 8;
        long padding = 3;
        
        Wm::Margin labelMargin(margin, margin + padding, 0, margin + padding);
        Wm::Margin boxMargin(margin + margin, margin, margin, margin);
        
        Wm::Size labelSize(0, 20 + margin);
        Wm::Size boxSize(Wm::Length(100, Wm::Percent), labelSize.height());

        _deviceLabel->setSize(labelSize);
        _deviceLabel->setMargin(labelMargin);
        _deviceLabel->setFitToWidth(true);

        _gpsLabel->setSize(labelSize);
        _gpsLabel->setPosition(Wm::Position(_deviceLabel->getPosition().left(),
            _deviceLabel->getSize().height()));
        _gpsLabel->setMargin(labelMargin);
        _gpsLabel->setFitToWidth(true);

        _deviceBox->setSize(boxSize);
        _deviceBox->setMargin(boxMargin);
        _deviceBox->setReadOnly(true);

        _gpsBox->setSize(boxSize);
        _gpsBox->setMargin(boxMargin);
        _gpsBox->setReadOnly(true);

        menuBar->add(_updateOption);
        menuBar->add(_exitOption);

        setMenuBar(menuBar);
        add(_deviceLabel);
        add(_deviceBox);
        add(_gpsLabel);
        add(_gpsBox);
    }


    virtual void onChoose(ref<Wm::MenuItem> item) {
        if (item == _exitOption) {
            close();
        }
    }


    virtual void onResize() {
        _deviceBox->setPosition(Wm::Position(_deviceLabel->getSize().width(),
            _deviceLabel->getPosition().top()));
        
        _gpsBox->setPosition(Wm::Position(_deviceLabel->getSize().width(),
            _gpsLabel->getPosition().top()));
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return Wm::Application::start(windowShowMode);
    }
};
