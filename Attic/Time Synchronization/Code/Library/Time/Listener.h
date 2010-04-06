#pragma once


#include "../DateTime.h"
#include "../Object.h"
#include "../String.h"


namespace Wm {
namespace Time {
    class Listener: public Object {
    public:
        virtual void onStatusChange(String status) = 0;
        virtual void onTimeChange(DateTime time) = 0;
    };
}}
