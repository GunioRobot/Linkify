#pragma once


#include <vector>
#include "../Mutex.h"
#include "Listener.h"
#include "Listeners.h"
#include "Source.h"


namespace Wm {
namespace Time {
    class Multiplexer: public Listener, public Source {
    private:
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


    private:
        DateTime multiplex(std::vector<Sample> samples) {
            DWORD tickCount = samples.back().tickCount;
            
            if (samples.size() == 1) {
                return samples.front().time;
            }

            std::vector<ULARGE_INTEGER> timeSamples;
            timeSamples.reserve(samples.size());

            for (size_t i = 0; i < samples.size(); ++i) {
                Sample& sample = samples[i];
                DWORD delayMs = tickCount - sample.tickCount;
                ULARGE_INTEGER time = sample.time;
                
                time.QuadPart += delayMs * (10 * 1000);
                timeSamples.push_back(time);
            }

            ULARGE_INTEGER time = timeSamples.back();
            time.QuadPart /= samples.size();
            timeSamples.pop_back();

            for (size_t i = 0; i < timeSamples.size(); ++i) {
                time.QuadPart += timeSamples[i].QuadPart / samples.size();
            }

            DWORD processingDelayMs = GetTickCount() - tickCount;
            time.QuadPart += processingDelayMs * (10 * 1000);

            return time;
        }
    };
}}
