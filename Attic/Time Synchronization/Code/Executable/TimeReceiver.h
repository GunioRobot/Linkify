#pragma once


#include "TimeInformation.h"
#include "TimeListener.h"


class TimeReceiver: public TimeInformation, public TimeListener {
};
