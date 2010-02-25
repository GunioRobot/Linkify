#pragma once


#include <map>
#include "Exception.h"
#include "Object.h"


namespace WM {
    class Ril: public Object {
    private:
        static const DWORD _PORT = 1;
        static size_t _references;
        static std::map<HRIL, Ril*> _instances;


        static void CALLBACK notifyHandler(
            DWORD code,
            const void* data,
            DWORD dataSize,
            DWORD userData)
        {
        }


        static void CALLBACK resultHandler(
            DWORD code,
            HRESULT command,
            const void* data,
            DWORD dataSize,
            DWORD userData)
        {
        }


    private:
        HRIL _handle;


    public:
        Ril(): _handle(NULL) {
            if ((_references == 0) && !RIL_Load()) {
                throw Exception(S("RIL loading failed."));
            }
            
            ++_references;

            HRESULT result = RIL_Initialize(_PORT,
                resultHandler, notifyHandler, RIL_NCLASS_ALL, 0, &_handle);

            if (FAILED(result)) {
                throw Exception(S("RIL initialization failed."));
            }
        }


        virtual ~Ril() {
            HRESULT result = RIL_Deinitialize(_handle);

            if ((--_references == 0) && !RIL_Unload()) {
                throw Exception(S("RIL unloading failed."));
            }

            if (FAILED(result)) {
                throw Exception(S("RIL finalization failed."));
            }
        }
    };
}
