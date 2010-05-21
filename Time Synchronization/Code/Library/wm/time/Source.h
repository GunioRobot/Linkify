#pragma once


#include "../Object.h"
#include "Listener.h"


namespace wm {
namespace time {
    class Source: public Object {
    public:
        virtual void addListener(ref<Listener> listener) = 0;
        virtual void removeListener(ref<Listener> listener) = 0;
    };
}}
