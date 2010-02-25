#include "TimeTextBox.h"
#include "WM.h"


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


class Executable: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;

    ref<WM::Label> _deviceLabel;
    ref<WM::Label> _gpsLabel;
    
    ref<WM::TextBox> _deviceBox;
    ref<WM::TextBox> _gpsBox;

    ref<WM::Ril> _ril;


public:
    Executable(HINSTANCE handle):
        WM::Application(handle),
        WM::Window(WINDOW_CLASS, TITLE),
        _exitOption(new WM::MenuItem(OPTION_EXIT)),
        _updateOption(new WM::MenuItem(OPTION_UPDATE)),
        _deviceLabel(new WM::DynamicLabel(LABEL_DEVICE_SHORT, LABEL_DEVICE_LONG)),
        _gpsLabel(new WM::DynamicLabel(LABEL_GPS_SHORT, LABEL_GPS_LONG)),
        _deviceBox(new TimeTextBox(GetLocalTime)),
        _gpsBox(new WM::TextBox()),
        _ril(new WM::Ril())
    {
        ref<WM::Menu> menuBar = new WM::Menu(MENU_BAR);

        long margin = 8;
        long padding = 3;
        
        WM::Margin labelMargin(margin, margin + padding, 0, margin + padding);
        WM::Margin boxMargin(margin + margin, margin, margin, margin);
        
        WM::Size labelSize(0, 20 + margin);
        WM::Size boxSize(WM::Length(100, WM::Percent), labelSize.height());

        _deviceLabel->setSize(labelSize);
        _deviceLabel->setMargin(labelMargin);
        _deviceLabel->setFitToWidth(true);

        _gpsLabel->setSize(labelSize);
        _gpsLabel->setPosition(WM::Position(_deviceLabel->getPosition().left(),
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


    virtual void onChoose(ref<WM::MenuItem> item) {
        if (item == _exitOption) {
            close();
        }
    }


    virtual void onResize() {
        _deviceBox->setPosition(WM::Position(_deviceLabel->getSize().width(),
            _deviceLabel->getPosition().top()));
        
        _gpsBox->setPosition(WM::Position(_deviceLabel->getSize().width(),
            _gpsLabel->getPosition().top()));
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return WM::Application::start(windowShowMode);
    }
};
