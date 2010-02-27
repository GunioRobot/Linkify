#pragma once


#include <map>
#include "Application.h"
#include "Exception.h"
#include "Object.h"
#include "Result.h"


namespace Wm {
    class CellularRadio: public Object {
    private:
        static const DWORD _PORT = 1;
        static size_t _references;


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

            HRESULT result = Api::Ril::Initialize(_PORT,
                genericResultHandler, genericNotifyHandler,
                RIL_NCLASS_ALL, (DWORD) this, &_handle);

            if (FAILED(result)) {
                throw Exception(S("RIL_Initialize"));
            }

            _radioPresent = (result == S_OK);
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


        virtual ref<Result> getNitzSupport() {
            ref<Result> result = new Result();
            HRESULT id = Api::Ril::GetDevCaps(getHandle(),
                RIL_CAPSTYPE_NITZNOTIFICATION);

            if (FAILED(id)) {
                throw Exception(S("RIL_GetDevCaps"));
            }

            _results[id] = result;
            return result;
        }


        virtual ref<Result> getPhoneLockedState() {
            ref<Result> result = new Result();
            HRESULT id = Api::Ril::GetPhoneLockedState(getHandle());

            if (FAILED(id)) {
                throw Exception(S("RIL_GetPhoneLockedState"));
            }

            _results[id] = result;
            return result;
        }


        virtual ref<Result> getSystemTime() {
            ref<Result> result = new Result();
            HRESULT id = Api::Ril::GetSystemTime(getHandle());

            if (FAILED(id)) {
                throw Exception(S("RIL_GetSystemTime"));
            }
            
            _results[id] = result;
            return result;
        }


        virtual bool isRadioPresent() {
            return _radioPresent;
        }


    private:
        void CALLBACK notifyHandler(DWORD code, const void* data, DWORD size) {
            if (code == RIL_NOTIFY_RADIOPRESENCECHANGED) {
                switch (*(DWORD*) data) {
                case RIL_RADIOPRESENCE_NOTPRESENT:
                    _radioPresent = false;
                    break;
                case RIL_RADIOPRESENCE_PRESENT:
                    _radioPresent = true;
                    break;
                default:
                    throw Exception(S("RIL_NOTIFY_RADIOPRESENCECHANGED"));
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
            
            if (it == _results.end()) {
                throw Exception(S("RILRESULTCALLBACK"));
            }
            
            ref<Result> result = (*it).second;
            _results.erase(id);
            result->setRawValue(data, size);
        }
    };
}
