#include "OpenGLTypeConversion.hh"

using namespace Gfx;

//
// Handle a few combinations; other possible combinations in comments,
// from the OpenGL glTexture2d documentation.
//
const TextureFormatConversion Gfx::textureFormatLUT[] = {
	// Depth and stencil formats
	// Format name:					Internal format:		Format:
	{ TextureFormat::Stencil,		GL_STENCIL_INDEX,		GL_STENCIL_INDEX,	GL_UNSIGNED_BYTE,		},
	{ TextureFormat::Depth,			GL_DEPTH_COMPONENT,		GL_DEPTH_COMPONENT,	GL_UNSIGNED_BYTE,		},
// 	{ TextureFormat::Depth16,		GL_DEPTH_COMPONENT16,	GL_DEPTH_COMPONENT,	GL_UNSIGNED_SHORT,		},
// 	{ TextureFormat::Depth24,		GL_DEPTH_COMPONENT24,	GL_DEPTH_COMPONENT,	GL_UNSIGNED_BYTE,		},
// 	{ TextureFormat::Depth32f,		GL_DEPTH_COMPONENT32F,	GL_DEPTH_COMPONENT,	GL_FLOAT,				},
	{ TextureFormat::D24S8,			GL_DEPTH_STENCIL,		GL_DEPTH_STENCIL,	GL_UNSIGNED_INT_24_8,	},

	// Sized formats
	// Format name:				Internal format:	Format:
	{ TextureFormat::R8,		GL_R8,				GL_RED,		GL_UNSIGNED_BYTE,					},
// 	{ ,							GL_R8_SNORM,		GL_RED,		,									},
// 	{ ,							GL_R16,				GL_RED,		,									},
// 	{ ,							GL_R16_SNORM,		GL_RED,		,									},
	{ TextureFormat::RG8,		GL_RG8,				GL_RG,		GL_UNSIGNED_BYTE,					},
// 	{ ,							GL_RG8_SNORM,		GL_RG,		,									},
// 	{ ,							GL_RG16,			GL_RG,		,									},
// 	{ ,							GL_RG16_SNORM,		GL_RG,		,									},
// 	{ ,							GL_R3_G3_B2,		GL_RGB,		,									},
// 	{ ,							GL_RGB4,			GL_RGB,		,									},
// 	{ ,							GL_RGB5,			GL_RGB,		,									},
// 	{ ,							GL_RGB8,			GL_RGB,		,									},
// 	{ ,							GL_RGB8_SNORM,		GL_RGB,		,									},
// 	{ ,							GL_RGB10,			GL_RGB,		,									},
// 	{ ,							GL_RGB12,			GL_RGB,		,									},
// 	{ ,							GL_RGB16_SNORM,		GL_RGB,		,									},
// 	{ ,							GL_RGBA2,			GL_RGB,		,									},
// 	{ ,							GL_RGBA4,			GL_RGB,		,									},
	{ TextureFormat::RGB5_A1,	GL_RGB5_A1,			GL_RGBA,	GL_UNSIGNED_SHORT_5_5_5_1,			},
	{ TextureFormat::RGBA8,		GL_RGBA8,			GL_RGBA,	GL_UNSIGNED_BYTE,					},
// 	{ ,							GL_RGBA8_SNORM,		GL_RGBA,	,									},
// 	{ ,							GL_RGB10_A2,		GL_RGBA,	,									},
// 	{ ,							GL_RGB10_A2UI,		GL_RGBA,	,									},
// 	{ ,							GL_RGBA12,			GL_RGBA,	,									},
	{ TextureFormat::RGBA16,	GL_RGBA16,			GL_RGBA,	GL_UNSIGNED_BYTE,					},
// 	{ ,							GL_SRGB8,			GL_RGB,		,									},
// 	{ ,							GL_SRGB8_ALPHA8,	GL_RGBA,	,									},
	{ TextureFormat::R16f,		GL_R16F,			GL_RED,		GL_FLOAT,							},
	{ TextureFormat::RG16f,		GL_RG16F,			GL_RG,		GL_FLOAT,							},
// 	{ ,							GL_RGB16F,			GL_RGB,		,									},
	{ TextureFormat::RGBA16f,	GL_RGBA16F,			GL_RGBA,	GL_FLOAT,							},
	{ TextureFormat::R32f,		GL_R32F,			GL_RED,		GL_FLOAT,							},
	{ TextureFormat::RG32f,		GL_RG32F,			GL_RG,		GL_FLOAT,							},
// 	{ ,							GL_RGB32F,			GL_RGB,		GL_RGB32F_ARB,						},
	{ TextureFormat::RGBA32f,	GL_RGBA32F,			GL_RGBA,	GL_FLOAT,							},
	{ TextureFormat::R11G11B10f,GL_R11F_G11F_B10F,	GL_RGB,		GL_UNSIGNED_INT_10F_11F_11F_REV,	},
// 	{ ,							GL_RGB9_E5,			GL_RGB,		,									},
// 	{ ,							GL_R8I,				GL_RED,		,									},
// 	{ ,							GL_R8UI,			GL_RED,		,									},
// 	{ ,							GL_R16I,			GL_RED,		,									},
// 	{ ,							GL_R16UI,			GL_RED,		,									},
// 	{ ,							GL_R32I,			GL_RED,		,									},
// 	{ ,							GL_R32UI,			GL_RED,		,									},
// 	{ ,							GL_RG8I,			GL_RG,		,									},
// 	{ ,							GL_RG8UI,			GL_RG,		,									},
// 	{ ,							GL_RG16I,			GL_RG,		,									},
// 	{ ,							GL_RG16UI,			GL_RG,		,									},
// 	{ ,							GL_RG32I,			GL_RG,		,									},
// 	{ ,							GL_RG32UI,			GL_RG,		,									},
// 	{ ,							GL_RGB8I,			GL_RGB,		,									},
// 	{ ,							GL_RGB8UI,			GL_RGB,		,									},
// 	{ ,							GL_RGB16I,			GL_RGB,		,									},
// 	{ ,							GL_RGB16UI,			GL_RGB,		,									},
// 	{ ,							GL_RGB32I,			GL_RGB,		,									},
// 	{ ,							GL_RGB32UI,			GL_RGB,		,									},
// 	{ ,							GL_RGBA8I,			GL_RGBA,	,									},
// 	{ ,							GL_RGBA8UI,			GL_RGBA,	,									},
// 	{ ,							GL_RGBA16I,			GL_RGBA,	,									},
// 	{ ,							GL_RGBA16UI,		GL_RGBA,	,									},
// 	{ ,							GL_RGBA32I,			GL_RGBA,	,									},
// 	{ ,							GL_RGBA32UI,		GL_RGBA,	,									},

	// Compressed formats
	// Format name:					Internal format:						Format:
// 	{ ,								GL_COMPRESSED_RED,						GL_RED,		,			},
// 	{ ,								GL_COMPRESSED_RG,						GL_RG,		,			},
// 	{ ,								GL_COMPRESSED_RGB,						GL_RGB,		,			},
	{ TextureFormat::Compressed,	GL_COMPRESSED_RGBA,						GL_RGBA,	GL_ZERO,	},
// 	{ ,								GL_COMPRESSED_SRGB,						GL_RGB,		,			},
// 	{ ,								GL_COMPRESSED_SRGB_ALPHA,				GL_RGBA,	,			},
// 	{ ,								GL_COMPRESSED_RED_RGTC1,				GL_RED,		,			},
// 	{ ,								GL_COMPRESSED_SIGNED_RED_RGTC1,			GL_RED,		,			},
// 	{ ,								GL_COMPRESSED_RG_RGTC2,					GL_RG,		,			},
// 	{ ,								GL_COMPRESSED_SIGNED_RG_RGTC2,			GL_RG,		,			},
// 	{ ,								GL_COMPRESSED_RGBA_BPTC_UNORM,			GL_RGBA,	,			},
// 	{ ,								GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,	GL_RGBA,	,			},
// 	{ ,								GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,	GL_RGB,		,			},
// 	{ ,								GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,	GL_RGB,		,			},
// 	{ ,								GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,		GL_RGBA,	,			},
// 	{ ,								GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,		GL_RGBA,	,			},
// 	{ ,								GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,		GL_RGBA,	,			},

	// Formats:
	//   GL_RED
	//   GL_RG
	//   GL_RGB
	//   GL_BGR
	//   GL_RGBA
	//   GL_BGRA
	//   GL_RED_INTEGER
	//   GL_RG_INTEGER
	//   GL_RGB_INTEGER
	//   GL_BGR_INTEGER
	//   GL_RGBA_INTEGER
	//   GL_BGRA_INTEGER

	// Types:
	//   GL_UNSIGNED_BYTE
	//   GL_BYTE
	//   GL_UNSIGNED_SHORT
	//   GL_SHORT
	//   GL_UNSIGNED_INT
	//   GL_INT
	//   GL_FLOAT
	//   GL_UNSIGNED_BYTE_3_3_2
	//   GL_UNSIGNED_SHORT_5_6_5
	//   GL_UNSIGNED_SHORT_4_4_4_4
	//   GL_UNSIGNED_SHORT_5_5_5_1
	//   GL_UNSIGNED_INT_8_8_8_8
	//   GL_UNSIGNED_INT_10_10_10_2
	//   GL_UNSIGNED_BYTE_2_3_3_REV
	//   GL_UNSIGNED_SHORT_5_6_5_REV
	//   GL_UNSIGNED_SHORT_4_4_4_4_REV
	//   GL_UNSIGNED_SHORT_1_5_5_5_REV
	//   GL_UNSIGNED_INT_8_8_8_8_REV
	//   GL_UNSIGNED_INT_2_10_10_10_REV
};

