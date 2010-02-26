#include "wrapper.h"


#undef __WM__API__RIL__WRAPPER__
#define WM_API_RIL_FUNCTION_DEFINITIONS
#include "wrapper.h"
#undef WM_API_RIL_FUNCTION_DEFINITIONS

#undef __WM__API__RIL__WRAPPER__
#define WM_API_RIL_FUNCTION_LOADER
#include "wrapper.h"
#undef WM_API_RIL_FUNCTION_LOADER

#undef __WM__API__RIL__WRAPPER__
#define WM_API_RIL_FUNCTION_UNLOADER
#include "wrapper.h"
#undef WM_API_RIL_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


namespace Wm {
namespace Api {
namespace Ril {
    bool Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("ril"));
            
            if (_library != NULL) {
                loadFunctions(_library);
            }
        }

        return _library != NULL;
    }


    bool Unload() {
        if (_library != NULL) {
            if (FreeLibrary(_library) == 0) {
                return false;
            }
            
            _library = NULL;
            unloadFunctions();
        }

        return true;
    }
}}}
