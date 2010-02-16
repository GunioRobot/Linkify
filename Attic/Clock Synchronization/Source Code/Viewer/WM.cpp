#include "WM.h"


namespace WM {
    std::map<HWND, ref<Window::State>> Window::_windows;


    void ErrorMessageBox(ref<String> message) {
        UINT type = MB_OK + MB_ICONERROR;
        MessageBox(GetForegroundWindow(), message->c_str(), NULL, type);
    }


    ref<String> GetLastErrorMessage() {
        DWORD code = GetLastError();
        TCHAR* buffer;
        
        DWORD length = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER + FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            code,
            0,
            (LPTSTR) &buffer,
            0,
            NULL);

        ref<String> message = (length == 0) ? NULL : new String(buffer);
        LocalFree(buffer);
        return message;
    }

    
    ref<String> ToString(const TCHAR* string) {
        return new String(string);
    }
}
