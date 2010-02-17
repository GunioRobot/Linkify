#include "WM.h"


class Viewer: public WM::Application, WM::Window {
private:
    ref<WM::MenuItem> _exitOption;
    ref<WM::MenuItem> _updateOption;


public:
    Viewer(HINSTANCE handle):
        WM::Application(handle), WM::Window(S("Viewer"), S("VIEWER"))
    {
        _exitOption = new WM::MenuItem(S("Exit"));
        _updateOption = new WM::MenuItem(S("Update"));
     
        const size_t PADDING = 4;
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));
        ref<WM::Label> localTimeLabel = new WM::Label(S("Local time:"), 65, 20);
        ref<WM::TextBox> localTime = new WM::TextBox(S("-"), 165, 20);

        mainMenu->add(_updateOption);
        mainMenu->add(_exitOption);
        
        enableMenuBar(mainMenu);
        add(localTimeLabel, PADDING, PADDING + 2);
        add(localTime, PADDING + localTimeLabel->getWidth(), PADDING);
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
        return Viewer(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::ErrorMessageBox(exception.getMessage());
        return EXIT_FAILURE;
    }
}
