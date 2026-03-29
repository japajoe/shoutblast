#include <vector>
#include "AudioBuffer.hpp"
#include <AudioSettings.hpp>
#include <cstring>
#include <cmath>
#include <iostream>

namespace ShoutBlast
{
    AudioBuffer::AudioBuffer()
    {
        float durationMs = 100.0f;
        float sampleRate = AudioSettings::GetOutputSampleRate();
        this->capacity = static_cast<uint32_t>(std::ceil(static_cast<float>(sampleRate) * (durationMs / 1000.0f)));
        this->buffer.assign(capacity, 0.0f);
        this->writeIndex.store(0);
    }

    void AudioBuffer::SetData(const float* data, uint32_t frameCount, uint32_t channels)
    {
        // Acquire the lock. If UI is reading, we wait (usually only for a few microseconds)
        while (lock.test_and_set(std::memory_order_acquire)); 

        uint32_t currentWrite = writeIndex.load(std::memory_order_relaxed);

        for (uint32_t i = 0; i < frameCount; ++i)
        {
            float sum = 0.0f;
            for (uint32_t c = 0; c < channels; ++c)
            {
                sum += data[i * channels + c];
            }

            buffer[currentWrite] = sum / static_cast<float>(channels);
            currentWrite = (currentWrite + 1) % capacity;
        }

        writeIndex.store(currentWrite, std::memory_order_release);
        
        // Release the lock
        lock.clear(std::memory_order_release);
    }

    void AudioBuffer::GetData(std::vector<float>& outBuffer)
    {
        if(outBuffer.size() < AudioSettings::GetPeriodSize())
            outBuffer.resize(AudioSettings::GetPeriodSize());

        uint32_t requestedFrames = static_cast<uint32_t>(outBuffer.size());
        if (requestedFrames == 0 || requestedFrames > capacity)
        {
            return;
        }

        // Try to acquire the lock. 
        // If the audio thread is writing, we just skip this UI frame to avoid glitches.
        if (!lock.test_and_set(std::memory_order_acquire))
        {
            uint32_t currentWrite = writeIndex.load(std::memory_order_acquire);
            
            int32_t readStart = (static_cast<int32_t>(currentWrite) - static_cast<int32_t>(requestedFrames) + static_cast<int32_t>(capacity)) % static_cast<int32_t>(capacity);
            uint32_t currentRead = static_cast<uint32_t>(readStart);

            for (uint32_t i = 0; i < requestedFrames; ++i)
            {
                outBuffer[i] = buffer[currentRead];
                currentRead = (currentRead + 1) % capacity;
            }

            lock.clear(std::memory_order_release);
        }
    }
}