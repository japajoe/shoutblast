#ifndef SHOUTBLAST_TEXTURE_HPP
#define SHOUTBLAST_TEXTURE_HPP

namespace ShoutBlast
{
	enum class TextureWrapMode 
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder
	};

	enum class TextureFilterMode 
	{
		Nearest,
		Linear,
		Trilinear,      // Maps to GL_LINEAR_MIPMAP_LINEAR
		BilinearMipmap  // Maps to GL_LINEAR_MIPMAP_NEAREST
	};

	struct TextureSettings 
	{
		TextureWrapMode wrapS = TextureWrapMode::Repeat;
		TextureWrapMode wrapT = TextureWrapMode::Repeat;
		TextureFilterMode minFilter = TextureFilterMode::Trilinear;
		TextureFilterMode magFilter = TextureFilterMode::Linear;
	};
}

#endif