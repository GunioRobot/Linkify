#include "wrapper.h"
#include "messages.h"


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
    HINSTANCE Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("ril"));
            
            if (_library != NULL) {
                loadFunctions(_library);
            }
        }

        return _library;
    }


    const TCHAR* LoadErrorMessage(HRESULT result) {
        if (HRESULT_FACILITY(result) == FACILITY_RIL) {
            return _messages[HRESULT_CODE(result) & 0xFF];
        }
        else {
            return NULL;
        }
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
