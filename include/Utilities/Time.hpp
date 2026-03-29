#ifndef SHOUTBLAST_TIME_HPP
#define SHOUTBLAST_TIME_HPP

namespace ShoutBlast
{
    class Time
    {
    friend class ApplicationBase;
    public:
        static float GetDeltaTime();
        static float GetElapsed();
    private:
        static void NewFrame();
    };
}

#endif