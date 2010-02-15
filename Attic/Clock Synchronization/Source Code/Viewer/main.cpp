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
        ref<WM::Menu> menuBar = new WM::Menu(S("Menu"));

        menuBar->addItem(new WM::MenuItem(_updateOption));
        menuBar->addItem(new WM::MenuItem(_exitOption));
        
        enableMenuBar(menuBar);
    }


    virtual void chooseMenuItem(ref<WM::MenuItem> item) {
        if (item->getCaption() == _exitOption) {
            close();
        }
    }


    virtual int start(int windowShowMode) {
        show(windowShowMode);
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
            L"BUTTON",   // Predefined class; Unicode assumed.
            L"OK",       // Button text. 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles.
            10,         // x position. 
            10,         // y position. 
            100,        // Button width.
            100,        // Button height.
            getHandle(),       // Parent window.
            NULL,       // No menu.
            application, 
            NULL);      // Pointer not needed.
*/
