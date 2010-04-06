#ifndef __WM__GID__WRAPPER__
#define __WM__GID__WRAPPER__


#include "../Object.h"
#include <COMMON/SDK/INC/gpsapi.h>


#if defined(API_FUNCTION_DEFINITION)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        name##Function name = NULL
#elif defined(API_FUNCTION_LOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        (name = (name##Function) GetProcAddress(library, TEXT(#name)))
#elif defined(API_FUNCTION_UNLOADER)
#   undef API_FUNCTION
#   define API_FUNCTION(return, name, args) \
        (name = NULL)
#else
#   define API_FUNCTION(return ,name, args) \
        typedef return (*name##Function) args; \
        extern name##Function name
#endif


namespace Wm {
namespace Api {
namespace Gps {
    HINSTANCE Load();
    bool Unload();
    
#if defined(API_FUNCTION_LOADER)
static void LoadFunctions(HINSTANCE library) {
#elif defined(API_FUNCTION_UNLOADER)
static void UnloadFunctions() {
#endif
    
    API_FUNCTION(HANDLE, GPSOpenDevice, (HANDLE hNewLocationData, HANDLE hDeviceStateChange, const WCHAR *szDeviceName, DWORD dwFlags));
    API_FUNCTION(DWORD, GPSCloseDevice, (HANDLE hGPSDevice));
    API_FUNCTION(DWORD, GPSGetPosition, (HANDLE hGPSDevice, GPS_POSITION *pGPSPosition, DWORD dwMaximumAge, DWORD dwFlags));
    API_FUNCTION(DWORD, GPSGetDeviceState, (GPS_DEVICE *pGPSDevice));
    
#if defined(API_FUNCTION_LOADER) || defined(API_FUNCTION_UNLOADER)
}
#endif
}}}


#undef API_FUNCTION


#endif
