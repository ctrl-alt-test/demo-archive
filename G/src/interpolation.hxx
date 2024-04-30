//=============================================================================
//
// Various interpolations
//
//=============================================================================

#ifndef		INTERPOLATION_HXX
# define	INTERPOLATION_HXX

#include "interpolation.hh"
#include "sys/msys.h"

//=============================================================================
// Linear interpolation (mix)
// (was UnsecureLinear_interpolation)


// FIXME: suggestion, on ne garde qu'un seul mix, sans clamp ni assert
//        (comme glsl).


// Identique a la fonction mix de GLSL
// state entre 0 et 1 est remis entre start et end.
//
//     0 <= state <= 1
// start <= value <= end
//
inline
float	mix(float start, float end, float state)
{
  state = clamp(state);
  return start + (end - start) * state;
}

inline
float	mixUnsafe(float start, float end, float state)
{
  assert(state >= 0.f && state <= 1.f);
  return start + (end - start) * state;
}

//=============================================================================
// Bicubic interpolation
// (was UnsecureSpline3_interpolation)

// 0 <= state <= 1
// 0 <= value <= 1
inline
float	smoothStep(float state)
{
  assert(state >= 0.f && state <= 1.f);

  // f(x) = 3x^2 - 2x^3
  return state * state * (3.f - 2.f * state);
}

// state entre 0 et 1 est remis entre start et end,
// avec la dérivée nulle en start et en end.
//
//     0 <= state <= 1
// start <= value <= end
inline
float	smoothMix(float start, float end, float state)
{
  assert(state >= 0.f && state <= 1.f);
  return mix(start, end, smoothStep(state));
}

//=============================================================================
// ^5 interpolation
// (was *Spline5_interpolation)

// 0 <= state <= 1
// 0 <= value <= 1
inline
float	smoothStep5(float state)
{
  assert(state >= 0.f && state <= 1.f);

  // f(x) = 6x^5 - 15x^4 + 10x^3
  const float sqr = state * state;
  return state * sqr * (6.f * sqr - 15.f * state + 10.f);
}

// Version améliorée du smoothstep,
// avec la dérivée seconde nulle en start et end.
//
//     0 <= state <= 1
// start <= value <= end
//
inline
float	smoothMix5(float start, float end, float state)
{
  assert(state >= 0.f && state <= 1.f);
  return mix(start, end, smoothStep5(state));
}

//=============================================================================

//
// FIXME:
// - avoir le même prototype pour interpolate et smoothInterpolate*.
// - avoir le même comportement (choisir entre clamp, assert ou rien).
// - avoir la même convention de nommage pour le int/float.
//

// contraire de mix :
// x (compris entre min et max) est remis entre 0 et 1 et clampé.
inline
float	interpolate(float x, float min, float max)
{
  assert(min != max);
  return clamp((x - min) / (max - min), 0.f, 1.f);
}

inline
float	interpolate(int x, int min, int max)
{
  return interpolate((float) x, (float) min, (float) max);
}

// smoothstep, as in GLSL.
inline
float smoothStep(float min, float max, float x)
{
  const float t = interpolate(x, min, max);
  return smoothStep(t);
}

inline
float smoothStep(int min, int max, int x)
{
  return smoothStep((float)min, (float)max, (float)x);
}

//=============================================================================
// Clamp functions

inline
int	clamp(int x, int min, int max)
{
  if (x <= min) return min;
  if (x >= max) return max;
  return x;
}

inline
float	clamp(float x, float min, float max)
{
  if (x <= min) return min;
  if (x >= max) return max;
  return x;
}

inline
float	clamp(float x)
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
float	hermite(float p0, float m0, float p1, float m1, float x)
{
  return p0 * h00(x) + m0 * h10(x) + p1 * h01(x) + m1 * h11(x);
}

inline
float	hermite(float m0, float m1, float x)
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
  return msys_powf(4.0f * x * (1.0f - x), k);
}

// Make a gaussian-like curve, such that:
//   - if x == c, return 1.
//   - if x < c-w || x > c+w, return 0.
inline float cubicPulse(float c, float w, float x)
{
  x = msys_fabsf(x - c);
  if (x > w) return 0.f;
  x /= w;
  return 1.0f - x*x * (3.f - 2.f * x);
}

inline int sign(float x)
{
  if (msys_fabsf(x) < 0.01f) return 0;
  return x < 0 ? -1 : 1;
}


#if DEBUG

// Cette fonction n'a pas de sens en release
// roundb(f, 16) => annule les 16 derniers bits de f
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

#endif		// INTERPOLATION_HXX
