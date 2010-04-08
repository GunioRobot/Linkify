#include "Executable.h"


const wm::String Executable::TITLE(S("Time Synchronization"));
const wm::String Executable::WINDOW_CLASS(S("TIME_SYNCHRONIZATION"));


int WINAPI WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPTSTR commandLine,
    int windowShowMode)
{
    return Executable::start(instance, windowShowMode);
}
