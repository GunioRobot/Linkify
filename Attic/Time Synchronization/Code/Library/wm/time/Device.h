#pragma once


#include "../Exception.h"
#include "../Timer.h"
#include "Receiver.h"
#include "Sender.h"


namespace Wm {
namespace Time {
    class Device: public Receiver, public Sender, protected Timer {
    public:
        virtual String getDescription() {
            return S("Uses the local time from the internal clock.");
        }


        virtual String getName() {
            return S("Device");
        }
        
        
        virtual void onFinalize() {
            stop();
        }
        
        
        virtual void onInitialize(bool automatic) {
            onTimeout();
            start(1 * 1000);
        }


        virtual void onStatusChange(String status) {
        }


        virtual void onTimeChange(DateTime time) {
            if (!SetLocalTime(&time)) {
                Exception::throwLastError();
            }
        }


    protected:
        virtual void onTimeout() {
            DateTime time;
            
            GetLocalTime(&time);
            getListeners()->onTimeChange(time);
        }
    };
}}
