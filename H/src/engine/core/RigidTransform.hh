#ifndef RIGID_TRANSFORM_HH
#define RIGID_TRANSFORM_HH

#include "engine/algebra/Interpolation.hh"
#include "engine/algebra/Matrix.hh"
#include "engine/algebra/Quaternion.hh"
#include "engine/algebra/Vector3.hh"

namespace Core
{
	struct RigidTransform
	{
		Algebra::vector3f	v;
		Algebra::quaternion	q;

		static RigidTransform rotation(float angle, Algebra::vector3f axis);
		static RigidTransform rotation(float angle, float x, float y, float z);
		static RigidTransform translation(const Algebra::vector3f& t);
		static RigidTransform translation(float x, float y, float z);

		RigidTransform&	rotate(float angle, const Algebra::vector3f& axis);
		RigidTransform&	rotate(float angle, float x, float y, float z);
		RigidTransform&	translate(const Algebra::vector3f& t);
		RigidTransform&	translate(float x, float y, float z);
		RigidTransform	Inverse();

		RigidTransform&	operator *= (const RigidTransform& t);
	};

	bool			operator == (const RigidTransform& lhs, const RigidTransform& rhs);
	bool			operator != (const RigidTransform& lhs, const RigidTransform& rhs);
	RigidTransform	operator * (const RigidTransform& lhs, const RigidTransform& rhs);

	void			Invert(RigidTransform& t);

	Algebra::matrix4 ComputeMatrix(const RigidTransform& t);
}

namespace Algebra
{
	template<>
	Core::RigidTransform mix(const Core::RigidTransform& start,
							 const Core::RigidTransform& end,
							 float x);
	template<>
	Core::RigidTransform interpolate(const Core::RigidTransform& camera1,
									 const Core::RigidTransform& camera2,
									 const Core::RigidTransform& camera3,
									 const Core::RigidTransform& camera4,
									 const float weights[4]);
}

#endif // RIGID_TRANSFORM_HH
