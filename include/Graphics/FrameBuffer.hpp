#ifndef SHOUTBLAST_FRAMEBUFFER_HPP
#define SHOUTBLAST_FRAMEBUFFER_HPP

#include "Texture.hpp"
#include <cstdint>
#include <vector>

namespace ShoutBlast
{
	enum BlitOption_
	{
		BlitOption_Color = 1 << 0,
		BlitOption_Depth = 1 << 1,
	};

	typedef uint32_t BlitOption;

    enum class FrameBufferTextureFormat
    {
        None,
        Depth24Stencil8,
        Depth32F,
        RGBA8,
        RGBA16F,
        RGBA32F,
        RG32F,
        Depth = Depth24Stencil8,
    };

    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureFormat format = FrameBufferTextureFormat::None;
        TextureWrapMode wrap = TextureWrapMode::ClampToEdge;
        TextureFilterMode filter = TextureFilterMode::Linear;
    };

    struct FrameBufferSpecification
    {
        uint32_t width = 512;
        uint32_t height = 512;
        uint32_t samples = 1;
        bool resizable = true;
        std::vector<FrameBufferTextureSpecification> attachments;
    };

    class FrameBuffer
    {
    public:
        FrameBuffer() = default;
        void Generate(const FrameBufferSpecification &specification);
        void Destroy();
        void Resize(uint32_t width, uint32_t height);
        void Bind();
        void Unbind();
        void Clear(float r, float g, float b, float a);
        void Blit(const FrameBuffer &target, BlitOption options);
        void Blit(const FrameBuffer &target, BlitOption options, uint32_t attachmentIndex);
        void Blit(const FrameBuffer &target, BlitOption options, uint32_t attachmentIndexSource, uint32_t attachmentIndexDestination);
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        uint32_t GetSamples() const;
        uint32_t GetColorAttachment(uint32_t index) const;
        uint32_t GetDepthAttachment() const;
    private:
        uint32_t id = 0;
        uint32_t depthAttachment = 0;
        std::vector<uint32_t> colorAttachments;
        FrameBufferSpecification specification;
        std::vector<FrameBufferTextureSpecification> colorAttachmentSpecifications;
        FrameBufferTextureSpecification depthAttachmentSpecification;
        void Invalidate();
    };
}

#endif