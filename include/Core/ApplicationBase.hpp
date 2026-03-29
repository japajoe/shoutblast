#ifndef SHOUTBLAST_APPLICATIONBASE_HPP
#define SHOUTBLAST_APPLICATIONBASE_HPP

#include <cstdint>
#include <memory>

struct GLFWwindow;

namespace ShoutBlast
{
    class GraphicsContext;
    
	class ApplicationBase
	{
	public:
		ApplicationBase();
		ApplicationBase(const char *title, uint32_t width, uint32_t height, bool vsync = true);
		ApplicationBase(const ApplicationBase &other) = delete;
		ApplicationBase(ApplicationBase &&other) noexcept = delete;
		ApplicationBase &operator=(const ApplicationBase &other) = delete;
		ApplicationBase &operator=(ApplicationBase &&other) noexcept = delete;
		virtual ~ApplicationBase();
		virtual void OnLoad() {};
		virtual void OnDestroy() {};
		virtual void OnUpdate() {};
		virtual void OnLateUpdate() {};
		virtual void OnGUI() {};
		bool Run();
		void Close();
		static void SetTitle(const char *title);
		static void Quit();
		static GLFWwindow *GetNativeWindow();
	private:
        std::unique_ptr<GraphicsContext> graphicsContext;
		static void PumpEvents();
		static void WindowResizeCallback(GLFWwindow *window, int32_t width, int32_t height);
		static void KeyPressCallback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
		static void CharPressCallback(GLFWwindow *window, uint32_t codepoint);
		static void MouseButtonPressCallback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods);
		static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
		static void ErrorCallback(int32_t errorCode, const char *description);
	};
}

#endif