#ifndef RIGID_TRANSFORM_HXX
#define RIGID_TRANSFORM_HXX

#include "RigidTransform.hh"
#include "engine/algebra/Interpolation.hxx"

namespace Core
{
	inline
	RigidTransform& RigidTransform::operator *= (const RigidTransform& t)
	{
		v += Algebra::rotate(q, t.v);
		q *= t.q;

		return *this;
	}

	//
	// Compare operators
	//
	inline
	bool operator == (const RigidTransform& lhs, const RigidTransform& rhs)
	{
		return ((lhs.v == rhs.v) &&
				(lhs.q == rhs.q));
	}

	inline
	bool operator != (const RigidTransform& lhs, const RigidTransform& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators
	//
	inline
	RigidTransform operator * (const RigidTransform& lhs, const RigidTransform& rhs)
	{
		RigidTransform result = lhs;
		result *= rhs;
		return result;
	}

	inline
	RigidTransform RigidTransform::rotation(float angle, Algebra::vector3f axis)
	{
		return rotation(angle, axis.x, axis.y, axis.z);
	}

	inline
	RigidTransform RigidTransform::rotation(float angle, float x, float y, float z)
	{
		RigidTransform result = { Algebra::vector3f::zero, Algebra::quaternion::rotation(angle, x, y, z) };
		return result;
	}

	inline
	RigidTransform RigidTransform::translation(const Algebra::vector3f& t)
	{
		RigidTransform result = { t, Algebra::quaternion::identity };
		return result;
	}

	inline
	RigidTransform RigidTransform::translation(float x, float y, float z)
	{
		Algebra::vector3f t = { x, y, z};
		return translation(t);
	}

	inline
	RigidTransform&	RigidTransform::rotate(float angle, const Algebra::vector3f& axis)
	{
		return rotate(angle, axis.x, axis.y, axis.z);
	}

	inline
	RigidTransform&	RigidTransform::rotate(float angle, float x, float y, float z)
	{
		RigidTransform transform = rotation(angle, x, y, z);
		*this *= transform;
		return *this;
	}

	inline
	RigidTransform&	RigidTransform::translate(const Algebra::vector3f& t)
	{
		v += t;
		return *this;
	}

	inline
	RigidTransform&	RigidTransform::translate(float x, float y, float z)
	{
		Algebra::vector3f t = { x, y, z};
		return translate(t);
	}

	inline
	RigidTransform RigidTransform::Inverse()
	{
		const Algebra::quaternion q2 = conjugate(q);
		RigidTransform result = { Algebra::rotate(q2, -v), q2 };
		return result;
	}

	inline
	Algebra::matrix4 ComputeMatrix(const RigidTransform& t)
	{
		return Algebra::computeMatrix(t.v, t.q);
	}
}

#endif // RIGID_TRANSFORM_HXX
