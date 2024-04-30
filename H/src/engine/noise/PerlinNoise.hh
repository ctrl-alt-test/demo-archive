#ifndef PERLIN_NOISE_HH
#define PERLIN_NOISE_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/algebra/Vector4.hh"

namespace Noise
{
	/// <summary>
	/// Perlin noise is blurred noise in [-1..1], deterministic and
	/// with control over frequency.
	/// </summary>
	class PerlinNoise
	{
	public:
		static float value(float p, float period);
		static float value(const Algebra::vector2f& p, float period);
		static float value(const Algebra::vector3f& p, float period);
		static float value(const Algebra::vector4f& p, float period);
	};
}

#endif // PERLIN_NOISE_HH
