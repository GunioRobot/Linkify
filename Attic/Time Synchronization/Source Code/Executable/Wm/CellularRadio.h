#pragma once


#include <map>
#include "Application.h"
#include "AsynchronousResult.h"
#include "Exception.h"
#include "Object.h"


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


        static void throwError(HRESULT result) {
            const TCHAR* message = Api::Ril::LoadErrorMessage(result);

            if (message != NULL) {
                throw Exception(message);
            }

            SetLastError(HRESULT_CODE(result));
            Exception::throwLastError();
        }


    private:
        HRIL _handle;
        bool _radioPresent;
        std::map<HRESULT, ref<AsynchronousResult>> _results;


    public:
        CellularRadio(): _handle(NULL), _radioPresent(false) {
            if ((_references == 0) && (Api::Ril::Load() == NULL)) {
                Exception::throwLastError();
            }
            
            ++_references;

            HRESULT result = Api::Ril::RIL_Initialize(_PORT,
                genericResultHandler, genericNotifyHandler,
                RIL_NCLASS_ALL, (DWORD) this, &_handle);

            if (FAILED(result)) {
                throwError(result);
            }

            _radioPresent = (result == S_OK);
        }


        virtual ~CellularRadio() {
            HRESULT result = Api::Ril::RIL_Deinitialize(_handle);

            if ((--_references == 0) && !Api::Ril::Unload()) {
                Exception::throwLastError();
            }

            if (FAILED(result)) {
                throwError(result);
            }
        }


        virtual HRIL getHandle() {
            return _handle;
        }


        virtual ref<AsynchronousResult> getLockingStatus(
            DWORD facility,
            String password)
        {
            char* pass = password.toCharArray();
            ref<AsynchronousResult> result = new AsynchronousResult();
            
            HRESULT id = Api::Ril::RIL_GetLockingStatus(getHandle(),
                facility, pass);

            delete[] pass;

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = result;
            return result;
        }


        virtual ref<AsynchronousResult> getPhoneLockedState() {
            ref<AsynchronousResult> result = new AsynchronousResult();
            HRESULT id = Api::Ril::RIL_GetPhoneLockedState(getHandle());

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = result;
            return result;
        }


        virtual ref<AsynchronousResult> getSystemTime() {
            ref<AsynchronousResult> result = new AsynchronousResult();
            HRESULT id = Api::Ril::RIL_GetSystemTime(getHandle());

            if (FAILED(id)) {
                throwError(id);
            }
            
            _results[id] = result;
            return result;
        }


        virtual bool isRadioPresent() {
            return _radioPresent;
        }


        virtual ref<AsynchronousResult> queryFeatures(DWORD type) {
            ref<AsynchronousResult> result = new AsynchronousResult();
            HRESULT id = Api::Ril::RIL_GetDevCaps(getHandle(), type);

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = result;
            return result;
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
                    throw Exception(S("Unknown RIL radio presence state."));
                }
            }
        }


        void CALLBACK resultHandler(
            DWORD code,
            HRESULT id,
            const void* data,
            DWORD size)
        {
            std::map<HRESULT, ref<AsynchronousResult>>::iterator it =
                _results.find(id);
            
            if (it == _results.end()) {
                throw Exception(S("Unexpected asynchronous RIL response."));
            }
            
            ref<AsynchronousResult> result = (*it).second;
            _results.erase(id);
            result->setRawValue(data, size);
        }
    };
}
