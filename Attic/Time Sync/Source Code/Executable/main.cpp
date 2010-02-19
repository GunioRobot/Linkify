#include "WM.h"


class Executable: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;


public:
    Executable(HINSTANCE handle):
        WM::Application(handle), WM::Window(S("Time Sync"), S("TIMESYNC"))
    {
        _exitOption = new WM::MenuItem(S("Exit"));
        _updateOption = new WM::MenuItem(S("Update"));
     
        const long MARGIN = 4;
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));

        ref<WM::Label> deviceLabel = new WM::Label(S("Device:"), 44, 20 - 2);
        ref<WM::TextBox> deviceTime = new WM::TextBox(S("-"), WM::Widget::EXPANDABLE, 20);

        mainMenu->add(_updateOption);
        mainMenu->add(_exitOption);
        enableMenuBar(mainMenu);

        deviceTime->setRightMargin(4);
        add(deviceLabel, MARGIN, MARGIN + 2);
        add(deviceTime, MARGIN + deviceLabel->getWidth(), MARGIN);
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


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        return Executable(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::ErrorMessageBox(exception.getMessage());
        return EXIT_FAILURE;
    }
}
