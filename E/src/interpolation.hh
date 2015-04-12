//=============================================================================
//
// Various interpolations
//
//=============================================================================

#ifndef		INTERPOLATION_HH
# define	INTERPOLATION_HH

// Assuming state is between 0 and 1

// Between 0 and 1
inline float	UnsecureLinear_interpolation(float state);
inline float	UnsecureSpline3_interpolation(float state);
inline float	UnsecureSpline5_interpolation(float state);

// Between start and end
inline float	UnsecureLinear_interpolation(float start, float end, float state);
inline float	UnsecureSpline3_interpolation(float start, float end, float state);
inline float	UnsecureSpline5_interpolation(float start, float end, float state);

inline float	interpolate(float x, float start, float end);

// Assuming nothing

inline float	Linear_interpolation(float state);
inline float	Spline3_interpolation(float state);
inline float	Spline5_interpolation(float state);

inline float	Linear_interpolation(float start, float end, float state);
inline float	Spline3_interpolation(float start, float end, float state);
inline float	Spline5_interpolation(float start, float end, float state);
inline float	mix(float start, float end, float x);

inline int	iclamp(int state, int min, int max);
inline float	clamp(float state, float min, float max);
inline float	clamp(float state);

inline float	smoothStep(float min, float max, float x);
inline float	smoothStepI(int min, int max, int x);

#include "interpolation.cc"

#endif		// INTERPOLATION_HH
