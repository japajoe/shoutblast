#ifndef SHOUTBLAST_RUNTIME_HPP
#define SHOUTBLAST_RUNTIME_HPP

#include <string>

namespace ShoutBlast
{
    class Runtime
    {
    public:
        static void *LoadLibraryFromPath(const std::string &filePath);
        static void UnloadLibrary(void *libraryHandle);
        static void *GetSymbol(void *libraryHandle, const std::string &symbolName);
        static bool FindLibraryPath(const std::string &libraryName, std::string &libraryPath);
    };
}

#endif