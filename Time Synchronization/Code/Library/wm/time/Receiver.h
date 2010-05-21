#pragma once


#include "Information.h"
#include "Listener.h"


namespace wm {
namespace time {
    class Receiver: public Information, public Listener {
    };
}}
