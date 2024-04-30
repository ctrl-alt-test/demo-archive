#include "Functions.hh"
#include "Vector2.hxx"
#include "Vector3.hxx"
#include "Vector4.hxx"

using namespace Algebra;

float Algebra::abs(float x)
{
	return std::abs(x);
}

vector2f Algebra::abs(const vector2f& v)
{
	vector2f result = {
		Algebra::abs(v.x),
		Algebra::abs(v.y),
	};
	return result;
}

vector3f Algebra::abs(const vector3f& v)
{
	vector3f result = {
		Algebra::abs(v.x),
		Algebra::abs(v.y),
		Algebra::abs(v.z),
	};
	return result;
}

vector4f Algebra::abs(const vector4f& v)
{
	vector4f result = {
		Algebra::abs(v.x),
		Algebra::abs(v.y),
		Algebra::abs(v.z),
		Algebra::abs(v.w),
	};
	return result;
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::floor(float x)
{
	union
	{
		float	f;
		int		i;
	} val;
	val.f = x;
	int exponent = (val.i >> 23) & 0xff; // extract the exponent field;
	int fractional_bits = 127 + 23 - exponent;
	if (fractional_bits > 23) // abs(x) < 1.0
	{
		return 0.0;
	}
	if (fractional_bits > 0)
	{
		val.i &= ~((1U << fractional_bits) - 1);
	}
	return val.f;
}

vector2f Algebra::floor(const vector2f& v)
{
	vector2f result = {
		Algebra::floor(v.x),
		Algebra::floor(v.y),
	};
	return result;
}

vector3f Algebra::floor(const vector3f& v)
{
	vector3f result = {
		Algebra::floor(v.x),
		Algebra::floor(v.y),
		Algebra::floor(v.z),
	};
	return result;
}

vector4f Algebra::floor(const vector4f& v)
{
	vector4f result = {
		Algebra::floor(v.x),
		Algebra::floor(v.y),
		Algebra::floor(v.z),
		Algebra::floor(v.w),
	};
	return result;
}

float Algebra::fract(float x)
{
	return x - Algebra::floor(x);
}

vector2f Algebra::fract(const vector2f& v)
{
	vector2f result = {
		Algebra::fract(v.x),
		Algebra::fract(v.y),
	};
	return result;
}

vector3f Algebra::fract(const vector3f& v)
{
	vector3f result = {
		Algebra::fract(v.x),
		Algebra::fract(v.y),
		Algebra::fract(v.z),
	};
	return result;
}

vector4f Algebra::fract(const vector4f& v)
{
	vector4f result = {
		Algebra::fract(v.x),
		Algebra::fract(v.y),
		Algebra::fract(v.z),
		Algebra::fract(v.w),
	};
	return result;
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::mod(float x, float y)
{
	float res;

#ifdef _WIN32
	_asm fld     dword ptr [y]
	_asm fld     dword ptr [x]
	_asm fprem
	_asm fxch    st(1)
	_asm fstp    st(0)
	_asm fstp    dword ptr [res]
#else // !_WIN32
	res = fmodf(x, y);
#endif // !_WIN32

	return res;
}

vector2f Algebra::mod(const vector2f& u, const vector2f& v)
{
	vector2f result = {
		Algebra::mod(u.x, v.x),
		Algebra::mod(u.y, v.y),
	};
	return result;
}

vector3f Algebra::mod(const vector3f& u, const vector3f& v)
{
	vector3f result = {
		Algebra::mod(u.x, v.x),
		Algebra::mod(u.y, v.y),
		Algebra::mod(u.z, v.z),
	};
	return result;
}

vector4f Algebra::mod(const vector4f& u, const vector4f& v)
{
	vector4f result = {
		Algebra::mod(u.x, v.x),
		Algebra::mod(u.y, v.y),
		Algebra::mod(u.z, v.z),
		Algebra::mod(u.w, v.w),
	};
	return result;
}

vector2f Algebra::mod(const vector2f& u, float y)
{
	vector2f result = {
		Algebra::mod(u.x, y),
		Algebra::mod(u.y, y),
	};
	return result;
}

vector3f Algebra::mod(const vector3f& u, float y)
{
	vector3f result = {
		Algebra::mod(u.x, y),
		Algebra::mod(u.y, y),
		Algebra::mod(u.z, y),
	};
	return result;
}

vector4f Algebra::mod(const vector4f& u, float y)
{
	vector4f result = {
		Algebra::mod(u.x, y),
		Algebra::mod(u.y, y),
		Algebra::mod(u.z, y),
		Algebra::mod(u.w, y),
	};
	return result;
}

// Code from Iñigo Quilez's 64k demo framework.
// http://iquilezles.org/code/framework64k/framework64k.htm
float Algebra::pow(float x, float y)
{
	if (x == 0.f)
	{
		return 0.f;
	}

	float res;

#ifdef _WIN32
	_asm fld     dword ptr [y]
	_asm fld     dword ptr [x]
	_asm fyl2x
	_asm fld1
	_asm fld     st(1)
	_asm fprem
	_asm f2xm1
	_asm faddp   st(1), st(0)
	_asm fscale
	_asm fxch
	_asm fstp    st(0)
	_asm fstp    dword ptr [res];
#else // !_WIN32
	res = std::pow(x, y);
#endif // !_WIN32

	return res;
}

vector2f Algebra::pow(const vector2f& u, const vector2f& v)
{
	vector2f result = {
		Algebra::pow(u.x, v.x),
		Algebra::pow(u.y, v.y),
	};
	return result;
}

vector3f Algebra::pow(const vector3f& u, const vector3f& v)
{
	vector3f result = {
		Algebra::pow(u.x, v.x),
		Algebra::pow(u.y, v.y),
		Algebra::pow(u.z, v.z),
	};
	return result;
}

vector4f Algebra::pow(const vector4f& u, const vector4f& v)
{
	vector4f result = {
		Algebra::pow(u.x, v.x),
		Algebra::pow(u.y, v.y),
		Algebra::pow(u.z, v.z),
		Algebra::pow(u.w, v.w),
	};
	return result;
}

vector2f Algebra::pow(const vector2f& u, float y)
{
	vector2f result = {
		Algebra::pow(u.x, y),
		Algebra::pow(u.y, y),
	};
	return result;
}

vector3f Algebra::pow(const vector3f& u, float y)
{
	vector3f result = {
		Algebra::pow(u.x, y),
		Algebra::pow(u.y, y),
		Algebra::pow(u.z, y),
	};
	return result;
}

vector4f Algebra::pow(const vector4f& u, float y)
{
	vector4f result = {
		Algebra::pow(u.x, y),
		Algebra::pow(u.y, y),
		Algebra::pow(u.z, y),
		Algebra::pow(u.w, y),
	};
	return result;
}
