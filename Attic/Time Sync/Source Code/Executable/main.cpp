#include "Executable.h"
#include "WM.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        WM::Window::createClass(WINDOW_CLASS);

        if (WM::Window::exists(WINDOW_CLASS, TITLE)) {
            return EXIT_SUCCESS;
        }
        
        return Executable(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::Application::exit(exception);
        return EXIT_FAILURE;
    }
}
