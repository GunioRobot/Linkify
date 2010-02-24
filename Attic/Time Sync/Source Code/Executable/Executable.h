#include "TimeTextBox.h"
#include "WM.h"


#define LABEL_DEVICE S("Device:")
#define LABEL_GPS S("GPS:")

#define MENU_BAR S("Menu")

#define OPTION_EXIT S("Exit")
#define OPTION_UPDATE S("Update")

#define TITLE S("Time Sync")
#define WINDOW_CLASS S("TIMESYNC")


class Executable: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;


public:
    Executable(HINSTANCE handle):
        WM::Application(handle), WM::Window(WINDOW_CLASS, TITLE),
        _exitOption(new WM::MenuItem(OPTION_EXIT)),
        _updateOption(new WM::MenuItem(OPTION_UPDATE))
    {
        ref<WM::Menu> menuBar = new WM::Menu(MENU_BAR);
        ref<WM::Label> deviceLabel = new WM::Label(LABEL_DEVICE);
        ref<WM::TextBox> deviceBox = new TimeTextBox(GetLocalTime);
        ref<WM::Label> gpsLabel = new WM::Label(LABEL_GPS);
        ref<WM::TextBox> gpsBox = new WM::TextBox();

        long margin = 8;
        long padding = 3;
        
        WM::Margin labelMargin(margin, margin + padding, 0, margin + padding);
        WM::Margin boxMargin(margin + margin, margin, margin, margin);
        
        WM::Size labelSize(0, 20 + margin);
        WM::Size boxSize(WM::Length(100, WM::Percent), labelSize.height());

        deviceLabel->setSize(labelSize);
        deviceLabel->setMargin(labelMargin);
        deviceLabel->setFitToWidth(true);

        gpsLabel->setSize(deviceLabel->getSize());
        gpsLabel->setPosition(WM::Position(deviceLabel->getPosition().left(), deviceLabel->getSize().height()));
        gpsLabel->setMargin(labelMargin);

        deviceBox->setSize(boxSize);
        deviceBox->setPosition(WM::Position(deviceLabel->getSize().width(), deviceLabel->getPosition().top()));
        deviceBox->setMargin(boxMargin);
        deviceBox->setReadOnly(true);

        gpsBox->setSize(boxSize);
        gpsBox->setPosition(WM::Position(deviceLabel->getSize().width(), gpsLabel->getPosition().top()));
        gpsBox->setMargin(boxMargin);
        gpsBox->setReadOnly(true);

        menuBar->add(_updateOption);
        menuBar->add(_exitOption);

        setMenuBar(menuBar);
        add(deviceLabel);
        add(deviceBox);
        add(gpsLabel);
        add(gpsBox);
    }


    virtual void onChoose(ref<WM::MenuItem> item) {
        if (item == _exitOption) {
            close();
        }
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return WM::Application::start(windowShowMode);
    }
};
