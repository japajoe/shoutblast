#include "AudioSettings.hpp"

namespace ShoutBlast
{
    uint32_t AudioSettings::GetOutputBufferSize()
    {
        return GetPeriodSize() / GetOutputChannels();
    }

    uint32_t AudioSettings::GetOutputSampleRate()
    {
        return 44100;
    }

    uint32_t AudioSettings::GetOutputChannels()
    {
        return 2;
    }

    uint32_t AudioSettings::GetPeriodSize()
    {
        return 1024;
    }
}