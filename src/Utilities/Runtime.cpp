#include "Runtime.hpp"
#include "Platform.hpp"

#if defined(SB_PLATFORM_WINDOWS)
#include <windows.h>
#endif

#if defined(SB_PLATFORM_LINUX) || defined(SB_PLATFORM_MAC) || defined(SB_PLATFORM_BSD)
#include <dlfcn.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#endif

#include <filesystem>
#include <iostream>
#include <cstring>

namespace ShoutBlast
{
	void *Runtime::LoadLibrary(const std::string &filePath)
	{
		if (!std::filesystem::exists(std::filesystem::path(filePath)))
		{
			std::cout << "File not found: " << filePath << '\n';
			return nullptr;
		}

		void *moduleHandle = nullptr;

#if defined(SB_PLATFORM_WINDOWS)
		moduleHandle = (void *)LoadLibrary(filePath.c_str());
		if (!moduleHandle)
			std::cout << "Failed to load plugin: " << filePath << '\n';
#elif defined(SB_PLATFORM_LINUX) || defined(SB_PLATFORM_MAC) || defined(SB_PLATFORM_BSD)
		moduleHandle = dlopen(filePath.c_str(), RTLD_LAZY);
		if (!moduleHandle)
		{
			char *error = dlerror();
			std::cout << "Failed to load plugin: " << filePath << ". Error: " << error << '\n';
		}
#endif

		return moduleHandle;
	}

	void Runtime::UnloadLibrary(void *libraryHandle)
	{
		if (!libraryHandle)
			return;
#if defined(SB_PLATFORM_WINDOWS)
		FreeLibrary((HINSTANCE)libraryHandle);
#elif defined(SB_PLATFORM_LINUX) || defined(SB_PLATFORM_MAC) || defined(SB_PLATFORM_BSD)
		dlclose(libraryHandle);
#endif
	}

	void *Runtime::GetSymbol(void *libraryHandle, const std::string &symbolName)
	{
		if (!libraryHandle)
			return nullptr;

		void *s = nullptr;

#if defined(SB_PLATFORM_WINDOWS)
		s = (void *)GetProcAddress((HINSTANCE)libraryHandle, symbolName.c_str());
		if (s == nullptr)
			std::cout << "Error: undefined symbol: " << symbolName << '\n';
#elif defined(SB_PLATFORM_LINUX) || defined(SB_PLATFORM_MAC) || defined(SB_PLATFORM_BSD)
		dlerror(); /* clear error code */
		s = dlsym(libraryHandle, symbolName.c_str());
		char *error = dlerror();

		if (error != nullptr)
			std::cout << "Error: " << error << '\n';
#endif

		return s;
	}

	bool Runtime::FindLibraryPath(const std::string &libraryName, std::string &libraryPath)
	{
#if defined(SB_PLATFORM_WINDOWS)
		static char result[4096]; // Static buffer to hold result
		DWORD res = SearchPath(nullptr, libraryName.c_str(), nullptr, MAX_PATH, result, nullptr);
		if (res == 0)
			return false;
		int len = strlen(result);
		char *outputPath = new char[len + 1];
		std::memcpy(outputPath, result, len);
		outputPath[len] = '\0';
		libraryPath = std::string(outputPath);
		delete[] outputPath;
		return true;
#elif defined(SB_PLATFORM_LINUX) || defined(SB_PLATFORM_BSD)
		// Prepare the command to search the library
		char cmd[256];

#ifdef SB_PLATFORM_LINUX
		snprintf(cmd, sizeof(cmd), "ldconfig -p 2>/dev/null | grep %s", libraryName.c_str());
#else
		snprintf(cmd, sizeof(cmd), "ldconfig -r | grep %s", libraryName.c_str());
#endif

		FILE *pipe = popen(cmd, "r");

		if (!pipe)
		{
			std::cout << "popen() failed\n";
			return false;
		}

		static char result[4096]; // Static buffer to hold result

		while (fgets(result, sizeof(result), pipe) != NULL)
		{
			// Find the path after the "=>" symbol
			char *pos = strstr(result, "=>");
			if (pos != NULL)
			{
				pos += 2; // Move pointer to the path
				// Trim whitespace
				while (*pos == ' ')
					pos++;
				// Remove newline character
				char *newline = strchr(pos, '\n');
				if (newline)
					*newline = '\0';
				pclose(pipe);
				int len = strlen(pos);
				char *outputPath = new char[len + 1];
				std::memcpy(outputPath, pos, len);
				outputPath[len] = '\0';
				libraryPath = std::string(outputPath);
				delete[] outputPath;
				return true;
			}
		}

		pclose(pipe);
		return false;
#elif defined(SB_PLATFORM_MAC)
		return false;
#else
		return false;
#endif
	}
}