#pragma once


#include <map>
#include "Application.h"
#include "BoolResult.h"
#include "Exception.h"
#include "Log.h"
#include "Object.h"
#include "Result.h"


namespace Wm {
    class CellularRadio: public Object {
    private:
        static size_t _references;


        static size_t countDevices() {
            size_t devices = 0;
            DEVMGR_DEVICE_INFORMATION device;
            
            HANDLE search = FindFirstDevice(DeviceSearchByLegacyName,
                TEXT("RIL*"), &device);

            if (search == INVALID_HANDLE_VALUE) {
                Exception::throwLastError();
            }
            
            do {
                ++devices;
            }
            while (FindNextDevice(search, &device));

            // Don't check for success or failure, doesn't seem to work.
            FindClose(search);

            return devices;
        }


        static void CALLBACK genericNotifyHandler(
            DWORD code,
            const void* data,
            DWORD size,
            DWORD userData)
        {
            CellularRadio* self = ((CellularRadio*) userData);

            try {
                self->notifyHandler(code, data, size);
            }
            catch (Exception exception) {
                Application::exit(exception);
            }
        }


        static void CALLBACK genericResultHandler(
            DWORD code,
            HRESULT command,
            const void* data,
            DWORD size,
            DWORD userData)
        {
            CellularRadio* self = ((CellularRadio*) userData);
            
            try {
                self->resultHandler(code, command, data, size);
            }
            catch (Exception exception) {
                Application::exit(exception);
            }
        }


    private:
        HRIL _handle;
        bool _radioPresent;
        std::map<HRESULT, ref<Result>> _results;


    public:
        CellularRadio(): _handle(NULL), _radioPresent(false) {
            if ((_references == 0) && !Api::Ril::Load()) {
                throw Exception(S("RIL_Load"));
            }
            
            ++_references;
            size_t devices = countDevices();

            for (size_t i = 1; i <= devices; ++i) {
                HRESULT result = Api::Ril::Initialize(i,
                    genericResultHandler, genericNotifyHandler,
                    RIL_NCLASS_ALL, (DWORD) this, &_handle);

                if (SUCCEEDED(result)) {
                    _radioPresent = (result == S_OK);
                    return;
                }
            }
            
            throw Exception(S("RIL_Initialize"));
        }


        virtual ~CellularRadio() {
            HRESULT result = Api::Ril::Deinitialize(_handle);

            if ((--_references == 0) && !Api::Ril::Unload()) {
                throw Exception(S("RIL_Unload"));
            }

            if (FAILED(result)) {
                throw Exception(S("RIL_Deinitialize"));
            }
        }


        virtual HRIL getHandle() {
            return _handle;
        }


        virtual ref<BoolResult> isNitzSupported() {
            ref<BoolResult> result = new BoolResult(
                RIL_CAPS_NITZ_DISABLED,
                RIL_CAPS_NITZ_ENABLED);

            HRESULT id = Api::Ril::GetDevCaps(getHandle(),
                RIL_CAPSTYPE_NITZNOTIFICATION);

            _results[id] = result;
            
            if (FAILED(id)) {
                _results.erase(id);
                throw Exception(S("RIL_GetDevCaps"));
            }

            return result;
        }


        virtual bool isRadioPresent() {
            return _radioPresent;
        }


    private:
        void CALLBACK notifyHandler(DWORD code, const void* data, DWORD size) {
            if (code == RIL_NOTIFY_RADIOPRESENCECHANGED) {
                switch ((DWORD) data) {
                case RIL_RADIOPRESENCE_NOTPRESENT:
                    _radioPresent = false;
                    break;
                case RIL_RADIOPRESENCE_PRESENT:
                    _radioPresent = true;
                    break;
                }
            }
        }


        void CALLBACK resultHandler(
            DWORD code,
            HRESULT id,
            const void* data,
            DWORD size)
        {
            std::map<HRESULT, ref<Result>>::iterator it = _results.find(id);
            
            if (it != _results.end()) {
                ref<Result> result = (*it).second;
                
                _results.erase(id);
                result->setRawValue(data, size);
            }
        }
    };
}
