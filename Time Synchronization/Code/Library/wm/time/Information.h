#pragma once


#include "../Object.h"
#include "../String.h"


namespace wm {
namespace time {
    class Information: public Object {
    public:
        virtual String getDescription() = 0;
        virtual String getName() = 0;
    };
}}
