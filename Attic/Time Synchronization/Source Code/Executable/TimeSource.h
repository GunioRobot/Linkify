#pragma once


#include "TimeListener.h"
#include "Wm.h"


class TimeSource: public Wm::Object {
public:
    virtual void finalize() = 0;
    virtual Wm::String getDescription() = 0;
    virtual Wm::String getName() = 0;
    virtual void initialize(ref<TimeListener> listener) = 0;
};
