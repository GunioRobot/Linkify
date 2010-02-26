#pragma once


#include "Result.h"


namespace Wm {
    class SystemTimeResult: public Result {
    public:
        virtual SYSTEMTIME getValue() {
            return *(SYSTEMTIME*) getRawValue();
        }
    };
}
