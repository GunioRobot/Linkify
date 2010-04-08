#include "Executable.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        wm::Window::createClass(Executable::WINDOW_CLASS);

        if (wm::Window::exists(Executable::WINDOW_CLASS, Executable::TITLE)) {
            return EXIT_SUCCESS;
        }
        
        return Executable(instance).start(windowShowMode);
    }
    catch (wm::Exception& exception) {
        exception.die();
        return EXIT_FAILURE;
    }
}
