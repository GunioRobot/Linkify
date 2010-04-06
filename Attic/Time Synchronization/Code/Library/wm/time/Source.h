#pragma once


#include "../Object.h"
#include "Listener.h"


namespace Wm {
namespace Time {
    class Source: public Object {
    public:
        virtual void addListener(ref<Listener> listener) = 0;
        virtual void removeListener(ref<Listener> listener) = 0;
    };
}}
