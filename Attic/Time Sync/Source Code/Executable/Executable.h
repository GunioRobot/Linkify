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
        ref<WM::TextBox> gpsBox = new WM::TextBox(WM::TextBox::ALIGN_CENTER);

        const long margin = 8;
        const long padding = 3;
        const RECT labelMargin = {margin, margin + padding, margin, margin};
        const RECT boxMargin = {margin, margin, margin, margin};

        deviceLabel->setSize(50 + padding, 20 + margin);
        deviceLabel->setPosition(0, 0);
        deviceLabel->setMargin(labelMargin);

        gpsLabel->setSize(deviceLabel);
        gpsLabel->setPosition(0, deviceLabel->getHeight());
        gpsLabel->setMargin(labelMargin);

        deviceBox->setSize(WM::Widget::EXPANDABLE, deviceLabel->getHeight());
        deviceBox->setPosition(deviceLabel->getWidth(), 0);
        deviceBox->setMargin(boxMargin);
        deviceBox->setReadOnly(true);

        gpsBox->setSize(deviceBox);
        gpsBox->setPosition(gpsLabel->getWidth(), gpsLabel->getTop());
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


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return WM::Application::start(windowShowMode);
    }


protected:
    virtual void onChoose(ref<WM::MenuItem> item) {
        if (item == _exitOption) {
            close();
        }
    }
};
