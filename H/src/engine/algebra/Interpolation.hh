//=============================================================================
//
// Various interpolations
//
//=============================================================================

#ifndef INTERPOLATION_HH
#define INTERPOLATION_HH

#include "Vector2.hh"
#include "Vector3.hh"
#include "Vector4.hh"

namespace Algebra
{
	template<typename T>
	T		mix(const T& start, const T& end, float x);
	float	mix(int start, int end, float x);

	float	smoothStep(float x);
	float	smoothMix(float start, float end, float x);
	float	smootherStep(float x);
	float	smootherMix(float start, float end, float x);

	float	interpolate(float min, float max, float x);
	float	interpolate(int min, int max, int i);

	template<typename T>
	T		interpolate(const T& a,
						const T& b,
						const T& c,
						const T& d,
						const float weights[4]);

	float	smoothStep(float min, float max, float x);
	float	smoothStep(int min, int max, int i);
	vector2f	smoothStep(float min, float max, const vector2f& v);
	vector3f	smoothStep(float min, float max, const vector3f& v);
	vector4f	smoothStep(float min, float max, const vector4f& v);

	float	smootherStep(float min, float max, float x);

	float	hermite(float p0, float m0, float p1, float m1, float x);
	float	hermite(float m0, float m1, float x);

	int		clamp(int i, int min, int max);
	float	clamp(float x, float min, float max);
	float	clamp(float x);

	int		sign(float x);

#if DEBUG
	float	roundb(float f, int bits);
#endif
}

#endif // INTERPOLATION_HH
