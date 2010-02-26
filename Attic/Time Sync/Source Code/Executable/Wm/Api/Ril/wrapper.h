#ifndef __WM__API__RIL__WRAPPER__
#define __WM__API__RIL__WRAPPER__


#include "../../Core.h"
#include "ril.h"


#if defined(WM_API_RIL_FUNCTION_DEFINITIONS)
#   undef WM_API_RIL_FUNCTION
#   define WM_API_RIL_FUNCTION(name, arguments) \
        name##_t name = NULL
#elif defined(WM_API_RIL_FUNCTION_LOADER)
#   undef WM_API_RIL_FUNCTION
#   define WM_API_RIL_FUNCTION(name, arguments) \
        (name = reinterpret_cast<name##_t>(GetProcAddress(library, TEXT("RIL_") TEXT(#name))))
#elif defined(WM_API_RIL_FUNCTION_UNLOADER)
#   undef WM_API_RIL_FUNCTION
#   define WM_API_RIL_FUNCTION(name, arguments) \
        (name = NULL)
#else
#   define WM_API_RIL_FUNCTION(name, arguments) \
        typedef HRESULT (*name##_t) arguments; \
        extern name##_t name
#endif


namespace Wm {
namespace Api {
namespace Ril {
    bool Load();
    bool Unload();
    
#if defined(WM_API_RIL_FUNCTION_LOADER)
static void loadFunctions(HINSTANCE library) {
#elif defined(WM_API_RIL_FUNCTION_UNLOADER)
static void unloadFunctions() {
#endif

    WM_API_RIL_FUNCTION(Deinitialize, (HRIL));
    WM_API_RIL_FUNCTION(GetDevCaps, (HRIL, DWORD));
    WM_API_RIL_FUNCTION(GetSystemTime, (HRIL));
    WM_API_RIL_FUNCTION(Initialize, (DWORD, RILRESULTCALLBACK, RILNOTIFYCALLBACK, DWORD, DWORD, HRIL*));

#if defined(WM_API_RIL_FUNCTION_LOADER) || defined(WM_API_RIL_FUNCTION_UNLOADER)
}
#endif
}}}


#endif
