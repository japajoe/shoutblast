#ifndef SHOUTBLAST_AUDIOSETTINGS_HPP
#define SHOUTBLAST_AUDIOSETTINGS_HPP

#include <cstdint>

namespace ShoutBlast
{
    class AudioSettings
    {
    public:
        static uint32_t GetOutputBufferSize();
        static uint32_t GetOutputSampleRate();
        static uint32_t GetOutputChannels();
        static uint32_t GetPeriodSize();
    };
}

#endif