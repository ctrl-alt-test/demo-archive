#ifndef VERTEX_ATTRIBUTE_HH
#define VERTEX_ATTRIBUTE_HH

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32
#include <GL/gl.h>

namespace Gfx
{
	struct VertexAttributeType
	{
		enum Enum {
			Byte,
			UnsignedByte,
			Int,
			Float,
		};
	};

	struct VertexIndexType
	{
		enum Enum {
			UInt8 = GL_UNSIGNED_BYTE,
			UInt16 = GL_UNSIGNED_SHORT,
			UInt32 = GL_UNSIGNED_INT,
		};
	};

	/// <summary>
	/// A vertex attribute description. Consists in a name, a type and
	/// a dimension.
	/// </summary>
	struct VertexAttribute
	{
		const char*					name;
		int							num;
		VertexAttributeType::Enum	type;
	};
}

#endif // VERTEX_ATTRIBUTE_HH
