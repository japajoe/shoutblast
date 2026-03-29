#include "Time.hpp"
#include <chrono>

namespace ShoutBlast
{
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimePoint;
    static TimePoint gTP1 = std::chrono::system_clock::now();
    static TimePoint gTP2 = std::chrono::system_clock::now();
    static float gDeltaTime = 0.0f;
    static float gElapsed = 0.0f;

    void Time::NewFrame()
    {
        gTP2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = gTP2 - gTP1;
        gTP1 = gTP2;
        gDeltaTime = elapsed.count();
        gElapsed += gDeltaTime;
    }

    float Time::GetDeltaTime()
    {
        return gDeltaTime;
    }

    float Time::GetElapsed()
    {
        return gElapsed;
    }
}