#pragma once


#include <vector>
#include "../Mutex.h"
#include "Listener.h"
#include "Listeners.h"
#include "Source.h"


namespace Wm {
namespace Time {
    class Multiplexer: public Listener, public Source {
    protected:
        static struct Sample {
            DateTime time;
            DWORD tickCount;

            Sample(DateTime time): time(time), tickCount(GetTickCount()) {
            }
        };


    private:
        bool _active;
        size_t _samplingInterval;
        ref<Listeners> _listeners;
        ref<Mutex> _modifications;
        std::vector<Sample> _samples;


    public:
        Multiplexer(size_t samplingInterval):
            _active(false),
            _samplingInterval(samplingInterval),
            _listeners(new Listeners()),
            _modifications(new Mutex())
        {
        }


        virtual void addListener(ref<Listener> listener) {
            _listeners->addListener(listener);
        }


        virtual void onStatusChange(String status) {
            if (_active) {
                _listeners->onStatusChange(status);
            }
        }


        virtual void onTimeChange(DateTime time) {
            if (_active) {
                _modifications->lock();
                _samples.push_back(Sample(time));

                if (_samples.size() >= _samplingInterval) {
                    std::vector<Sample> samples(_samples);
                    _samples.clear();
                    _modifications->unlock();

                    _listeners->onTimeChange(multiplex(samples));
                }
                else {
                    _modifications->unlock();
                }
            }
        }


        virtual void removeListener(ref<Listener> listener) {
            _listeners->removeListener(listener);
        }


        virtual void start() {
            _active = true;
        }


        virtual void stop() {
            _modifications->lock();
            _samples.clear();
            _modifications->unlock();

            _active = false;
        }


    protected:
        virtual DateTime multiplex(std::vector<Sample> samples) = 0;
    };
}}
