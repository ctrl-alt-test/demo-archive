#ifndef UNIFORM_HXX
#define UNIFORM_HXX

#include "Uniform.hh"

namespace Gfx
{
	inline
	Uniform Uniform::Float1(const char* name, float x)
	{
		Uniform result = { name, 1, UniformType::Float, { { x, } }, };
		return result;
	}

	inline
	Uniform Uniform::Float2(const char* name, float x, float y)
	{
		Uniform result = { name, 2, UniformType::Float, { { x, y, } }, };
		return result;
	}

	inline
	Uniform Uniform::Float3(const char* name, float x, float y, float z)
	{
		Uniform result = { name, 3, UniformType::Float, { { x, y, z, } }, };
		return result;
	}

	inline
	Uniform Uniform::Float4(const char* name, float x, float y, float z, float w)
	{
		Uniform result = { name, 4, UniformType::Float, { { x, y, z, w, } }, };
		return result;
	}

	inline
	Uniform Uniform::Int1(const char* name, int i)
	{
		Uniform result = { name, 1, UniformType::Int, { { 0 } } };
		result.iValue[0] = i;
		return result;
	}

	inline
	Uniform Uniform::Int2(const char* name, int i, int j)
	{
		Uniform result = { name, 2, UniformType::Int, { { 0 } } };
		result.iValue[0] = i;
		result.iValue[1] = j;
		return result;
	}

	inline
	Uniform Uniform::Int3(const char* name, int i, int j, int k)
	{
		Uniform result = { name, 3, UniformType::Int, { { 0 } } };
		result.iValue[0] = i;
		result.iValue[1] = j;
		result.iValue[2] = k;
		return result;
	}

	inline
	Uniform Uniform::Int4(const char* name, int i, int j, int k, int l)
	{
		Uniform result = { name, 4, UniformType::Int, { { 0 } } };
		result.iValue[0] = i;
		result.iValue[1] = j;
		result.iValue[2] = k;
		result.iValue[3] = l;
		return result;
	}

	inline
	Uniform Uniform::Sampler1(const char* name, TextureID id)
	{
		Uniform result = { name, 1, UniformType::Sampler, { { 0 } } };
		result.id = id;
		return result;
	}
}

#endif // UNIFORM_HXX
