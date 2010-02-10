#include "Win32.h"


namespace Win32 {
    std::map<HWND, ref<Window::State>> Window::_windows;
    

    void ErrorMessageBox(ref<tstring> message) {
        MessageBox(NULL, message->c_str(), NULL, MB_OK + MB_ICONERROR);
    }


    ref<tstring> GetLastErrorMessage() {
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

        ref<tstring> message = (length == 0) ? NULL : new tstring(buffer);
        LocalFree(buffer);
        return message;
    }

    
    ref<tstring> LoadStringT(UINT id, HINSTANCE module) {
        const size_t BUFFER_SIZE = BUFSIZ;
        TCHAR buffer[BUFFER_SIZE];
        int length = LoadString(module, id, buffer, BUFFER_SIZE);

        if (length == 0) {
            throw Exception(TEXT("Unknown string resource."));
        }

        return new tstring(buffer);
    }
}
