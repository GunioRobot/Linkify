#include "Win32.h"


namespace Win32 {
    std::map<HWND, Window*> Window::_windows;
    
    
    // TODO: Check when the buffer is too small for the whole string.
    ref<tstring> LoadStringT(UINT id, HINSTANCE module) {
        const size_t BUFFER_SIZE = 128;
        TCHAR buffer[BUFFER_SIZE];

        int length = LoadString(module, id, buffer, BUFFER_SIZE);
        ref<tstring> string = (length == 0) ? NULL : new tstring(buffer);

        return string;
    }
}
