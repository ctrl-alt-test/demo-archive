#ifndef WORLEY_NOISE_HH
#define WORLEY_NOISE_HH

#include "engine/algebra/Vector2.hh"

namespace Noise
{
	struct VoronoiInfo
	{
		float				f1;
		float				f2;
		float				f3;
		float				f4;
		unsigned int		id;
		Algebra::vector2f	pos;
		float				edge;

		float GetHash() const { return float(id) / float(0xffffffff); }
	};

	class WorleyNoise
	{
	public:
		static VoronoiInfo value(const Algebra::vector2f& p, float period, float randomness, float norm);
	};
}

#endif // WORLEY_NOISE_HH
