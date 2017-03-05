//=============================================================================
//
// Various interpolations
//
//=============================================================================

#include "sys/msys_debug.h"

inline
float	UnsecureLinear_interpolation(float state)
{
  assert(state >= 0 && state <= 1.f);
  return state;
}

inline
float	UnsecureSpline3_interpolation(float state)
{
  assert(state >= 0 && state <= 1.f);

  // Basic spline :
  // (3x^2 - 2x^3)
  return state * state * (3.f - 2.f * state);
}

inline
float	UnsecureSpline5_interpolation(float state)
{
  assert(state >= 0 && state <= 1.f);

  // Enhanced spline :
  // (3x^2 + 2x^3) is not as good as (6x^5 - 15x^4 + 10x^3)
  const float sqr = state * state;
  return state * sqr * (6.f * sqr - 15.f * state + 10.f);
}

// Identique a la fonction mix de GLSL
// x entre 0 et 1 est remis entre start et end.
inline
float	mix(float start, float end, float x)
{
  return start + (end - start) * x;
}

// contraire de mix :
// x (compris entre min et max) est remis entre 0 et 1
inline
float	interpolate(float x, float min, float max)
{
  assert(min != max);
  assert((x >= min && x <= max) || (x <= min && x >= max));
  return (x - min) / (max - min);
}

inline
float	UnsecureLinear_interpolation(float start, float end, float state)
{
  assert(state >= 0 && state <= 1.f);
  return mix(start, end, UnsecureLinear_interpolation(state));
}

inline
float	UnsecureSpline3_interpolation(float start, float end, float state)
{
  assert(state >= 0 && state <= 1.f);
  return mix(start, end, UnsecureSpline3_interpolation(state));
}

inline
float	UnsecureSpline5_interpolation(float start, float end, float state)
{
  assert(state >= 0 && state <= 1.f);
  return mix(start, end, UnsecureSpline5_interpolation(state));
}

inline
int	iclamp(int state, int min, int max)
{
  if (state <= min)
    return min;
  if (state >= max)
    return max;
  return state;
}

inline
float	clamp(float state, float min, float max)
{
  if (state <= min)
    return min;
  if (state >= max)
    return max;
  return state;
}

inline
float	clamp(float state)
{
  return clamp(state, 0.f, 1.f);
}

inline
float	Linear_interpolation(float state)
{
  return UnsecureLinear_interpolation(clamp(state));
}

inline
float	Spline3_interpolation(float state)
{
  return UnsecureSpline3_interpolation(clamp(state));
}

inline
float	Spline5_interpolation(float state)
{
  return UnsecureSpline5_interpolation(clamp(state));
}

// Identique a la fonction smoothStep en GLSL
inline
float smoothStep(float min, float max, float x)
{
  assert(min < max);
  if (x <= min) return 0.f;
  if (x >= max) return 1.f;
  const float t = interpolate(x, min, max);
  return UnsecureSpline3_interpolation(t);
}

inline
float smoothStepI(int min, int max, int x)
{
  return smoothStep((float)min, (float)max, (float)x);
}

inline
float	Linear_interpolation(float start, float end, float state)
{
  return UnsecureLinear_interpolation(start, end, clamp(state));
}

inline
float	Spline3_interpolation(float start, float end, float state)
{
  return UnsecureSpline3_interpolation(start, end, clamp(state));
}

inline
float	Spline5_interpolation(float start, float end, float state)
{
  return UnsecureSpline5_interpolation(start, end, clamp(state));
}

//=============================================================================

// Other math functions (move somewhere else?)

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
