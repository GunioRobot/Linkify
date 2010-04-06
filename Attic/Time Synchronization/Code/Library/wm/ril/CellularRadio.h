#pragma once


#include <map>
#include "../Application.h"
#include "../Event.h"
#include "../Exception.h"
#include "../Object.h"
#include "../Result.h"
#include "messages.h"
#include "wrapper.h"


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
            try {
                ((CellularRadio*) userData)->notifyHandler(code, data, size);
            }
            catch (Exception& exception) {
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
            try {
                ((CellularRadio*) userData)->resultHandler(code, command,
                    data, size);
            }
            catch (Exception& exception) {
                Application::exit(exception);
            }
        }


        static void throwError(HRESULT result) {
            const TCHAR* message = Api::Ril::GetErrorMessage(result);

            if (message != NULL) {
                throw Exception(message);
            }

            Exception::throwError(HRESULT_CODE(result));
        }


    private:
        HRIL _handle;
        bool _radioPresent;
        std::map<HRESULT, ref<Event>> _results;


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


        template<typename T>
        ref<Result<T>> getCapabilities(DWORD type) {
            ref<Event> event = new Event();
            HRESULT id = Api::Ril::RIL_GetDevCaps(getRilHandle(), type);

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = event;
            return new Result<T>(event);
        }


        virtual ref<Result<RILEQUIPMENTSTATE>> getEquipmentState() {
            ref<Event> event = new Event();
            HRESULT id = Api::Ril::RIL_GetEquipmentState(getRilHandle(), NULL);

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = event;
            return new Result<RILEQUIPMENTSTATE>(event);
        }


        virtual HRIL getRilHandle() {
            return _handle;
        }


        virtual ref<Result<DWORD>> getLockingStatus(
            DWORD facility,
            String password)
        {
            ref<Event> event = new Event();
            char* passwordArray = password.toCharArray();
            
            HRESULT id = Api::Ril::RIL_GetLockingStatus(getRilHandle(),
                facility, passwordArray);

            delete[] passwordArray;

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = event;
            return new Result<DWORD>(event);
        }


        virtual ref<Result<DWORD>> getPhoneLockedState() {
            ref<Event> event = new Event();
            HRESULT id = Api::Ril::RIL_GetPhoneLockedState(getRilHandle());

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = event;
            return new Result<DWORD>(event);
        }


        virtual ref<Result<SYSTEMTIME>> getSystemTime() {
            ref<Event> event = new Event();
            HRESULT id = Api::Ril::RIL_GetSystemTime(getRilHandle());

            if (FAILED(id)) {
                throwError(id);
            }
            
            _results[id] = event;
            return new Result<SYSTEMTIME>(event);
        }


        virtual bool isRadioPresent() {
            return _radioPresent;
        }


        virtual ref<Waitable> setEquipmentState(DWORD state) {
            ref<Event> event = new Event();
            HRESULT id = Api::Ril::RIL_SetEquipmentState(getRilHandle(), state);

            if (FAILED(id)) {
                throwError(id);
            }

            _results[id] = event;
            return event;
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
            std::map<HRESULT, ref<Event>>::iterator it = _results.find(id);
            
            if (it == _results.end()) {
                throw Exception(S("Unexpected asynchronous RIL response."));
            }
            
            ref<Event> event = (*it).second;
            
            _results.erase(id);
            event->setValue((void*) data, size);
            event->notify();
        }
    };
}
