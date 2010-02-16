#include "WM.h"
#include <cmath>


size_t mmToPx(double mm, int pixelsPerInch) {
    return (size_t) ceil(mm * 0.0393700787401575 * pixelsPerInch);
}


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

        HDC screen = GetDC(NULL);
        int xPxPerIn = GetDeviceCaps(screen, LOGPIXELSX);
        int yPxPerIn = GetDeviceCaps(screen, LOGPIXELSY);
        ReleaseDC(NULL, screen);

        CreateWindow(
            TEXT("BUTTON"),   // Predefined class; Unicode assumed.
            TEXT("OK"),       // Button text. 
            WS_CHILD + WS_TABSTOP + WS_VISIBLE + BS_PUSHBUTTON,  // Styles.
            mmToPx(3, xPxPerIn),         // x position. 
            mmToPx(3, yPxPerIn),         // y position. 
            mmToPx(10, xPxPerIn),        // Button width.
            mmToPx(10, yPxPerIn),        // Button height.
            WM::Window::getHandle(),       // Parent window.
            NULL,       // No menu.
            GetModuleHandle(NULL), 
            NULL);      // Pointer not needed.
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
