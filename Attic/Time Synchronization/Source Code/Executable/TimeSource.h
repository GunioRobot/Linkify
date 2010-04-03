#pragma once


#include "Library/Wm.h"
#include "TimeListener.h"


class TimeSource: public Wm::Object {
public:
    virtual void addListener(ref<TimeListener> listener) = 0;
    virtual void removeListener(ref<TimeListener> listener) = 0;
};
