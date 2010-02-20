#include "Executable.h"
#include "WM.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        return Executable(instance).start(windowShowMode);
    }
    catch (WM::Exception exception) {
        WM::Application::error(exception);
        return EXIT_FAILURE;
    }
}
