#include "Win32.h"


namespace Win32 {
    std::map<HWND, ref<Window::State>> Window::_windows;


    void ErrorMessageBox(ref<String> message, bool useWindow) {
        HWND owner = useWindow ? GetForegroundWindow() : NULL;
        UINT type = MB_OK + MB_ICONERROR;

        MessageBox(owner, message->c_str(), NULL, type);
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

    
    // TODO: Check when the buffer is too small for the whole string?
    ref<String> LoadStringT(UINT id, HINSTANCE module) {
        const size_t BUFFER_SIZE = BUFSIZ;
        TCHAR buffer[BUFFER_SIZE];
        int length = LoadString(module, id, buffer, BUFFER_SIZE);

        if (length == 0) {
            throw Exception(TEXT("Unknown string resource."));
        }

        return new String(buffer);
    }
}
