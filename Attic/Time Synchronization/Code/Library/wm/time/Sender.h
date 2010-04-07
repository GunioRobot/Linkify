#pragma once


#include "Information.h"
#include "Listeners.h"
#include "Source.h"


namespace wm {
namespace time {
    class Sender: public Information, public Source {
    private:
        ref<Listeners> _listeners;


    public:
        Sender(): _listeners(new Listeners()) {
        }


        virtual void addListener(ref<Listener> listener) {
            _listeners->addListener(listener);
        }
        
        
        virtual void onFinalize() {
        }


        virtual void onInitialize(bool automatic) {
        }
        
        
        virtual void removeListener(ref<Listener> listener) {
            _listeners->removeListener(listener);
        }


    protected:
        virtual ref<Listeners> getListeners() {
            return _listeners;
        }
    };
}}
