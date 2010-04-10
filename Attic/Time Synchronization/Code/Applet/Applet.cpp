#include <wm/Log.h>


extern "C"
BOOL APIENTRY DllMain(HANDLE module, DWORD  mode, LPVOID reserved) {
    wm::Log::error(TEXT("##### DLL\n"));
    return TRUE;
}


extern "C" __declspec(dllexport) 
LONG APIENTRY CPlApplet(HWND window, UINT message, LONG param1, LONG param2) {
    wm::Log::error(TEXT("##### CPL\n"));
    return 1;
}
