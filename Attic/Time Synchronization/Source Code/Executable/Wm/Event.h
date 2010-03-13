#pragma once


#include "Exception.h"
#include "Object.h"


namespace Wm {
    class Event: public Object {
    private:
        HANDLE _handle;
        bool _valueCopy;


    public:
        Event(bool automatic = false):
            _handle(CreateEvent(NULL, !automatic, false, NULL)),
            _valueCopy(false)
        {
            if (_handle == NULL) {
                Exception::throwLastError();
            }
        }


        virtual ~Event() {
            if (_valueCopy) {
                delete getValue();
            }

            if (!CloseHandle(_handle)) {
                Exception::throwLastError();
            }
        }


        virtual HANDLE getHandle() {
            return _handle;
        }


        virtual void* getValue() {
            SetLastError(ERROR_SUCCESS);
            DWORD value = GetEventData(getHandle());

            if ((value == 0) && (GetLastError() != ERROR_SUCCESS)) {
                Exception::throwLastError();
            }

            return (void*) value;
        }


        virtual void reset() {
            if (!ResetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void set() {
            if (!SetEvent(getHandle())) {
                Exception::throwLastError();
            }
        }


        virtual void setValue(void* value) {
            void* previous = _valueCopy ? getValue() : NULL;

            if (!SetEventData(getHandle(), (DWORD) value)) {
                Exception::throwLastError();
            }
            
            if (_valueCopy) {
                delete previous;
                _valueCopy = false;
            }
        }


        virtual void setValue(void* value, size_t size) {
            bool doCopy = (value != NULL) && (size > 0);
            void* copy = NULL;
            
            if (doCopy) {
                copy = new BYTE[size];
                memcpy(copy, value, size);
            }

            try {
                setValue(copy);
                _valueCopy = doCopy;
            }
            catch (Exception) {
                if (doCopy) {
                    delete copy;
                }
                throw;
            }
        }


        virtual bool wait(DWORD ms = INFINITE) {
            DWORD result = WaitForSingleObject(getHandle(), ms);

            if (result == WAIT_FAILED) {
                Exception::throwLastError();
            }

            return result != WAIT_TIMEOUT;
        }
    };
}
