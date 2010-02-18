#include "Window.h"


namespace WM {
    std::map<HWND, ref<Window::State>> Window::_windows;
}
