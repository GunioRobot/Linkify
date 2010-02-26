#pragma once


#include "Result.h"


namespace Wm {
    class BoolResult: public Result {
    private:
        long _falsehood;
        long _truth;


    public:
        BoolResult(long falsehood = false, long truth = true):
            _falsehood(falsehood), _truth(truth)
        {
        }


    public:
        virtual bool getValue() {
            const DWORD value = *(DWORD*) getRawValue();

            if (value == _falsehood) {
                return false;
            }
            else if (value == _truth) {
                return true;
            }

            throw Exception(S("Invalid boolean result."));
        }
    };
}
