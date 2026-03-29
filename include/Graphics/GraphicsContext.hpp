#ifndef SHOUTBLAST_GRAPHICSCONTEXT_HPP
#define SHOUTBLAST_GRAPHICSCONTEXT_HPP

#include <cstdint>

namespace ShoutBlast
{
    class GraphicsContext
    {
    public:
        bool Initialize(uint32_t screenWidth, uint32_t screenHeight);
        void Destroy();
        void NewFrame();
        void BeginGUI();
        void EndGUI();
        void SetScreenSize(uint32_t width, uint32_t height);
    private:
        uint32_t screenWidth;
        uint32_t screenHeight;
    };
}

#endif