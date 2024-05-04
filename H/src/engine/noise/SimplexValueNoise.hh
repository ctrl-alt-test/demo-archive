#ifndef SIMPLEX_VALUE_NOISE_HH
#define SIMPLEX_VALUE_NOISE_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"

namespace Noise
{
	/// <summary>
	/// Simplex value noise looks like a bee nest of random values in
	/// [-1..1], blurred, deterministic and with control over
	/// frequency.
	///
	/// FIXME: 1D version is periodic; 2D and 3D versions are not (yet).
	/// </summary>
	class SimplexValueNoise
	{
	public:
		static float value(float p, float period);
		static float value(const Algebra::vector2f& p);
		static float value(const Algebra::vector3f& p);
	};
}

#endif // SIMPLEX_VALUE_NOISE_HH
