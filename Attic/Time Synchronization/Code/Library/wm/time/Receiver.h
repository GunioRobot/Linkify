#pragma once


#include "Information.h"
#include "Listener.h"


namespace Wm {
namespace Time {
    class Receiver: public Information, public Listener {
    };
}}
