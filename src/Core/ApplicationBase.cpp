#include "ApplicationBase.hpp"
#include "../Graphics/GraphicsContext.hpp"
#include "../Graphics/Image.hpp"
#include "../Embedded/Logo.hpp"
#include "../Utilities/Time.hpp"
#include "../../../libs/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>

namespace ShoutBlast
{
	struct Configuration
	{
		GLFWwindow *pWindow;
		ApplicationBase *pApplication;
		uint32_t width;
		uint32_t height;
		bool vsync;
		double deltaTime;
		double lastTime;
		std::string title;
		std::vector<uint8_t> cursorPixels;
		GLFWcursor* cursor = nullptr;
	};

	static Configuration gConfig = {0};

	static GLFWwindow *CreateWindow(uint32_t width, uint32_t height, const char *title, GLFWwindow *shared)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	#if defined(__APPLE__)
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#else
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#endif
		glfwWindowHint(GLFW_SAMPLES, 4);

		if(shared)
			glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		return glfwCreateWindow(width, height, title, nullptr, shared);
	}

	ApplicationBase::ApplicationBase()
	{
		gConfig.pApplication = this;
		gConfig.title = "ShoutBlast";
		gConfig.pWindow = nullptr;
		gConfig.width = 512;
		gConfig.height = 512;
		gConfig.deltaTime = 0.0;
		gConfig.lastTime = 0.0;
		gConfig.vsync = true;
	}

	ApplicationBase::ApplicationBase(const char *title, uint32_t width, uint32_t height, bool vsync)
	{
		gConfig.pApplication = this;
		gConfig.title = title;
		gConfig.pWindow = nullptr;
		gConfig.width = width;
		gConfig.height = height;
		gConfig.deltaTime = 0.0;
		gConfig.lastTime = 0.0;
		gConfig.vsync = vsync;
	}

	ApplicationBase::~ApplicationBase()
	{
		gConfig.pApplication = nullptr;
	}

	bool ApplicationBase::Run()
	{
		if(gConfig.pWindow)
		{
			std::cout << "Window is already created\n";
			return false;
		}

		if (!glfwInit()) 
		{
			std::cout << "GLFW could not initialize\n";
			return false;
		}

		glfwSetErrorCallback(ErrorCallback);
		
		gConfig.pWindow = CreateWindow(gConfig.width, gConfig.height, gConfig.title.c_str(), nullptr);
		
		if (!gConfig.pWindow) 
		{
			std::cout << "Window could not be created\n";
			glfwTerminate();
			return false;
		}

		Image *image = new Image(GetLogoData(), GetLogoSize());

		if(image->IsLoaded())
		{
			GLFWimage windowIcon;
			windowIcon.width = image->GetWidth();
			windowIcon.height = image->GetHeight();
			windowIcon.pixels = image->GetData();
			
			if(windowIcon.pixels != nullptr)
			{
				GLFWimage images[1] {
					windowIcon
				};
				
				glfwSetWindowIcon(gConfig.pWindow, 1, images);
			}
		}

		delete image;

		glfwSetFramebufferSizeCallback(gConfig.pWindow, WindowResizeCallback);
		glfwSetKeyCallback(gConfig.pWindow, KeyPressCallback);
		glfwSetCharCallback(gConfig.pWindow, CharPressCallback);
		glfwSetMouseButtonCallback(gConfig.pWindow, MouseButtonPressCallback);
		glfwSetScrollCallback(gConfig.pWindow, ScrollCallback);
		glfwMakeContextCurrent(gConfig.pWindow);

		glfwSwapInterval(gConfig.vsync ? 1 : 0);

        gConfig.pApplication->graphicsContext = std::make_unique<GraphicsContext>();

		if(!gConfig.pApplication->graphicsContext->Initialize(gConfig.width, gConfig.height))
		{
			gConfig.pWindow = nullptr;
			glfwDestroyWindow(gConfig.pWindow);
			glfwTerminate();
			std::cout << "Failed to initialize OpenGL\n";
			return false;
		}

		if(gConfig.pApplication)
		{
			gConfig.pApplication->OnLoad();
		}

		while(!glfwWindowShouldClose(gConfig.pWindow))
		{
			PumpEvents();
		}

		if(gConfig.pApplication)
		{
			gConfig.pApplication->OnDestroy();
		}

		glfwDestroyWindow(gConfig.pWindow);
		glfwTerminate();
		
		gConfig.pWindow = nullptr;
		gConfig.deltaTime = 0.0;
		gConfig.lastTime = 0.0;

		return true;
	}

	void ApplicationBase::Close()
	{
		Quit();
	}

	void ApplicationBase::Quit()
	{
	#ifndef __EMSCRIPTEN__
		if(glfwWindowShouldClose(gConfig.pWindow))
			return;
		
		glfwSetWindowShouldClose(gConfig.pWindow, GLFW_TRUE);
	#endif
	}

	void ApplicationBase::SetTitle(const char *title)
	{
		gConfig.title = title;
		
		if(gConfig.pWindow)
			glfwSetWindowTitle(gConfig.pWindow, gConfig.title.c_str());
	}

	void ApplicationBase::PumpEvents()
	{
		double currentTime = glfwGetTime();
		gConfig.deltaTime = currentTime - gConfig.lastTime;
		gConfig.lastTime = currentTime;

		if(gConfig.pApplication)
		{
			Time::NewFrame();
			gConfig.pApplication->OnUpdate();
			gConfig.pApplication->OnLateUpdate();
            gConfig.pApplication->graphicsContext->NewFrame();
            gConfig.pApplication->graphicsContext->BeginGUI();
			gConfig.pApplication->OnGUI();
            gConfig.pApplication->graphicsContext->EndGUI();
		}
		
		glfwSwapBuffers(gConfig.pWindow);
		glfwPollEvents();
	}
	
	GLFWwindow *ApplicationBase::GetNativeWindow()
	{
		return gConfig.pWindow;
	}

	void ApplicationBase::WindowResizeCallback(GLFWwindow *window, int32_t width, int32_t height)
	{
		if(width < 10 || height < 10)
			return;
		gConfig.width = width;
		gConfig.height = height;
        gConfig.pApplication->graphicsContext->SetScreenSize(width, height);
	}

	void ApplicationBase::KeyPressCallback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
	}

	void ApplicationBase::CharPressCallback(GLFWwindow *window, uint32_t codepoint)
	{
	}

	void ApplicationBase::MouseButtonPressCallback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods)
	{
	}

	void ApplicationBase::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
	{
	}

	void ApplicationBase::ErrorCallback(int32_t errorCode, const char *description)
	{
		std::cout << "GLFW error (" << errorCode << "): " << description << '\n';
	}
}