#ifndef SHOUTBLAST_AUDIOBUFFER_HPP
#define  SHOUTBLAST_AUDIOBUFFER_HPP

#include <vector>
#include <mutex>
#include <atomic>

namespace ShoutBlast 
{
    class AudioBuffer
    {
    public:
        AudioBuffer();
        void SetData(const float* data, uint32_t frameCount, uint32_t channels);
        void GetData(std::vector<float>& outBuffer);
    private:
        std::vector<float> buffer;
        uint32_t capacity;
        std::atomic<uint32_t> writeIndex;
        // A simple atomic flag used as a spinlock
        std::atomic_flag lock = ATOMIC_FLAG_INIT;
    };
}

#endif