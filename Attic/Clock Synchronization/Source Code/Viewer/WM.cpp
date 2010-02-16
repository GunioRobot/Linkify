#include "WM.h"
#include <cmath>


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


    size_t Scale(double logicalMm, int pxPerLogicalIn) {
        return (size_t) ceil(logicalMm * 0.0393700787401575 * pxPerLogicalIn);
    }


    size_t ScaleX(double logicalMm) {
        static int pxPerLogicalIn = GetDeviceCaps(LOGPIXELSX);
        return Scale(logicalMm, pxPerLogicalIn);
    }


    size_t ScaleY(double logicalMm) {
        static int pxPerLogicalIn = GetDeviceCaps(LOGPIXELSY);
        return Scale(logicalMm, pxPerLogicalIn);
    }

    
    ref<String> ToString(const TCHAR* string) {
        return new String(string);
    }
}
