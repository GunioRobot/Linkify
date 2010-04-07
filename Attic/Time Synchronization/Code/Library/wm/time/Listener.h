#pragma once


#include "../Object.h"
#include "../String.h"
#include "DateTime.h"


namespace wm {
namespace time {
    class Listener: public Object {
    public:
        virtual void onStatusChange(String status) = 0;
        virtual void onTimeChange(DateTime time) = 0;
    };
}}
