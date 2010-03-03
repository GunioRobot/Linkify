#pragma once


#include "Wm.h"


class TimeSource: public Wm::Object {
public:
    virtual Wm::String getDescription() = 0;
    virtual Wm::String getName() = 0;
    virtual Wm::String getStatus() = 0;
    virtual SYSTEMTIME getTime() = 0;
    virtual bool isReady() = 0;
};