const VertexAttributeTypeConversion Gfx::vertexAttributeTypeLUT[] = {
	{ VertexAttributeType::Byte,			GL_BYTE,			sizeof(char),			},
	{ VertexAttributeType::UnsignedByte,	GL_UNSIGNED_BYTE,	sizeof(unsigned char),	},
	{ VertexAttributeType::Int,				GL_INT,				sizeof(int),			},
	{ VertexAttributeType::Float,			GL_FLOAT,			sizeof(float),			},
};

const ErrorDescription Gfx::errorDescriptionLUT[] = {
	{ GL_NO_ERROR,						"GL_NO_ERROR",						"No error has been recorded. The value of this symbolic constant is guaranteed to be 0.", },
	{ GL_INVALID_ENUM,					"GL_INVALID_ENUM",					"An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.", },
	{ GL_INVALID_VALUE,					"GL_INVALID_VALUE",					"A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.", },
	{ GL_INVALID_OPERATION,				"GL_INVALID_OPERATION",				"The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.", },
	{ GL_INVALID_FRAMEBUFFER_OPERATION,	"GL_INVALID_FRAMEBUFFER_OPERATION",	"The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.", },
	{ GL_OUT_OF_MEMORY,					"GL_OUT_OF_MEMORY",					"There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.", },
	{ GL_STACK_UNDERFLOW,				"GL_STACK_UNDERFLOW",				"An attempt has been made to perform an operation that would cause an internal stack to underflow.", },
	{ GL_STACK_OVERFLOW,				"GL_STACK_OVERFLOW",				"An attempt has been made to perform an operation that would cause an internal stack to overflow.", },
};

const char* Gfx::getErrorCode(GLenum error)
{
	for (size_t i = 0; i < sizeof(errorDescriptionLUT) / sizeof(errorDescriptionLUT[0]); ++i)
	{
		if (errorDescriptionLUT[i].error == error)
		{
			return errorDescriptionLUT[i].errorString;
		}
	}

	return "Unkown error";
}

const char* Gfx::getErrorDescription(GLenum error)
{
	for (size_t i = 0; i < sizeof(errorDescriptionLUT) / sizeof(errorDescriptionLUT[0]); ++i)
	{
		if (errorDescriptionLUT[i].error == error)
		{
			return errorDescriptionLUT[i].description;
		}
	}

	return "Unkown error";
}
