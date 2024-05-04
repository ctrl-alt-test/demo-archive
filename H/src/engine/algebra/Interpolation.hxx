//=============================================================================
//
// Various interpolations
//
//=============================================================================

#ifndef INTERPOLATION_HXX
#define INTERPOLATION_HXX

#include "Interpolation.hh"
#include "Functions.hh"
#include <cassert>

//#include "sys/msys.h"
#include "engine/core/msys_temp.hh"

namespace Algebra
{
	//=============================================================================
	// Linear interpolation (mix)

	template<typename T>
	T mix(const T& start, const T& end, float x)
	{
		return start + (end - start) * x;
	}

	inline
	float mix(int start, int end, float x)
	{
		return mix((float)start, (float)end, x);
	}

	//=============================================================================
	// Bicubic interpolation

	// 0 <= x <= 1
	// 0 <= value <= 1
	inline
	float smoothStep(float x)
	{
		assert(x >= 0.f && x <= 1.f);

		// f(x) = 3x^2 - 2x^3
		return x * x * (3.f - 2.f * x);
	}

	// x in 0..1 is mapped to start..end,
	// derivative is zero at start and at end.
	//
	//     0 <= x <= 1
	// start <= value <= end
	inline
	float smoothMix(float start, float end, float x)
	{
		assert(x >= 0.f && x <= 1.f);
		return mix(start, end, smoothStep(x));
	}

	//=============================================================================
	// ^5 interpolation

	// 0 <= x <= 1
	// 0 <= value <= 1
	inline
	float smootherStep(float x)
	{
		assert(x >= 0.f && x <= 1.f);

		// f(x) = 6x^5 - 15x^4 + 10x^3
		const float sqr = x * x;
		return x * sqr * (6.f * sqr - 15.f * x + 10.f);
	}

	// x in 0..1 is mapped to start..end,
	// derivative and second derivative are zero at start and at end.
	//
	//     0 <= x <= 1
	// start <= value <= end
	//
	inline
	float smootherMix(float start, float end, float x)
	{
		assert(x >= 0.f && x <= 1.f);
		return mix(start, end, smootherStep(x));
	}

	//=============================================================================

	// opposite of mix:
	//
	// min <= x <= max
	//   0 <= value <= 1 (clamped outside)
	//
	inline
	float interpolate(float min, float max, float x)
	{
		if (min == max)
		{
			return float(x > max);
		}
		return clamp((x - min) / (max - min), 0.f, 1.f);
	}

	inline
	float interpolate(int min, int max, int i)
	{
		return interpolate((float)min, (float)max, (float)i);
	}

	template<typename T>
	T interpolate(const T& a,
				  const T& b,
				  const T& c,
				  const T& d,
				  const float weights[4])
	{
		return (a * weights[0] +
				b * weights[1] +
				c * weights[2] +
				d * weights[3]);
	}

	// smoothstep, as in GLSL.
	inline
	float smoothStep(float min, float max, float x)
	{
		return smoothStep(interpolate(min, max, x));
	}

	inline
	float smoothStep(int min, int max, int i)
	{
		return smoothStep((float)min, (float)max, (float)i);
	}

	inline
	vector2f smoothStep(float min, float max, const vector2f& v)
	{
		vector2f result = {
			smoothStep(min, max, v.x),
			smoothStep(min, max, v.y),
		};
		return result;
	}

	inline
	vector3f smoothStep(float min, float max, const vector3f& v)
	{
		vector3f result = {
			smoothStep(min, max, v.x),
			smoothStep(min, max, v.y),
			smoothStep(min, max, v.z),
		};
		return result;
	}

	inline
	vector4f smoothStep(float min, float max, const vector4f& v)
	{
		vector4f result = {
			smoothStep(min, max, v.x),
			smoothStep(min, max, v.y),
			smoothStep(min, max, v.z),
			smoothStep(min, max, v.w),
		};
		return result;
	}

	inline
	float smootherStep(float min, float max, float x)
	{
		return smootherStep(interpolate(min, max, x));
	}

	inline
	float smootherStep(int min, int max, int i)
	{
		return smootherStep((float)min, (float)max, (float)i);
	}

	//=============================================================================
	// Clamp functions

	inline
	int clamp(int x, int min, int max)
	{
		if (x <= min) return min;
		if (x >= max) return max;
		return x;
	}

	inline
	float clamp(float x, float min, float max)
	{
		if (x <= min) return min;
		if (x >= max) return max;
		return x;
	}

	inline
	float clamp(float x)
	{
		return clamp(x, 0.f, 1.f);
	}

	//=============================================================================
	// Hermite spline function

	inline float h00(float x) { return 2.f*x*x*x - 3.f*x*x + 1.f; }
	inline float h10(float x) { return x*x*x - 2.f*x*x + x; }
	inline float h01(float x) { return 3.f*x*x - 2.f*x*x*x; }
	inline float h11(float x) { return x*x*x - x*x; }

	inline
	float hermite(float p0, float m0, float p1, float m1, float x)
	{
		return p0 * h00(x) + m0 * h10(x) + p1 * h01(x) + m1 * h11(x);
	}

	inline
	float hermite(float m0, float m1, float x)
	{
		return m0 * h10(x) + h01(x) + m1 * h11(x);
	}

	//=============================================================================
	// Other math functions

	// Remap the 0..1 interval into 0..1, such that the corners are remaped to 0
	// and the center to 1.
	// In other words, parabola(0) = parabola(1) = 0, and parabola(1/2) = 1.
	inline float parabola(float x, float k)
	{
		return pow(4.0f * x * (1.0f - x), k);
	}

	// Make a gaussian-like curve, such that:
	//   - if x == c, return 1.
	//   - if x < c-w || x > c+w, return 0.
	inline float cubicPulse(float c, float w, float x)
	{
		x = abs(x - c);
		if (x > w) return 0.f;
		x /= w;
		return 1.0f - x*x * (3.f - 2.f * x);
	}

	inline int sign(float x)
	{
		if (abs(x) < 0.0001f) return 0;
		return x < 0 ? -1 : 1;
	}


#if DEBUG

	// This function is useless in release.
	// roundb(f, 15) => keep 15 bits in the float, set the bits to zero
	// This is used to make floats smaller (after compression) to save some bytes.
	// Important: if you printf the result, use enough precision!
	// e.g.  printf("%.10ff", roundb(n, 16));
	inline float roundb(float f, int bits)
	{
		union { int i; float f; } num;

		bits = 32 - bits;
		num.f = f;
		int ones = -1;
		num.i = num.i & (ones << bits);
		return num.f;
	}

#endif

}

#endif // INTERPOLATION_HXX
