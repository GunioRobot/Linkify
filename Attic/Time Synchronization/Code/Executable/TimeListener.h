#pragma once


#include "Library/all.h"


class TimeListener: public Wm::Object {
public:
    virtual void onStatusChange(Wm::String status) = 0;
    virtual void onTimeChange(Wm::DateTime time) = 0;
};
