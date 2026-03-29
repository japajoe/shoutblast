#ifndef SHOUTBLAST_IMAGE_HPP
#define SHOUTBLAST_IMAGE_HPP

#include <string>
#include <cstdint>

namespace ShoutBlast 
{
    class Image
	{
    public:
        Image();
        Image(const std::string &filepath);
        Image(const uint8_t *compressedData, size_t size);
        Image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels);
        Image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
        Image(const Image &other);
        Image(Image &&other) noexcept;
        Image& operator=(const Image &other);
        Image& operator=(Image &&other) noexcept;
        ~Image();
        uint8_t *GetData() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetChannels() const;
        size_t GetDataSize() const;
        bool IsLoaded() const;
        static bool SaveAsPNG(const std::string &filepath, const void *data, size_t size, size_t width, size_t height, size_t channels);
    private:
        uint8_t *data;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        bool hasLoaded;
        bool LoadFromFile(const std::string &filepath);
        bool LoadFromMemory(const uint8_t *data, size_t size);
        bool Load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a);
    };
}

#endif