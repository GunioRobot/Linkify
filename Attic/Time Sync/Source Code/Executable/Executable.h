#include "TimeTextBox.h"
#include "WM.h"


class Executable: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;


public:
    Executable(HINSTANCE handle):
        WM::Application(handle), WM::Window(S("Time Sync"), S("TIMESYNC")),
        _exitOption(new WM::MenuItem(S("Exit"))),
        _updateOption(new WM::MenuItem(S("Update")))
    {
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));
        ref<WM::Label> deviceLabel = new WM::Label(S("Device:"));
        ref<WM::TextBox> deviceBox = new TimeTextBox(GetLocalTime);
        ref<WM::Label> gpsLabel = new WM::Label(S("GPS:"));
        ref<WM::TextBox> gpsBox = new WM::TextBox();

        long margin = 8;
        long padding = 3;
        
        WM::Margin labelMargin(margin, margin + padding, margin, margin);
        WM::Margin boxMargin(margin, margin, margin, margin);
        
        WM::Size labelSize(50 + padding, 20 + margin);
        WM::Size boxSize(WM::Length(100, WM::Percent), labelSize.height());

        deviceLabel->setSize(labelSize);
        deviceLabel->setMargin(labelMargin);

        gpsLabel->setSize(labelSize);
        gpsLabel->setPosition(WM::Position(deviceLabel->getPosition().left(), deviceLabel->getSize().height()));
        gpsLabel->setMargin(labelMargin);

        deviceBox->setSize(boxSize);
        deviceBox->setPosition(WM::Position(deviceLabel->getSize().width(), deviceLabel->getPosition().top()));
        deviceBox->setMargin(boxMargin);
        deviceBox->setReadOnly(true);

        gpsBox->setSize(boxSize);
        gpsBox->setPosition(WM::Position(gpsLabel->getSize().width(), gpsLabel->getPosition().top()));
        gpsBox->setMargin(boxMargin);
        gpsBox->setReadOnly(true);

        mainMenu->add(_updateOption);
        mainMenu->add(_exitOption);

        setMenuBar(mainMenu);
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
