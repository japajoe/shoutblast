#include "imgui_manager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../../glfw/include/GLFW/glfw3.h"
#include "../../../include/Embedded/RobotoRegular.hpp"
#include "../../../include/Embedded/ImGuiIni.hpp"
#include "../../../include/Utilities/File.hpp"
#include <cstring>

namespace ImGui::Manager
{
    void Initialize(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_Init("#version 300 es");
    #else
        ImGui_ImplOpenGL3_Init("#version 150");
    #endif
        
        io.Fonts->AddFontDefault();

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        std::strcpy(fontConfig.Name, "Roboto Regular");
        void *pFontData = (void*)ShoutBlast::GetFontData();
        io.FontDefault = io.Fonts->AddFontFromMemoryTTF(pFontData, ShoutBlast::GetFontSize(), 15.0f, &fontConfig);

        if(!ShoutBlast::File::Exists("imgui.ini"))
            ImGui::LoadIniSettingsFromMemory(ShoutBlast::GetImGuiIniData().data());
    }

    void Destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void BeginFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void EndFrame()
    {
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    static ImVec4 AdjustHueAndSaturation(const ImVec4& Color, const float HueShift, const float SaturationScale)
    {
        ImVec4 ResultColor = Color;
        ImGui::ColorConvertRGBtoHSV(ResultColor.x, ResultColor.y, ResultColor.z, ResultColor.x, ResultColor.y, ResultColor.z);

        ResultColor.x += HueShift;
        ResultColor.y *= SaturationScale;

        ImGui::ColorConvertHSVtoRGB(ResultColor.x, ResultColor.y, ResultColor.z, ResultColor.x, ResultColor.y, ResultColor.z);
        return ResultColor;
    }

    void SetStyle1()
    {
        auto &colors = ImGui::GetStyle().Colors;
        auto bg = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
        auto bgHovered = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
        auto bgActive = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
        auto frameBg = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
        auto menuBg = ImVec4(0.10f, 0.11f, 0.11f, 1.00f);
        auto text = ImVec4(0.86f, 0.87f, 0.88f, 1.00f);
        auto grab = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
        auto checkMark = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = bg;
        colors[ImGuiCol_ChildBg] = bg;
        colors[ImGuiCol_PopupBg] = frameBg;
        colors[ImGuiCol_Border] = bg;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
        colors[ImGuiCol_FrameBg] = frameBg;
        colors[ImGuiCol_FrameBgHovered] = bgHovered;
        colors[ImGuiCol_FrameBgActive] = bgActive;
        colors[ImGuiCol_TitleBg] = bg;
        colors[ImGuiCol_TitleBgActive] = bg;
        colors[ImGuiCol_TitleBgCollapsed] = bg;
        colors[ImGuiCol_MenuBarBg] = menuBg;
        colors[ImGuiCol_ScrollbarBg] = bg;
        colors[ImGuiCol_ScrollbarGrab] = grab;
        colors[ImGuiCol_ScrollbarGrabHovered] = bgHovered;
        colors[ImGuiCol_ScrollbarGrabActive] = bgActive;
        colors[ImGuiCol_CheckMark] = checkMark;
        colors[ImGuiCol_SliderGrab] = checkMark;
        colors[ImGuiCol_SliderGrabActive] = checkMark;
        colors[ImGuiCol_Button] = bg;
        colors[ImGuiCol_ButtonHovered] = bgHovered;
        colors[ImGuiCol_ButtonActive] = bg;
        colors[ImGuiCol_Header] = grab;
        colors[ImGuiCol_HeaderHovered] = bgHovered;
        colors[ImGuiCol_HeaderActive] = bgActive;
        colors[ImGuiCol_Separator] = bg;
        colors[ImGuiCol_SeparatorHovered] = bgHovered;
        colors[ImGuiCol_SeparatorActive] = bgActive;
        colors[ImGuiCol_ResizeGrip] = grab;
        colors[ImGuiCol_ResizeGripHovered] = bgHovered;
        colors[ImGuiCol_ResizeGripActive] = bgActive;
        colors[ImGuiCol_TabHovered] = menuBg;
        colors[ImGuiCol_Tab] = menuBg;
        colors[ImGuiCol_TabSelected] = menuBg;
        colors[ImGuiCol_TabSelectedOverline] = menuBg;
        colors[ImGuiCol_TabDimmed] = menuBg;
        colors[ImGuiCol_TabDimmedSelected] = bg;
        colors[ImGuiCol_TabDimmedSelectedOverline] = menuBg;
        colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = text;
        colors[ImGuiCol_PlotLinesHovered] = bgActive;
        colors[ImGuiCol_PlotHistogram] = text;
        colors[ImGuiCol_PlotHistogramHovered] = bgActive;
        colors[ImGuiCol_TableHeaderBg] = menuBg;
        colors[ImGuiCol_TableBorderStrong] = menuBg;
        colors[ImGuiCol_TableBorderLight] = menuBg;
        colors[ImGuiCol_TableRowBg] = bg;
        colors[ImGuiCol_TableRowBgAlt] = menuBg;
        colors[ImGuiCol_TextLink] = checkMark;
        colors[ImGuiCol_TextSelectedBg] = bgActive;
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = checkMark;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.11f, 0.50f);

        auto &style = ImGui::GetStyle();
        style.FrameBorderSize = 0.0f;
        style.FrameRounding = 2.0f;
        style.WindowBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 2.0f;
        style.GrabMinSize = 7.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 2.0f;

        style.WindowPadding = ImVec2(5.0f, 5.0f);
        style.FramePadding = ImVec2(4.0f, 3.0f);
        style.ItemSpacing = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.TabBarBorderSize = 0;
        style.WindowBorderSize = 0;
    }

