#ifndef SHOUTBLAST_AUDIORINGBUFFER_HPP
#define SHOUTBLAST_AUDIORINGBUFFER_HPP

#include <vector>
#include <mutex>
#include <algorithm>
#include <cstdint>
#include <cstdlib>

namespace ShoutBlast
{
    class AudioRingBuffer
    {
    public:
        AudioRingBuffer(size_t size);
        ~AudioRingBuffer();
        int GetCount();
        void Reset();
        void Write(const uint8_t* data, int offset, int length);
        int Read(void* destination, int length);
    private:
        std::vector<uint8_t> buffer;
        int readIndex;
        int writeIndex;
        int count;
        int mask;
        std::mutex lockObject;
    };
}

#endif