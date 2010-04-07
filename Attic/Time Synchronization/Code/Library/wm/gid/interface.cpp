#include "interface.h"


#undef __WM__GID__INTERFACE__
#define API_FUNCTION_DEFINITION
#include "interface.h"
#undef API_FUNCTION_DEFINITION

#undef __WM__GID__INTERFACE__
#define API_FUNCTION_LOADER
#include "interface.h"
#undef API_FUNCTION_LOADER

#undef __WM__GID__INTERFACE__
#define API_FUNCTION_UNLOADER
#include "interface.h"
#undef API_FUNCTION_UNLOADER


static HINSTANCE _library = NULL;


namespace wm {
namespace gid {
    HINSTANCE Load() {
        if (_library == NULL) {
            _library = LoadLibrary(TEXT("gpsapi"));
            
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
}}