    void SetStyle2()
    {
        ImGuiStyle& Style = ImGui::GetStyle();

        const float Hue = 0.0f; // [0,1] range.
        const float Saturation = 1.0f; // [0,6] range.
        const float SaturationAccent = 1.0f; // [0,6] range.
        const float Transparency = 0.95f;
        const float BorderSize = 0.0f;

        Style.FrameBorderSize = BorderSize;
        Style.ImageBorderSize = BorderSize;
        Style.TabBorderSize = BorderSize;
        Style.TabBarBorderSize = 3.0f;
        Style.WindowRounding = 4.0f;
        Style.ChildRounding = 4.0f;
        Style.FrameRounding = 4.0f;
        Style.GrabRounding = 4.0f;
        Style.TabRounding = 4.0f;

        ImVec4 TextColor{1.000f, 1.000f, 1.000f, 1.000f};
        ImVec4 TextDimmedColor{0.357f, 0.482f, 0.549f, 1.000f};
        ImVec4 BackroundColor{0.110f, 0.149f, 0.169f, 1.000f};
        ImVec4 BackroundChildColor{0.090f, 0.122f, 0.141f, 1.000f};
        ImVec4 BackroundDimmedColor{0.000f, 0.000f, 0.000f, 0.600f};
        ImVec4 TitleColor{0.078f, 0.102f, 0.122f, 1.000f};
        ImVec4 HeaderColor{0.184f, 0.247f, 0.286f, 1.000f};
        ImVec4 Accent1Color{0.000f, 0.490f, 1.000f, 1.000f};
        ImVec4 Accent2Color{0.000f, 0.412f, 0.824f, 1.000f};
        ImVec4 Accent1AlternativeColor{0.302f, 0.408f, 0.475f, 1.000f};
        ImVec4 Accent2AlternativeColor{0.251f, 0.337f, 0.392f, 1.000f};
        ImVec4 TransparentColor{0.0f, 0.0f, 0.0f, 0.0f};

        TextColor = AdjustHueAndSaturation(TextColor, Hue, Saturation);
        TextDimmedColor = AdjustHueAndSaturation(TextDimmedColor, Hue, Saturation);
        BackroundColor = AdjustHueAndSaturation(BackroundColor, Hue, Saturation);
        BackroundChildColor = AdjustHueAndSaturation(BackroundChildColor, Hue, Saturation);
        BackroundDimmedColor = AdjustHueAndSaturation(BackroundDimmedColor, Hue, Saturation);
        TitleColor = AdjustHueAndSaturation(TitleColor, Hue, Saturation);
        HeaderColor = AdjustHueAndSaturation(HeaderColor, Hue, Saturation);
        Accent1Color = AdjustHueAndSaturation(Accent1Color, Hue, SaturationAccent);
        Accent2Color = AdjustHueAndSaturation(Accent2Color, Hue, SaturationAccent);
        Accent1AlternativeColor = AdjustHueAndSaturation(Accent1AlternativeColor, Hue, SaturationAccent);
        Accent2AlternativeColor = AdjustHueAndSaturation(Accent2AlternativeColor, Hue, SaturationAccent);

        ImVec4 BackroundTransparentColor = BackroundColor;
        BackroundTransparentColor.w *= Transparency;

        ImVec4 BackroundChildTransparentColor = BackroundChildColor;
        BackroundChildTransparentColor.w *= Transparency;

        ImVec4 TitleTransparentColor = TitleColor;
        TitleTransparentColor.w *= Transparency;

        ImVec4 HeaderTransparentColor = HeaderColor;
        HeaderTransparentColor.w *= Transparency;

        Style.Colors[ImGuiCol_Text] = TextColor;
        Style.Colors[ImGuiCol_TextDisabled] = TextDimmedColor;
        Style.Colors[ImGuiCol_WindowBg] = BackroundTransparentColor;
        Style.Colors[ImGuiCol_ChildBg] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_PopupBg] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_Border] = TitleColor;
        Style.Colors[ImGuiCol_BorderShadow] = TransparentColor;
        Style.Colors[ImGuiCol_FrameBg] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_FrameBgHovered] = Accent1AlternativeColor;
        Style.Colors[ImGuiCol_FrameBgActive] = Accent2AlternativeColor;
        Style.Colors[ImGuiCol_TitleBg] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_TitleBgActive] = TitleTransparentColor;
        Style.Colors[ImGuiCol_TitleBgCollapsed] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_MenuBarBg] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_ScrollbarBg] = TitleTransparentColor;
        Style.Colors[ImGuiCol_ScrollbarGrab] = HeaderColor;
        Style.Colors[ImGuiCol_ScrollbarGrabHovered] = Accent1AlternativeColor;
        Style.Colors[ImGuiCol_ScrollbarGrabActive] = Accent2AlternativeColor;
        Style.Colors[ImGuiCol_CheckMark] = Accent1Color;
        Style.Colors[ImGuiCol_SliderGrab] = Accent1Color;
        Style.Colors[ImGuiCol_SliderGrabActive] = Accent2Color;
        Style.Colors[ImGuiCol_Button] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_ButtonHovered] = Accent1Color;
        Style.Colors[ImGuiCol_ButtonActive] = Accent2Color;
        Style.Colors[ImGuiCol_Header] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_HeaderHovered] = Accent1Color;
        Style.Colors[ImGuiCol_HeaderActive] = Accent2Color;
        Style.Colors[ImGuiCol_Separator] = HeaderColor;
        Style.Colors[ImGuiCol_SeparatorHovered] = Accent1Color;
        Style.Colors[ImGuiCol_SeparatorActive] = Accent2Color;
        Style.Colors[ImGuiCol_ResizeGrip] = HeaderColor;
        Style.Colors[ImGuiCol_ResizeGripHovered] = Accent1Color;
        Style.Colors[ImGuiCol_ResizeGripActive] = Accent2Color;
        Style.Colors[ImGuiCol_InputTextCursor] = TextColor;
        Style.Colors[ImGuiCol_TabHovered] = Accent1Color;
        Style.Colors[ImGuiCol_Tab] = TransparentColor;
        Style.Colors[ImGuiCol_TabSelected] = HeaderTransparentColor;
        Style.Colors[ImGuiCol_TabSelectedOverline] = TransparentColor;
        Style.Colors[ImGuiCol_TabDimmed] = TransparentColor;
        Style.Colors[ImGuiCol_TabDimmedSelected] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_TabDimmedSelectedOverline] = TransparentColor;
        Style.Colors[ImGuiCol_DockingPreview] = HeaderColor;
        Style.Colors[ImGuiCol_DockingEmptyBg] = BackroundTransparentColor;
        Style.Colors[ImGuiCol_PlotLines] = TextDimmedColor;
        Style.Colors[ImGuiCol_PlotLinesHovered] = Accent1Color;
        Style.Colors[ImGuiCol_PlotHistogram] = TextDimmedColor;
        Style.Colors[ImGuiCol_PlotHistogramHovered] = Accent1Color;
        Style.Colors[ImGuiCol_TableHeaderBg] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_TableBorderStrong] = TitleColor;
        Style.Colors[ImGuiCol_TableBorderLight] = HeaderColor;
        Style.Colors[ImGuiCol_TableRowBg] = BackroundTransparentColor;
        Style.Colors[ImGuiCol_TableRowBgAlt] = BackroundChildTransparentColor;
        Style.Colors[ImGuiCol_TextLink] = Accent1Color;
        Style.Colors[ImGuiCol_TextSelectedBg] = Accent1Color;
        Style.Colors[ImGuiCol_TreeLines] = TextDimmedColor;
        Style.Colors[ImGuiCol_DragDropTarget] = Accent1Color;
        Style.Colors[ImGuiCol_DragDropTargetBg] = TransparentColor;
        Style.Colors[ImGuiCol_UnsavedMarker] = Accent1Color;
        Style.Colors[ImGuiCol_NavCursor] = Accent1Color;
        Style.Colors[ImGuiCol_NavWindowingHighlight] = Accent1Color;
        Style.Colors[ImGuiCol_NavWindowingDimBg] = BackroundDimmedColor;
        Style.Colors[ImGuiCol_ModalWindowDimBg] = BackroundDimmedColor;
    }

    void SetStyle3()
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        ImVec4* colors = style->Colors;

        colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
        colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
        colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(1.000f, 0.391f, 0.000f, 0.781f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

        style->ChildRounding = 4.0f;
        style->FrameBorderSize = 1.0f;
        style->FrameRounding = 2.0f;
        style->GrabMinSize = 7.0f;
        style->PopupRounding = 2.0f;
        style->ScrollbarRounding = 12.0f;
        style->ScrollbarSize = 13.0f;
        style->TabBorderSize = 1.0f;
        style->TabRounding = 0.0f;
        style->WindowRounding = 4.0f;
    }
}