#include "RigidTransform.hxx"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Quaternion.hxx"

template<>
Core::RigidTransform Algebra::mix(const Core::RigidTransform& start,
								  const Core::RigidTransform& end,
								  float x)
{
	const Core::RigidTransform result = {
		mix(start.v, end.v, x),
		slerp(start.q, end.q, x),
	};
	return result;
}

template<>
Core::RigidTransform Algebra::interpolate(const Core::RigidTransform& t1,
										  const Core::RigidTransform& t2,
										  const Core::RigidTransform& t3,
										  const Core::RigidTransform& t4,
										  const float weights[4])
{
	Algebra::vector3f v = (t1.v * weights[0] +
						   t2.v * weights[1] +
						   t3.v * weights[2] +
						   t4.v * weights[3]);
	Algebra::quaternion q = normalized(t1.q * weights[0] +
									   t2.q * weights[1] +
									   t3.q * weights[2] +
									   t4.q * weights[3]);
	Core::RigidTransform result = { v, q };
	return result;
}
