#ifndef TEXTURE_FORMAT_HH
#define TEXTURE_FORMAT_HH

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <GL/gl.h>
#include "gfx/OpenGL/glext.h"

namespace Gfx
{
	struct TextureFormat
	{
		enum Enum {
			// Depth and stencil formats
			Stencil,
			Depth,
// 			Depth16,
// 			Depth24,
// 			Depth32f,
			D24S8,

			// Sized formats
			R8,
			RG8,
			RGB5_A1,
			RGBA8,
			RGBA16,
			R16f,
			RG16f,
			RGBA16f,
			R32f,
			RG32f,
			RGBA32f,
			R11G11B10f,

			// Compressed formats
			Compressed,
		};
	};

	struct TextureType
	{
		enum Enum {
			Texture2D = GL_TEXTURE_2D,
			CubeMap = GL_TEXTURE_CUBE_MAP,
		};
	};

	struct TextureFilter
	{
		enum Enum {
			Nearest = GL_NEAREST,
			Linear = GL_LINEAR,
			NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
			NearestMipmapLinear = GL_LINEAR_MIPMAP_NEAREST,
			LinearMipmapNearest = GL_NEAREST_MIPMAP_LINEAR,
			LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
		};
	};

	struct TextureWrap
	{
		enum Enum {
			ClampToEdge = GL_CLAMP_TO_BORDER,
			ClampToBorder = GL_CLAMP_TO_BORDER,
			Repeat = GL_REPEAT,
		};
	};

	struct TextureSampling
	{
		TextureFilter::Enum	minifyingFilter;
		TextureFilter::Enum	magnifyingFilter;
		float				maxAnisotropy;
		TextureWrap::Enum	rWrap;
		TextureWrap::Enum	sWrap;
		TextureWrap::Enum	tWrap;
	};
}

#endif // TEXTURE_FORMAT_HH
