#include "Exception.h"


namespace WM {
    void ErrorMessageBox(ref<String> message) {
        UINT type = MB_OK + MB_ICONERROR;
        MessageBox(GetForegroundWindow(), message->c_str(), NULL, type);
    }
}
