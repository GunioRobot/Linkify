#include "Win32.h"


class Viewer : public Win32::Application, Win32::Window {
public:
    Viewer(HINSTANCE handle)
        : Win32::Application(handle), Win32::Window(S("Viewer"), S("VIEWER"))
    {
        ref<Win32::Menu> menuBar = new Win32::Menu(S("Menu"));

        menuBar->addItem(new Win32::MenuItem(S("Update")));
        menuBar->addItem(new Win32::MenuItem(S("Exit")));
        
        addMenuBar(menuBar);
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

        case IDS_SK_EXIT:
            SendMessage(handle, WM_CLOSE, 0, 0);
            break;
*/
