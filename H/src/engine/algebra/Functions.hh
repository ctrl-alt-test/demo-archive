#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include "Vector2.hh"
#include "Vector3.hh"
#include "Vector4.hh"

namespace Algebra
{
	//template<typename T> vector2<T>	clamp(const vector2<T>& v, float min, float max);
	//vector2f	smoothStep(float min, float max, const vector2f& v);

	float		abs(float x);
	vector2f	abs(const vector2f& v);
	vector3f	abs(const vector3f& v);
	vector4f	abs(const vector4f& v);

	float		floor(float x);
	vector2f	floor(const vector2f& v);
	vector3f	floor(const vector3f& v);
	vector4f	floor(const vector4f& v);

	float		fract(float x);
	vector2f	fract(const vector2f& v);
	vector3f	fract(const vector3f& v);
	vector4f	fract(const vector4f& v);

	float		mod(float x, float y);
	vector2f	mod(const vector2f& u, const vector2f& v);
	vector3f	mod(const vector3f& u, const vector3f& v);
	vector4f	mod(const vector4f& u, const vector4f& v);
	vector2f	mod(const vector2f& u, float y);
	vector3f	mod(const vector3f& u, float y);
	vector4f	mod(const vector4f& u, float y);

	float		pow(float x, float y);
	vector2f	pow(const vector2f& u, const vector2f& v);
	vector3f	pow(const vector3f& u, const vector3f& v);
	vector4f	pow(const vector4f& u, const vector4f& v);
	vector2f	pow(const vector2f& u, float y);
	vector3f	pow(const vector3f& u, float y);
	vector4f	pow(const vector4f& u, float y);
}

#endif // FUNCTIONS_HH
