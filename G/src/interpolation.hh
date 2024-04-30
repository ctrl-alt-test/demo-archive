//=============================================================================
//
// Various interpolations
//
//=============================================================================

#ifndef		INTERPOLATION_HH
# define	INTERPOLATION_HH

float	mix(float start, float end, float state);
float	mixUnsafe(float start, float end, float state);
float	bicubic(float state);
float	smoothMix(float start, float end, float state);
float	smoothStep5(float state);
float	smoothMix5(float start, float end, float state);

float	interpolate(int x, int min, int max);
float	interpolate(float x, float min, float max);
float	smoothStep(float min, float max, float x);
float	smoothStep(int min, int max, int x);

float	hermite(float p0, float m0, float p1, float m1, float x);
float	hermite(float m0, float m1, float x);

int	clamp(int x, int min, int max);
float	clamp(float x, float min, float max);
float	clamp(float x);

int     sign(float x);

#if DEBUG
float	roundb(float f, int bits);
#endif

#endif		// INTERPOLATION_HH
