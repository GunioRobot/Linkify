#ifndef _RILWRAPPER_H_
#define _RILWRAPPER_H_


#include "ril.h"


#if defined(RIL_FUNCTION_DEFINITIONS)
#   undef RIL_FUNCTION
#   define RIL_FUNCTION(name, arguments) \
        name##_t name = NULL
#elif defined(RIL_FUNCTION_LOADER)
#   undef RIL_FUNCTION
#   define RIL_FUNCTION(name, arguments) \
        (name = (name##_t) GetProcAddress(library, TEXT(#name)))
#elif defined(RIL_FUNCTION_UNLOADER)
#   undef RIL_FUNCTION
#   define RIL_FUNCTION(name, arguments) \
        (name = NULL)
#else
#   define RIL_FUNCTION(name, arguments) \
        typedef HRESULT (*name##_t) arguments; \
        extern name##_t name
#endif


namespace WM {
    bool RIL_Load();
    bool RIL_Unload();
    
#if defined(RIL_FUNCTION_LOADER)
static void loadFunctions(HINSTANCE library) {
#elif defined(RIL_FUNCTION_UNLOADER)
static void unloadFunctions() {
#endif
    RIL_FUNCTION(RIL_Deinitialize, (HRIL));
    RIL_FUNCTION(RIL_GetDevCaps, (HRIL, DWORD));
    RIL_FUNCTION(RIL_GetSystemTime, (HRIL));
    RIL_FUNCTION(RIL_Initialize, (DWORD, RILRESULTCALLBACK, RILNOTIFYCALLBACK, DWORD, DWORD, HRIL*));
#if defined(RIL_FUNCTION_LOADER) || defined(RIL_FUNCTION_UNLOADER)
}
#endif
}


#endif
