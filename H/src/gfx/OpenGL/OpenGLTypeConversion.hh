#ifndef OPENGL_TYPE_CONVERSION_HH
#define OPENGL_TYPE_CONVERSION_HH

#include "gfx/BlendingMode.hh"
#include "gfx/IGraphicLayer.hh"
#include "gfx/PolygonMode.hh"
#include "gfx/RasterTests.hh"
#include "gfx/VertexAttribute.hh"
#include <cassert>
#include <GL/gl.h>

namespace Gfx
{
	// glTexImage2D (format)
	struct TextureFormatConversion
	{
		TextureFormat::Enum			format;
		GLenum						glenum_internalFormat;	// Specifies the number of color components in the texture.
		GLenum						glenum_format;			// Specifies the format of the pixel data.
		GLenum						glenum_type;			// Specifies the data type of the pixel data.
	};

	extern const TextureFormatConversion textureFormatLUT[];

	inline GLenum getTextureFormat_InternalFormatGLenum(const TextureFormat::Enum& format)
	{
		assert(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_internalFormat;
	}

	inline GLenum getTextureFormat_FormatGLenum(const TextureFormat::Enum& format)
	{
		assert(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_format;
	}

	inline GLenum getTextureFormat_TypeGLenum(const TextureFormat::Enum& format)
	{
		assert(textureFormatLUT[format].format == format);
		return textureFormatLUT[format].glenum_type;
	}

	// glVertexAttribPointer
	struct VertexAttributeTypeConversion
	{
		VertexAttributeType::Enum	type;
		GLenum						glenum;
		int							size;
	};

	extern const VertexAttributeTypeConversion vertexAttributeTypeLUT[];

	inline GLenum getVertexAttributeGLenum(const VertexAttributeType::Enum& type)
	{
		assert(vertexAttributeTypeLUT[type].type == type);
		return vertexAttributeTypeLUT[type].glenum;
	}

	inline int getVertexAttributeSize(const VertexAttributeType::Enum& type)
	{
		assert(vertexAttributeTypeLUT[type].type == type);
		return vertexAttributeTypeLUT[type].size;
	}

	// glGetError
	struct ErrorDescription
	{
		GLenum						error;
		const char*					errorString;
		const char*					description;
	};

	extern const ErrorDescription errorDescriptionLUT[];

	const char* getErrorCode(GLenum error);
	const char* getErrorDescription(GLenum error);
}

#endif // OPENGL_TYPE_CONVERSION_HH
