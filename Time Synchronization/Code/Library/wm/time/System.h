#pragma once


#include "../Exception.h"
#include "../Timer.h"
#include "Receiver.h"
#include "Sender.h"


namespace wm {
namespace time {
    class System: public Receiver, public Sender, protected Timer {
    public:
        virtual String getDescription() {
            return S("Uses the time from the internal system clock.");
        }


        virtual String getName() {
            return S("System");
        }
        
        
        virtual void finalize() {
            stop();
        }
        
        
        virtual void initialize(bool automatic) {
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
