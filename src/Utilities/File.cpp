#include "File.hpp"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace ShoutBlast
{
    bool File::Exists(const std::string &filePath)
    {
        return fs::exists(filePath) && fs::is_regular_file(filePath);
    }

    std::string File::ReadAllText(const std::string &filePath)
    {
		std::ifstream f(filePath);
		
        if (!f.is_open())
			throw std::runtime_error("Could not open file: " + filePath);

		std::ostringstream contentStream;
		contentStream << f.rdbuf();
        f.close();
		return contentStream.str();
    }

    bool File::WriteAllText(const std::string &filePath, const std::string &text)
    {
        std::ofstream f(filePath, std::ios_base::out);
        if (f.is_open())
        {
            f << text;
            f.close();
			return true;
        }
		return false;
    }

    std::vector<uint8_t> File::ReadAllBytes(const std::string &filePath)
    {
		std::ifstream f(filePath, std::ios::binary);
		
        if (!f.is_open()) 
			throw std::runtime_error("Could not open file: " + filePath);

		f.seekg(0, std::ios::end);
		std::streamsize fileSize = f.tellg();
		f.seekg(0, std::ios::beg);

		std::vector<uint8_t> buffer(fileSize);

		if (!f.read(reinterpret_cast<char*>(buffer.data()), fileSize))
			throw std::runtime_error("Error reading file: " + filePath);
        
        f.close();
		return buffer;
    }

    bool File::WriteAllBytes(const std::string &filePath, const void *data, size_t size)
    {
        std::ofstream f(filePath, std::ios::binary);

        if (f.is_open())
		{
            f.write(reinterpret_cast<const char*>(data), size);
            f.close();
			return true;
		}
		return false;
    }

	std::string File::GetDirectoryPath(const std::string &filePath)
	{
		if (filePath.empty()) 
			return "";

		fs::path p(filePath);
		
		// parent_path() extracts the directory part
		return p.parent_path().string();
	}
}