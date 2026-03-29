#include "AudioRingBuffer.hpp"
#include <cstring>

namespace ShoutBlast
{
    AudioRingBuffer::AudioRingBuffer(size_t size)
    {
        int actualSize = 1;
        while (actualSize < size)
        {
            actualSize <<= 1;
        }

        buffer.resize(actualSize);
        mask = actualSize - 1;
        readIndex = 0;
        writeIndex = 0;
        count = 0;
    }

    AudioRingBuffer::~AudioRingBuffer()
    {
    }

    int AudioRingBuffer::GetCount()
    {
        std::lock_guard<std::mutex> lock(lockObject);
        return count;
    }

    void AudioRingBuffer::Reset()
    {
        std::lock_guard<std::mutex> lock(lockObject);
        readIndex = 0;
        writeIndex = 0;
        count = 0;
    }

    void AudioRingBuffer::Write(const uint8_t* data, int offset, int length)
    {
        std::lock_guard<std::mutex> lock(lockObject);
        
        int bufferSize = static_cast<int>(buffer.size());
        int freeSpace = bufferSize - count;
        int bytesToWrite = std::min(length, freeSpace);

        if (bytesToWrite <= 0)
        {
            return;
        }

        int firstPart = std::min(bytesToWrite, bufferSize - writeIndex);
        std::memcpy(&buffer[writeIndex], data + offset, firstPart);

        int secondPart = bytesToWrite - firstPart;
        if (secondPart > 0)
        {
            std::memcpy(&buffer[0], data + offset + firstPart, secondPart);
        }

        writeIndex = (writeIndex + bytesToWrite) & mask;
        count += bytesToWrite;
    }

    int AudioRingBuffer::Read(void* destination, int length)
    {
        std::lock_guard<std::mutex> lock(lockObject);

        int bufferSize = static_cast<int>(buffer.size());
        int bytesToRead = std::min(length, count);

        if (bytesToRead <= 0)
        {
            return 0;
        }

        uint8_t* destPtr = static_cast<uint8_t*>(destination);

        int firstPart = std::min(bytesToRead, bufferSize - readIndex);
        std::memcpy(destPtr, &buffer[readIndex], firstPart);

        int secondPart = bytesToRead - firstPart;
        if (secondPart > 0)
        {
            std::memcpy(destPtr + firstPart, &buffer[0], secondPart);
        }

        readIndex = (readIndex + bytesToRead) & mask;
        count -= bytesToRead;

        return bytesToRead;
    }
}