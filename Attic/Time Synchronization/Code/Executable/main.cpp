#include "Executable.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        Wm::Window::createClass(Executable::WINDOW_CLASS);

        if (Wm::Window::exists(Executable::WINDOW_CLASS, Executable::TITLE)) {
            return EXIT_SUCCESS;
        }
        
        return Executable(instance).start(windowShowMode);
    }
    catch (Wm::Exception& exception) {
        Wm::Application::exit(exception);
        return EXIT_FAILURE;
    }
}
