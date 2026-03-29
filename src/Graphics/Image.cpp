#include "Image.hpp"
#include "../../libs/stb/include/stb_image.h"
#include "../../libs/stb/include/stb_image_write.h"
#include <cstring>
#include <vector>
#include <utility>

namespace ShoutBlast
{
    Image::Image() 
	{
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
    }

    Image::Image(const std::string &filepath) 
	{
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        this->hasLoaded = false;
        if(LoadFromFile(filepath)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *compressedData, size_t size) 
	{
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = 0;
        this->height = 0;
        this->channels = 0;
        if(LoadFromMemory(compressedData, size)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const uint8_t *uncompressedData, size_t size, uint32_t width, uint32_t height, uint32_t channels) 
	{
        this->width = width;
        this->height = height;
        this->channels = channels;
        this->data = new uint8_t[size];
        memcpy(data, uncompressedData, size);
        this->hasLoaded = true;
    }

    Image::Image(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) 
	{
        this->hasLoaded = false;
        this->data = nullptr;
        this->width = width;
        this->height = height;
        this->channels = channels;
        if(Load(width, height, channels, r, g, b, a)) 
		{
            this->hasLoaded = true;
        }
    }

    Image::Image(const Image &other) 
	{
        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;
        hasLoaded = other.hasLoaded;
    }

    Image::Image(Image &&other) noexcept 
	{
        data = std::exchange(other.data, nullptr);
        width = other.width;
        height = other.height;
        channels = other.channels;
        hasLoaded = std::exchange(other.hasLoaded, false);
    }

    Image &Image::operator=(const Image &other) 
	{
        if(this != &other) 
		{
            data = other.data;
            width = other.width;
            height = other.height;
            channels = other.channels;
            hasLoaded = other.hasLoaded;
        }
        return *this;
    }

    Image &Image::operator=(Image &&other) noexcept 
	{
        if(this != &other) 
		{
			data = std::exchange(other.data, nullptr);
			width = other.width;
			height = other.height;
			channels = other.channels;
			hasLoaded = std::exchange(other.hasLoaded, false);
        }
        return *this;
    }

    Image::~Image() 
	{
        if(data != nullptr) 
		{
            delete[] data;
        }
		data = nullptr;
    }

    uint8_t *Image::GetData() const 
	{
        return data;
    }

    uint32_t Image::GetWidth() const 
	{
        return width;
    }

    uint32_t Image::GetHeight() const 
	{
        return height;
    }

    uint32_t Image::GetChannels() const {
        return channels;
    }

    size_t Image::GetDataSize() const 
	{
        return width * height * channels;
    }

    bool Image::IsLoaded() const 
	{
        return hasLoaded;
    }

    bool Image::LoadFromFile(const std::string &filepath) 
	{
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

        if (uncompressedData) 
		{
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];
            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::LoadFromMemory(const uint8_t *data, size_t size) 
	{
        int width, height, channels;
        unsigned char *uncompressedData = stbi_load_from_memory(data, size, &width, &height, &channels, 0);

        if (uncompressedData)
		{
            size_t dataSize = width * height * channels * sizeof(unsigned char);

            this->data = new uint8_t[dataSize];

            this->width = width;
            this->height = height;
            this->channels = channels;
            memcpy(this->data, uncompressedData, dataSize);
            stbi_image_free(uncompressedData);
            return true;
        }

        return false;
    }

    bool Image::Load(uint32_t width, uint32_t height, uint32_t channels, float r, float g, float b, float a) 
	{
        if(channels < 3 || channels > 4)
            return false;

        size_t size = width * height * channels;

        if(size == 0)
            return false;

        this->data = new uint8_t[size];

        auto clamp = [] (float value, float min, float max) {
            if(value < min)
                return min;
            else if(value > max)
                return max;
            return value;
        };

        if(channels == 3) 
		{
            for(size_t i = 0; i < size; i += 3) 
			{
                uint8_t R = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t G = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t B = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));

                data[i+0] = R;
                data[i+1] = G;
                data[i+2] = B;
            }
        } 
		else 
		{
            for(size_t i = 0; i < size; i += 4) 
			{
                uint8_t R = static_cast<uint8_t>(clamp(r * 255, 0.0, 255.0));
                uint8_t G = static_cast<uint8_t>(clamp(g * 255, 0.0, 255.0));
                uint8_t B = static_cast<uint8_t>(clamp(b * 255, 0.0, 255.0));
                uint8_t A = static_cast<uint8_t>(clamp(a * 255, 0.0, 255.0));

                data[i+0] = R;
                data[i+1] = G;
                data[i+2] = B;
                data[i+3] = A;
            }
        }
        
        return true;
    }

    bool Image::SaveAsPNG(const std::string &filepath, const void *data, size_t size, size_t width, size_t height, size_t channels) 
	{
        return stbi_write_png(filepath.c_str(), width, height, channels, data, width * channels) > 0;
    }
}