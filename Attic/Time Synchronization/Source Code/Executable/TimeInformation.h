#pragma once


#include "Wm.h"


class TimeInformation: public Wm::Object {
public:
    virtual Wm::String getDescription() = 0;
    virtual Wm::String getName() = 0;
};
