#ifndef QUATERNION_HH
#define QUATERNION_HH

#include "Vector3.hh"

namespace Algebra
{
	struct matrix4;

	/// <summary>
	/// Quaternion.
	/// </summary>
	struct quaternion
	{
		float x;
		float y;
		float z;
		float w;

		static const quaternion i;
		static const quaternion j;
		static const quaternion k;
		static const quaternion identity;

		static quaternion rotation(float angle, const vector3f& axis);
		static quaternion rotation(float angle, float x, float y, float z);

		quaternion&	operator += (const quaternion& q2);
		quaternion&	operator -= (const quaternion& q2);
		quaternion&	operator *= (const quaternion& q2);
		quaternion&	operator += (float a);
		quaternion&	operator -= (float a);
		quaternion&	operator *= (float a);
		quaternion&	operator /= (float a);
	};

	bool		operator == (const quaternion& lhs, const quaternion& rhs);
	bool		operator != (const quaternion& lhs, const quaternion& rhs);

	quaternion	operator + (const quaternion& lhs, const quaternion& rhs);
	quaternion	operator - (const quaternion& lhs, const quaternion& rhs);
	quaternion	operator * (const quaternion& lhs, const quaternion& rhs);
	quaternion	operator + (const quaternion& lhs, float rhs);
	quaternion	operator - (const quaternion& lhs, float rhs);
	quaternion	operator * (const quaternion& lhs, float rhs);
	quaternion	operator / (const quaternion& lhs, float rhs);
	quaternion	operator * (float lhs, const quaternion& rhs);
	quaternion	operator - (const quaternion& q);

	quaternion	conjugate(const quaternion& q);
	float		dot(const quaternion& a, const quaternion& b);

	float		norm(const quaternion& q);
	quaternion&	normalize(quaternion& q);
	quaternion	normalized(const quaternion& q);
	float		dist(const quaternion& q1, const quaternion& q2);

	quaternion	nlerp(const quaternion& qa, const quaternion& qb, float x);
	quaternion	slerp(const quaternion& qa, const quaternion& qb, float x);

	quaternion	computeQuaternion(const matrix4& a);

	vector3f	rotate(const quaternion& q, const vector3f& v);

	struct compressedQuaternion
	{
		char x;
		char y;
		char z;
		char w;

		quaternion q() const;
	};

#if DEBUG
	compressedQuaternion	compressQuaternion(const quaternion& q, bool debug = false);
#endif // DEBUG
}

#endif // QUATERNION_HH
