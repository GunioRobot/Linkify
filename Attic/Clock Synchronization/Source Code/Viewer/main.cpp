#include "resources.h"
#include "Win32.h"


#define SK_EXIT Win32::LoadStringT(IDS_SK_EXIT)
#define SK_UPDATE Win32::LoadStringT(IDS_SK_UPDATE)
#define TITLE Win32::LoadStringT(IDS_TITLE)
#define WINDOW_CLASS Win32::LoadStringT(IDS_WINDOW_CLASS)


class Viewer : public Win32::Application, Win32::Window {
public:
    Viewer(HINSTANCE handle)
        : Win32::Application(handle), Win32::Window(TITLE, WINDOW_CLASS)
    {
        ref<Win32::Menu> optionsMenu = new Win32::Menu(new Win32::String(TEXT("Options")));
        ref<Win32::Menu> menuBar = new Win32::Menu(new Win32::String(TEXT("Menu")));

        optionsMenu->addItem(new Win32::MenuItem(SK_EXIT));
        menuBar->addItem(new Win32::MenuItem(SK_UPDATE));
        menuBar->addItem(optionsMenu);
        
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
