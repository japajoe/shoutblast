#ifndef IMGUI_IMGUIMANAGER_HPP
#define IMGUI_IMGUIMANAGER_HPP

struct GLFWwindow;

namespace ImGui::Manager
{
    void Initialize(GLFWwindow *window);
    void Destroy();
    void BeginFrame();
    void EndFrame();
    void SetStyle1();
    void SetStyle2();
    void SetStyle3();
}
#endif