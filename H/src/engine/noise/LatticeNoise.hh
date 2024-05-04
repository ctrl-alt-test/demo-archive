#ifndef LATTICE_NOISE_HH
#define LATTICE_NOISE_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/algebra/Vector4.hh"

namespace Noise
{
	/// <summary>
	/// Lattice noise is like random noise in [-1..1], but
	/// deterministic and with control over frequency.
	/// </summary>
	class LatticeNoise
	{
	public:
		static float value(float p, float period);
		static float value(const Algebra::vector2f& p, float period);
		static float value(const Algebra::vector3f& p, float period);
		static float value(const Algebra::vector4f& p, float period);
	};
}

#endif // LATTICE_NOISE_HH
