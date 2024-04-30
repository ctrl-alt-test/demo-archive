#ifndef UNIFORM_HH
#define UNIFORM_HH

#include "ResourceID.hh"

namespace Gfx
{
	struct UniformType
	{
		enum Enum {
			Float,
			Int,
			Sampler,
		};
	};

	/// <summary>
	/// Single uniform parameter for a shader. Consists in a name, a
	///  type and a size/dimension. Int and floats will be passed as
	///  is, but samplers will need to be translated first.
	/// </summary>
	struct Uniform
	{
		const char*			name;
		int					size;
		UniformType:: Enum	type;
		union
		{
			float		fValue[16];	// Up to a 4x4 matrix.
			int			iValue[4];	// Up to a 4d int vector.
			TextureID	id;			// One texture id.
		};

		static Uniform Float1(const char* name, float x);
		static Uniform Float2(const char* name, float x, float y);
		static Uniform Float3(const char* name, float x, float y, float z);
		static Uniform Float4(const char* name, float x, float y, float z, float w);
		static Uniform Int1(const char* name, int i);
		static Uniform Int2(const char* name, int i, int j);
		static Uniform Int3(const char* name, int i, int j, int k);
		static Uniform Int4(const char* name, int i, int j, int k, int l);
		static Uniform Sampler1(const char* name, TextureID id);
	};
}

#endif // UNIFORM_HH
