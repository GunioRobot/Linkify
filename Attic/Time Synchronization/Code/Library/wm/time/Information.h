#pragma once


#include "../Object.h"
#include "../String.h"


namespace Wm {
namespace Time {
    class Information: public Object {
    public:
        virtual String getDescription() = 0;
        virtual String getName() = 0;
    };
}}
