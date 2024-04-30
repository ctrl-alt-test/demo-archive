#ifndef PARTICIPATING_MEDIUM_HH
#define PARTICIPATING_MEDIUM_HH

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector3.hh"

namespace Tool
{
	/// <summary>
	/// Describes a participating medium, like fog or underwater.
	/// </summary>
	struct ParticipatingMedium
	{
		Algebra::vector3f	color;

		/// <summary>
		/// Between 0 and N, but 1 is already a high value.
		/// Fudge value that indicates how fast objects fade to the
		/// medium color.
		/// </summary>
		float				density;

		/// <summary>
		/// Between 0 and 1.
		/// Indicates how much the medium scatters the light, causing
		/// light cones.
		/// </summary>
		float				scatteringIntensity;

		/// <summary>
		/// Between -1 and 1.
		/// 0 means the scattering is the same in all directions.
		/// 1 means it is strongest in the forward direction.
		/// -1 means it is strongest in the backward direction.
		/// </summary>
		float				forwardScatteringStrength;

		void DecodeColorAndDensity(const Algebra::vector4f& colorAndSqrtDensity)
		{
			color.x = colorAndSqrtDensity.x;
			color.y = colorAndSqrtDensity.y;
			color.z = colorAndSqrtDensity.z;
			density = colorAndSqrtDensity.w * colorAndSqrtDensity.w;
		}

		Algebra::vector3f GetBackgroundColor(float distance) const
		{
			return 0.5f * Algebra::pow(color, 1.f + density * distance);
		}
	};
}

#endif // PARTICIPATING_MEDIUM_HH
