#pragma once


#include <vector>
#include "TimeListener.h"
#include "TimeListeners.h"
#include "TimeSource.h"


class TimeMultiplexer: public TimeListener, public TimeSource {
private:
    static struct Sample {
        Wm::DateTime time;
        DWORD tickCount;

        Sample(Wm::DateTime time): time(time), tickCount(GetTickCount()) {
        }
    };


private:
    bool _active;
    size_t _samplingInterval;
    ref<TimeListeners> _listeners;
    ref<Wm::Mutex> _modifications;
    std::vector<Sample> _samples;


public:
    TimeMultiplexer(size_t samplingInterval):
        _active(false),
        _samplingInterval(samplingInterval),
        _listeners(new TimeListeners()),
        _modifications(new Wm::Mutex())
    {
    }


    virtual void addListener(ref<TimeListener> listener) {
        _listeners->addListener(listener);
    }


    virtual void onStatusChange(Wm::String status) {
        if (_active) {
            _listeners->onStatusChange(status);
        }
    }


    virtual void onTimeChange(Wm::DateTime time) {
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


    virtual void removeListener(ref<TimeListener> listener) {
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
    Wm::DateTime multiplex(std::vector<Sample> samples) {
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
