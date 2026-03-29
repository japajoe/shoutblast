#include "GraphicsContext.hpp"
#include "OpenGL.hpp"
#include "FrameBuffer.hpp"
#include "Shader.hpp"
#include "../Core/Application.hpp"
#include "../../../libs/imgui/include/imgui_manager.h"

namespace ShoutBlast
{
    bool GraphicsContext::Initialize(uint32_t screenWidth, uint32_t screenHeight)
    {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;

        if(OpenGL::Initialize())
        {
            glViewport(0, 0, screenWidth, screenHeight);

            ImGui::Manager::Initialize(Application::GetNativeWindow());
            ImGui::Manager::SetStyle3();

            return true;
        }

        return false;
    }

    void GraphicsContext::Destroy()
    {
        ImGui::Manager::Destroy();
    }

    void GraphicsContext::NewFrame()
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void GraphicsContext::BeginGUI()
    {
        ImGui::Manager::BeginFrame();
    }

    void GraphicsContext::EndGUI()
    {
        ImGui::Manager::EndFrame();
    }

    void GraphicsContext::SetScreenSize(uint32_t width, uint32_t height)
    {
        this->screenWidth = width;
        this->screenHeight = height;
        glViewport(0, 0, width, height);
    }
}