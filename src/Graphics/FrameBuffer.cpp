#include "FrameBuffer.hpp"
#include "OpenGL.hpp"
#include <cassert>

namespace ShoutBlast
{
    static constexpr uint32_t MAX_FRAMEBUFFER_SIZE = 8192;

    namespace Utilities
    {
        static bool IsDepthFormat(FrameBufferTextureFormat format)
        {
            switch(format)
            {
            case FrameBufferTextureFormat::Depth24Stencil8:
            case FrameBufferTextureFormat::Depth32F:
                return true;
            default:
                return false;
            }
        }

        static GLenum TextureTarget(bool multiSampled)
        {
            return multiSampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        static GLenum FilterMode(TextureFilterMode filterMode)
        {
            switch(filterMode)
            {
            case TextureFilterMode::Nearest:
                return GL_NEAREST;
            case TextureFilterMode::Linear:
                return GL_LINEAR;
            case TextureFilterMode::Trilinear:
                return GL_LINEAR_MIPMAP_LINEAR;
            case TextureFilterMode::BilinearMipmap:
                return GL_LINEAR_MIPMAP_NEAREST;
            default:
                return GL_LINEAR;
            }
        }

        static GLenum WrapMode(TextureWrapMode wrapMode)
        {
            switch(wrapMode)
            {
            case TextureWrapMode::Repeat:
                return GL_REPEAT;
            case TextureWrapMode::MirroredRepeat:
                return GL_MIRRORED_REPEAT;
            case TextureWrapMode::ClampToEdge:
                return GL_CLAMP_TO_EDGE;
            case TextureWrapMode::ClampToBorder:
                return GL_CLAMP_TO_BORDER;
            default:
                return GL_CLAMP_TO_BORDER;
            }
        }

        static void CreateTextures(bool multiSampled, uint32_t *textures, uint32_t count)
        {
            //glCreateTextures(TextureTarget(multiSampled), count, textures); //OpenGL 4.5+
            glGenTextures(count, textures);
        }

        static void BindTexture(bool multiSampled, uint32_t textureId)
        {
            glBindTexture(TextureTarget(multiSampled), textureId);
        }

        static void AttachColorTexture(uint32_t textureId, uint32_t samples, GLenum internalFormat, GLenum wrapMode, GLenum filterMode, uint32_t width, uint32_t height, uint32_t index)
        {
            bool multiSampled = samples > 1;

            if(multiSampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            }
            else
            {
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multiSampled), textureId, 0);
        }

        static void AttachDepthTexture(uint32_t textureId, uint32_t samples, GLenum internalFormat, GLenum attachmentType, GLenum wrapMode, GLenum filterMode, uint32_t width, uint32_t height)
        {
            bool multiSampled = samples > 1;

            if(multiSampled)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
            }
            else
            {
                //OpenGL 3.3 approach for depth
                //For depth, format and type usually match the internalFormat requirements
                GLenum format = GL_DEPTH_COMPONENT;
                GLenum type = GL_FLOAT;

                if (internalFormat == GL_DEPTH24_STENCIL8)
                {
                    format = GL_DEPTH_STENCIL;
                    type = GL_UNSIGNED_INT_24_8;
                }

                //glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height); // OpenGL 4.2+
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
            }

            glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multiSampled), textureId, 0);
        }
    }

    void FrameBuffer::Generate(const FrameBufferSpecification &specification)
    {
        this->specification = specification;

        if(colorAttachmentSpecifications.size() > 0)
            colorAttachmentSpecifications.clear();
        
        for(const auto &spec : specification.attachments)
        {
            if(!Utilities::IsDepthFormat(spec.format))
            {
                colorAttachmentSpecifications.emplace_back(spec);
            }
            else
            {
                depthAttachmentSpecification = spec;
            }
        }

        Invalidate();
    }

    void FrameBuffer::Destroy()
    {
        if(id)
        {
            glDeleteFramebuffers(1, &id);
            glDeleteTextures(colorAttachments.size(), colorAttachments.data());
            glDeleteTextures(1, &depthAttachment);
            
            id = 0;
            depthAttachment = 0;
            colorAttachments.clear();
        }
    }

    void FrameBuffer::Resize(uint32_t width, uint32_t height)
    {
        if(!specification.resizable)
            return;

        if(width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE)
            return;

        specification.width = width;
        specification.height = height;

        Invalidate();
    }

    void FrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
        glViewport(0, 0, specification.width, specification.height);
    }

    void FrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Clear(float r, float g, float b, float a)
    {
        GLenum flags = 0;
        
        if(colorAttachments.size() > 0)
            flags |= GL_COLOR_BUFFER_BIT;
        if(depthAttachment)
            flags |= GL_DEPTH_BUFFER_BIT;

        glClearColor(r, g, b, a);
        glClear(flags);
    }

    void FrameBuffer::Blit(const FrameBuffer &target, BlitOption options)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.id);

		GLbitfield mask = 0;
		
		if (options & BlitOption_Color)
			mask |= GL_COLOR_BUFFER_BIT;
		
		if (options & BlitOption_Depth)
			mask |= GL_DEPTH_BUFFER_BIT;

        // Blit color and depth
        glBlitFramebuffer(
            0, 0, specification.width, specification.height,
            0, 0, target.specification.width, target.specification.height,
            mask,
            GL_NEAREST // GL_NEAREST is required when resolving multisampled buffers
        );
    }

    void FrameBuffer::Blit(const FrameBuffer &target, BlitOption options, uint32_t attachmentIndex)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.id);

        GLbitfield mask = 0;
        
        if (options & BlitOption_Color)
        {
            mask |= GL_COLOR_BUFFER_BIT;
            
            // We use the attachmentIndex to target GL_COLOR_ATTACHMENT0, 1, 2, etc.
            glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        }
        
        if (options & BlitOption_Depth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        glBlitFramebuffer(
            0, 0, specification.width, specification.height,
            0, 0, target.specification.width, target.specification.height,
            mask,
            GL_NEAREST 
        );

        // Reset to default state to avoid side effects in other draw calls
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    void FrameBuffer::Blit(const FrameBuffer &target, BlitOption options, uint32_t attachmentIndexSource, uint32_t attachmentIndexDestination)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->id);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.id);

        GLbitfield mask = 0;
        
        if (options & BlitOption_Color)
        {
            mask |= GL_COLOR_BUFFER_BIT;
            
            // Map source index to destination index
            glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndexSource);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndexDestination);
        }
        
        if (options & BlitOption_Depth)
        {
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        glBlitFramebuffer(
            0, 0, specification.width, specification.height,
            0, 0, target.specification.width, target.specification.height,
            mask,
            GL_NEAREST 
        );

        // Reset to defaults
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    uint32_t FrameBuffer::GetWidth() const
    {
        return specification.width;
    }

    uint32_t FrameBuffer::GetHeight() const
    {
        return specification.height;
    }

    uint32_t FrameBuffer::GetSamples() const
    {
        return specification.samples;
    }

    uint32_t FrameBuffer::GetColorAttachment(uint32_t index) const
    {
        assert(index < colorAttachments.size() && "Color attachment index must be less than the total number of attachments");
        return colorAttachments[index];
    }

    uint32_t FrameBuffer::GetDepthAttachment() const
    {
        return depthAttachment;
    }

    void FrameBuffer::Invalidate()
    {
        if(id)
        {
            glDeleteFramebuffers(1, &id);
            glDeleteTextures(colorAttachments.size(), colorAttachments.data());
            glDeleteTextures(1, &depthAttachment);

            colorAttachments.clear();
            depthAttachment = 0;
        }

        //glCreateFramebuffers(1, &id); //OpenGL 4.5+
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        bool multiSample = specification.samples > 1;

        if(colorAttachmentSpecifications.size())
        {
            colorAttachments.resize(colorAttachmentSpecifications.size());
            Utilities::CreateTextures(multiSample, colorAttachments.data(), colorAttachments.size());

            for(size_t i = 0; i < colorAttachments.size(); i++)
            {
                Utilities::BindTexture(multiSample, colorAttachments[i]);
                GLenum wrap = Utilities::WrapMode(colorAttachmentSpecifications[i].wrap);
                GLenum filter = Utilities::FilterMode(colorAttachmentSpecifications[i].filter);

                switch(colorAttachmentSpecifications[i].format)
                {
                case FrameBufferTextureFormat::RGBA8:
                    Utilities::AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA8, wrap, filter, specification.width, specification.height, i);
                    break;
                case FrameBufferTextureFormat::RGBA16F:
                    Utilities::AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA16F, wrap, filter, specification.width, specification.height, i);
                    break;
                case FrameBufferTextureFormat::RGBA32F:
                    Utilities::AttachColorTexture(colorAttachments[i], specification.samples, GL_RGBA32F, wrap, filter, specification.width, specification.height, i);
                    break;
                case FrameBufferTextureFormat::RG32F:
                    Utilities::AttachColorTexture(colorAttachments[i], specification.samples, GL_RG32F, wrap, filter, specification.width, specification.height, i);
                    break;
                default:
                    break;
                }
            }
        }

        if(depthAttachmentSpecification.format != FrameBufferTextureFormat::None)
        {
            Utilities::CreateTextures(multiSample, &depthAttachment, 1);
            Utilities::BindTexture(multiSample, depthAttachment);
            GLenum wrap = Utilities::WrapMode(depthAttachmentSpecification.wrap);
            GLenum filter = Utilities::FilterMode(depthAttachmentSpecification.filter);

            switch(depthAttachmentSpecification.format)
            {
            case FrameBufferTextureFormat::Depth24Stencil8:
                Utilities::AttachDepthTexture(depthAttachment, specification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, wrap, filter, specification.width, specification.height);
                break;
            case FrameBufferTextureFormat::Depth32F:
                Utilities::AttachDepthTexture(depthAttachment, specification.samples, GL_DEPTH_COMPONENT32F, GL_DEPTH_ATTACHMENT, wrap, filter, specification.width, specification.height);
                break;
            default:
                break;
            }
        }

        if(colorAttachments.size() >= 1)
        {
            assert(colorAttachments.size() <= 4 && "Max 4 color attachments allowed");
            GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glDrawBuffers(colorAttachments.size(), buffers);
        }
        else
        {
            // Only depth
            glDrawBuffer(GL_NONE);
        }

        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Frame buffer is incomplete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}