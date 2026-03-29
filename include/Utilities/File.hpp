#ifndef SHOUTBLAST_FILE_HPP
#define SHOUTBLAST_FILE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ShoutBlast
{
    class File
    {
    public:
        static bool Exists(const std::string &filePath);
        static std::string ReadAllText(const std::string &filePath);
        static bool WriteAllText(const std::string &filePath, const std::string &text);
        static std::vector<uint8_t> ReadAllBytes(const std::string &filePath);
        static bool WriteAllBytes(const std::string &filePath, const void *data, size_t size);
        static std::string GetDirectoryPath(const std::string &filePath);
    };
}

#endif