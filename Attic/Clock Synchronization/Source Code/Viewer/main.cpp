#include "Win32.h"


class Viewer: public Win32::Application, Win32::Window {
private:
    ref<Win32::String> _exitOption;
    ref<Win32::String> _updateOption;


public:
    Viewer(HINSTANCE handle):
        Win32::Application(handle), Win32::Window(S("Viewer"), S("VIEWER")),
        _exitOption(S("Exit")), _updateOption(S("Update"))
    {
        ref<Win32::Menu> menuBar = new Win32::Menu(S("Menu"));

        menuBar->addItem(new Win32::MenuItem(_updateOption));
        menuBar->addItem(new Win32::MenuItem(_exitOption));
        
        enableMenuBar(menuBar);
    }


    virtual void chooseMenuItem(ref<Win32::MenuItem> item) {
        if (item->getCaption() == _exitOption) {
            close();
        }
    }


    virtual int start(int windowShowMode) {
        show(windowShowMode);
        return Win32::Application::start(windowShowMode);
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
    catch (Win32::Exception exception) {
        Win32::ErrorMessageBox(exception.getMessage());
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
