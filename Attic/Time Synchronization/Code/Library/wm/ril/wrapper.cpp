#include "wrapper.h"


#undef __WM__RIL__WRAPPER__
#define API_FUNCTION_DEFINITION
#include "wrapper.h"
#undef API_FUNCTION_DEFINITION

#undef __WM__RIL__WRAPPER__
#define API_FUNCTION_LOADER
#include "wrapper.h"
#undef API_FUNCTION_LOADER

#undef __WM__RIL__WRAPPER__
#define API_FUNCTION_UNLOADER
#include "wrapper.h"
#undef API_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


namespace Wm {
namespace Api {
namespace Ril {
    HINSTANCE Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("ril"));
            
            if (_library != NULL) {
                LoadFunctions(_library);
            }
        }
        
        return _library;
    }
    
    
    bool Unload() {
        if (_library != NULL) {
            if (FreeLibrary(_library) == 0) {
                return false;
            }
            
            _library = NULL;
            UnloadFunctions();
        }
        
        return true;
    }
}}}
