#include "WM.h"
#include <cmath>


#define ORIGINAL_DPI 96


namespace WM {
    std::map<HWND, ref<Window::State>> Window::_windows;


    void ErrorMessageBox(ref<String> message) {
        UINT type = MB_OK + MB_ICONERROR;
        MessageBox(GetForegroundWindow(), message->c_str(), NULL, type);
    }


    int GetDeviceCaps(int item, HDC deviceContext) {
        HDC handle = deviceContext;

        if (deviceContext == NULL) {
            handle = GetDC(NULL);

            if (handle == NULL) {
                throw Exception(GetLastErrorMessage());
            }
        }

        int value = GetDeviceCaps(handle, item);

        if (deviceContext == NULL) {
            ReleaseDC(NULL, handle);
        }

        return value;
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


    size_t ScaleHorizontal(size_t pixels) {
        static int dpi = GetDeviceCaps(LOGPIXELSX);
        return pixels * dpi / ORIGINAL_DPI;
    }


    size_t ScaleVertical(size_t pixels) {
        static int dpi = GetDeviceCaps(LOGPIXELSY);
        return pixels * dpi / ORIGINAL_DPI;
    }

    
    ref<String> ToString(const TCHAR* string) {
        return new String(string);
    }
}
