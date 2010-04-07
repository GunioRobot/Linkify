#pragma once


#include "Multiplexer.h"


namespace wm {
namespace time {
    class MeanMultiplexer: public Multiplexer {
    public:
        MeanMultiplexer(size_t samplingInterval = 5):
            Multiplexer(samplingInterval)
        {
        }


    protected:
        virtual DateTime multiplex(std::vector<Sample> samples) {
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
                
                time.QuadPart += delayMs * 1000 * 10;
                timeSamples.push_back(time);
            }

            ULARGE_INTEGER time = timeSamples.back();
            time.QuadPart /= samples.size();
            timeSamples.pop_back();

            for (size_t i = 0; i < timeSamples.size(); ++i) {
                time.QuadPart += timeSamples[i].QuadPart / samples.size();
            }

            DWORD processingDelayMs = GetTickCount() - tickCount;
            time.QuadPart += processingDelayMs * 1000 * 10;

            return time;
        }
    };
}}
