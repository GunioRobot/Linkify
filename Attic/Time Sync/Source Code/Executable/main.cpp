#include "Executable.h"
#include "WM.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        if (WM::Window::exists(TITLE, WINDOW_CLASS)) {
            return EXIT_SUCCESS;
        }
        return Executable(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::Application::error(exception);
        return EXIT_FAILURE;
    }
}
