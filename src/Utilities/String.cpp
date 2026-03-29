#include "String.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ShoutBlast
{
    bool String::IsValidUTF8(const void *data, size_t size) 
	{
        int numBytes = 0; // Number of bytes expected in the current UTF-8 character
        unsigned char byte;
        const uint8_t *pData = reinterpret_cast<const uint8_t*>(data);

        for (size_t i = 0; i < size; ++i) 
		{
            byte = pData[i];

            if (numBytes == 0) 
			{
                // Determine the number of bytes in the UTF-8 character
                if ((byte & 0x80) == 0) 
				{
                    // 1-byte character (ASCII)
                    continue;
                } 
				else if ((byte & 0xE0) == 0xC0) 
				{
                    // 2-byte character
                    numBytes = 1;
                } 
				else if ((byte & 0xF0) == 0xE0) 
				{
                    // 3-byte character
                    numBytes = 2;
                } 
				else if ((byte & 0xF8) == 0xF0) 
				{
                    // 4-byte character
                    numBytes = 3;
                } 
				else 
				{
                    // Invalid first byte
                    return false;
                }
            } 
			else 
			{
                // Check continuation bytes
                if ((byte & 0xC0) != 0x80) 
				{
                    return false; // Invalid continuation byte
                }
                numBytes--;
            }
        }

        return numBytes == 0; // Ensure all characters were complete
    }

    bool String::Contains(const std::string &haystack, const std::string &needle, bool ignoreCase) 
	{
        if(!ignoreCase)
            return haystack.find(needle) != std::string::npos;
        
        auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(), [](uint8_t h, uint8_t n) {
            return std::tolower(h) == std::tolower(n);
        });

        return it != haystack.end();
    }

    bool String::StartsWith(const std::string &haystack, const std::string &needle)
    {
        if (haystack.length() >= needle.length()) 
            return (0 == haystack.compare(0, needle.length(), needle));
        return false;
    }

    bool String::EndsWith(const std::string &haystack, const std::string &needle) 
	{
        if (haystack.length() >= needle.length()) 
            return (0 == haystack.compare(haystack.length() - needle.length(), needle.length(), needle));
        return false;
    }
    
    std::string String::Trim(const std::string &str)
    {
        size_t start = 0;
        size_t end = str.length();

        // Find the first non-whitespace character
        while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) 
        {
            ++start;
        }

        // Find the last non-whitespace character
        while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) 
        {
            --end;
        }

        // Return the substring from the first to the last non-whitespace character
        return str.substr(start, end - start);
    }

    std::string String::TrimStart(const std::string &str) 
    {
        size_t start = 0;

        // Find the first non-whitespace character
        while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start]))) 
        {
            ++start;
        }

        // Return the substring from the first non-whitespace character to the end
        return str.substr(start);
    }

    std::string String::TrimEnd(const std::string &str)
    {
        size_t end = str.length();

        // Find the last non-whitespace character
        while (end > 0 && std::isspace(static_cast<unsigned char>(str[end - 1]))) 
        {
            --end;
        }

        // Return the substring from the beginning to the last non-whitespace character
        return str.substr(0, end);
    }

	std::vector<std::string> String::Split(const std::string& str, char separator, size_t maxParts) 
    {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = 0;

        while ((end = str.find(separator, start)) != std::string::npos) 
        {
            result.push_back(str.substr(start, end - start));
            start = end + 1;

            if (maxParts > 0 && result.size() >= maxParts - 1) 
                break; // Stop if we have reached maximum parts
        }
        result.push_back(str.substr(start)); // Add the last part
        return result;
    }

    std::vector<std::string> String::Split(const std::string& str, const std::string &separators, size_t maxParts)
    {
        std::vector<std::string> result;
        size_t start = 0;
        size_t end = 0;

        while ((end = str.find_first_of(separators, start)) != std::string::npos)
        {
            result.push_back(str.substr(start, end - start));
            start = end + 1;

            if (maxParts > 0 && result.size() >= maxParts - 1)
                break; // Stop if we have reached maximum parts
        }

        result.push_back(str.substr(start)); // Add the last part
        return result;
    }

    std::string String::Replace(const std::string &str, const std::string &target, const std::string &replacement)
    {
        std::string newStr = str;
        size_t startPos = 0;

        while ((startPos = newStr.find(target, startPos)) != std::string::npos)
        {
            newStr.replace(startPos, target.length(), replacement);
            startPos += replacement.length();
        }

        return newStr;
    }

    std::string String::ToLower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::string String::ToUpper(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    std::string String::SubString(const std::string &str, size_t startIndex)
    {
        return str.substr(startIndex);
    }

    std::string String::SubString(const std::string &str, size_t startIndex, size_t length)
    {
        return str.substr(startIndex, length);
    }

    std::string String::UrlEncode(const std::string &str)
    {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (unsigned char c : str)
        {
            if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
            {
                escaped << c;
            }
            else if (c == ' ')
            {
                escaped << '+';
            }
            else
            {
                escaped << '%' << std::setw(2) << std::uppercase << (int)c;
            }
        }

        return escaped.str();
    }

    int64_t String::IndexOf(const std::string &str, const std::string &subStr)
    {
        size_t pos = str.find(subStr);
        if (pos != std::string::npos) 
            return static_cast<int64_t>(pos);
        return -1; // Indicates substring not found
    }

	bool String::TryParseUInt8(const std::string &value, uint8_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseUInt16(const std::string &value, uint16_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseUInt32(const std::string &value, uint32_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseUInt64(const std::string &value, uint64_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseInt8(const std::string &value, int8_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseInt16(const std::string &value, int16_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseInt32(const std::string &value, int32_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseInt64(const std::string &value, int64_t &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseFloat(const std::string &value, float &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}

	bool String::TryParseDouble(const std::string &value, double &v)
	{
        std::stringstream ss(value);
        ss >> v;

        if (ss.fail() || !ss.eof())
            return false;
        
        return true;
	}	
}