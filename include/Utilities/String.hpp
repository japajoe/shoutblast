#ifndef SHOUTBLAST_STRING_HPP
#define SHOUTBLAST_STRING_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>

namespace ShoutBlast
{
    class String
    {
    public:
        static bool IsValidUTF8(const void *data, size_t size);
        static bool Contains(const std::string &haystack, const std::string &needle, bool ignoreCase = false);
        static bool StartsWith(const std::string &haystack, const std::string &needle);
        static bool EndsWith(const std::string &haystack, const std::string &needle);
        static std::string Trim(const std::string &str);
        static std::string TrimStart(const std::string &str);
        static std::string TrimEnd(const std::string &str);
        static std::vector<std::string> Split(const std::string& str, char separator, size_t maxParts = 0);
        static std::vector<std::string> Split(const std::string& str, const std::string &separators, size_t maxParts = 0);
        static std::string Replace(const std::string &str, const std::string &target, const std::string &replacement);
        static std::string ToLower(const std::string &str);
        static std::string ToUpper(const std::string &str);
        static std::string SubString(const std::string &str, size_t startIndex);
        static std::string SubString(const std::string &str, size_t startIndex, size_t length);
        static std::string UrlEncode(const std::string &str);
        static int64_t IndexOf(const std::string &str, const std::string &subStr);
        static bool TryParseUInt8(const std::string &value, uint8_t &v);
        static bool TryParseUInt16(const std::string &value, uint16_t &v);
        static bool TryParseUInt32(const std::string &value, uint32_t &v);
        static bool TryParseUInt64(const std::string &value, uint64_t &v);
        static bool TryParseInt8(const std::string &value, int8_t &v);
        static bool TryParseInt16(const std::string &value, int16_t &v);
        static bool TryParseInt32(const std::string &value, int32_t &v);
        static bool TryParseInt64(const std::string &value, int64_t &v);
        static bool TryParseFloat(const std::string &value, float &v);
        static bool TryParseDouble(const std::string &value, double &v);
    };
}

#endif