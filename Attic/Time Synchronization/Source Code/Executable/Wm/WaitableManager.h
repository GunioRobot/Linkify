#pragma once


#include <vector>
#include "Exception.h"
#include "Object.h"
#include "Waitable.h"


namespace Wm {
    class WaitableManager: public Object {
    private:
        std::vector<ref<Waitable>> _waitables;


    public:
        virtual void add(ref<Waitable> waitable) {
            if (!waitable.null()) {
                _waitables.push_back(waitable);
            }
        }


        virtual void remove(ref<Waitable> waitable) {
            if (!waitable.null()) {
                std::vector<ref<Waitable>>::iterator it = _waitables.begin();

                for (; it != _waitables.end(); ++it) {
                    if (*it == waitable) {
                        _waitables.erase(it);
                        break;
                    }
                }
            }
        }


        virtual ref<Waitable> wait(DWORD ms = INFINITE) {
            if (_waitables.empty()) {
                return NULL;
            }

            HANDLE* _handles = (HANDLE*)
                _alloca(_waitables.size() * sizeof(HANDLE));

            for (size_t i = 0; i < _waitables.size(); ++i) {
                _handles[i] = _waitables[i]->getWaitableHandle();
            }

            DWORD result = WaitForMultipleObjects(_waitables.size(), _handles,
                false, ms);

            switch (result) {
            case WAIT_FAILED:
                Exception::throwLastError();
            case WAIT_TIMEOUT:
                return NULL;
            default:
                return _waitables[result - WAIT_OBJECT_0];
            }
        }
    };
}
