#pragma once


#include "Library/Wm.h"


class TimeListener: public Wm::Object {
public:
    virtual void onStatusChange(Wm::String status) = 0;
    virtual void onTimeChange(SYSTEMTIME time) = 0;
};
