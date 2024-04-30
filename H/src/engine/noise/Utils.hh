#ifndef NOISE_UTILS_HH
#define NOISE_UTILS_HH

#include "engine/algebra/Vector2.hh"

namespace Noise
{
	float fBmPerlin(const Algebra::vector2f& p, float frequency, int octaves, float lacunarity, float invGain);
	float fBmWorley(const Algebra::vector2f& p, float frequency, int octaves, float lacunarity, float invGain, float minPeak);
}

#endif // NOISE_UTILS_HH
