#include "rilWrapper.h"


#undef _RILWRAPPER_H_
#define RIL_FUNCTION_DEFINITIONS
#include "rilWrapper.h"
#undef RIL_FUNCTION_DEFINITIONS

#undef _RILWRAPPER_H_
#define RIL_FUNCTION_LOADER
#include "rilWrapper.h"
#undef RIL_FUNCTION_LOADER

#undef _RILWRAPPER_H_
#define RIL_FUNCTION_UNLOADER
#include "rilWrapper.h"
#undef RIL_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


namespace WM {
    bool RIL_Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("ril"));
            
            if (_library != NULL) {
                loadFunctions(_library);
            }
        }

        return _library != NULL;
    }


    bool RIL_Unload() {
        if (_library != NULL) {
            if (FreeLibrary(_library) == 0) {
                return false;
            }
            
            _library = NULL;
            unloadFunctions();
        }

        return true;
    }
}
