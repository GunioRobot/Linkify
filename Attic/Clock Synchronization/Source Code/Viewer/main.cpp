#include "WM.h"


class Viewer: public WM::Application, WM::Window {
private:
    ref<WM::String> _exitOption;
    ref<WM::String> _updateOption;


public:
    Viewer(HINSTANCE handle):
        WM::Application(handle), WM::Window(S("Viewer"), S("VIEWER")),
        _exitOption(S("Exit")), _updateOption(S("Update"))
    {
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));

        mainMenu->add(new WM::MenuItem(_updateOption));
        mainMenu->add(new WM::MenuItem(_exitOption));
        enableMenuBar(mainMenu);

        ref<WM::Window> owner = noref (WM::Window*) this;
        ref<WM::Label> localTime = new WM::Label(S("Local time:"), owner);
    }


    virtual void choose(ref<WM::MenuItem> item) {
        if (item->getCaption() == _exitOption) {
            close();
        }
    }


    virtual int start(int windowShowMode) {
        open(windowShowMode);
        return WM::Application::start(windowShowMode);
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


/*
        CreateWindow(
            TEXT("BUTTON"),
            TEXT("OK"),
            WS_CHILD + WS_TABSTOP + WS_VISIBLE + BS_PUSHBUTTON,
            WM::ScaleX(3),
            WM::ScaleY(3),
            WM::ScaleX(15),
            WM::ScaleY(10),
            WM::Window::getHandle(),
            NULL,
            GetModuleHandle(NULL), 
            NULL);
*/


/*
        CreateWindow(
            TEXT("STATIC"),
            TEXT("Hello world!"),
            WS_CHILD + WS_TABSTOP + WS_VISIBLE,
            WM::ScaleX(3),
            WM::ScaleY(3),
            WM::ScaleX(30),
            WM::ScaleY(10),
            WM::Window::getHandle(),
            NULL,
            GetModuleHandle(NULL), 
            NULL);
*/
