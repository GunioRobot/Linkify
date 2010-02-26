#include "Executable.h"
#include "Wm.h"


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    try {
        Wm::Window::createClass(WINDOW_CLASS);

        if (Wm::Window::exists(WINDOW_CLASS, TITLE)) {
            return EXIT_SUCCESS;
        }
        
        return Executable(instance).start(windowShowMode);
    }
    catch (Wm::Exception exception) {
        Wm::Application::exit(exception);
        return EXIT_FAILURE;
    }
}
