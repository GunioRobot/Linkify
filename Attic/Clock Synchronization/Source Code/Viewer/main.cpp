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
        
        ref<WM::Menu> mainMenu = new WM::Menu(S("Menu"));
        ref<WM::Label> localTime = new WM::Label(S("Local time:"));

        mainMenu->add(_updateOption);
        mainMenu->add(_exitOption);
        
        enableMenuBar(mainMenu);
    }


    virtual void choose(ref<WM::MenuItem> item) {
        if (item == _exitOption) {
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
