#ifndef QUATERNION_HXX
#define QUATERNION_HXX

#include "Quaternion.hh"

#include <cassert>
#include "engine/core/msys_temp.hh"

namespace Algebra
{
	//
	// Unary arithmeic operators
	//
	inline
	quaternion& quaternion::operator += (const quaternion& q)
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
	}

	inline
	quaternion& quaternion::operator -= (const quaternion& q)
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
	}

	inline
	quaternion& quaternion::operator *= (const quaternion& q)
	{
		*this = *this * q;
		return *this;
	}

	inline
	quaternion& quaternion::operator += (float a)
	{
		x += a;
		y += a;
		z += a;
		w += a;
		return *this;
	}

	inline
	quaternion& quaternion::operator -= (float a)
	{
		x -= a;
		y -= a;
		z -= a;
		w -= a;
		return *this;
	}

	inline
	quaternion& quaternion::operator *= (float a)
	{
		x *= a;
		y *= a;
		z *= a;
		w *= a;
		return *this;
	}

	inline
	quaternion& quaternion::operator /= (float a)
	{
		*this *= 1.f / a;
		return *this;
	}

	inline
	quaternion quaternion::rotation(float angle, const vector3f& axis)
	{
		return rotation(angle, axis.x, axis.y, axis.z);
	}

	//
	// Compare operators
	//
	inline
	bool operator == (const quaternion& lhs, const quaternion& rhs)
	{
		return ((lhs.x == rhs.x) &&
				(lhs.y == rhs.y) &&
				(lhs.z == rhs.z) &&
				(lhs.w == rhs.w));
	}

	inline
	bool operator != (const quaternion& lhs, const quaternion& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators
	//
	inline
	quaternion operator + (const quaternion& lhs, const quaternion& rhs)
	{
		quaternion result = lhs;
		result += rhs;
		return result;
	}

	inline
	quaternion operator - (const quaternion& lhs, const quaternion& rhs)
	{
		quaternion result = lhs;
		result -= rhs;
		return result;
	}

	inline
	quaternion operator * (const quaternion& lhs, float rhs)
	{
		quaternion result = lhs;
		result *= rhs;
		return result;
	}

	inline
	quaternion operator * (float lhs, const quaternion& rhs)
	{
		return rhs * lhs;
	}

	inline
	quaternion operator / (const quaternion& lhs, float rhs)
	{
		return lhs * (1.f / rhs);
	}

	inline
	quaternion operator - (const quaternion& q)
	{
		quaternion result = { -q.x, -q.y, -q.z, -q.w };
		return result;
	}

	inline
	quaternion conjugate(const quaternion& q)
	{
		quaternion result = { -q.x, -q.y, -q.z, q.w };
		return result;
	}

	//
	// Dot (scalar) product
	//
	inline
	float dot(const quaternion& a, const quaternion& b)
	{
		return (a.x * b.x +
				a.y * b.y +
				a.z * b.z +
				a.w * b.w);
	}

	//
	// Norm
	//
	inline
	float norm(const quaternion& q)
	{
		return msys_sqrtf(dot(q, q));
	}

	inline
	quaternion& normalize(quaternion& q)
	{
		const float n = norm(q);
		assert(n != 0);
		q *= (1.f / n);
		return q;
	}

	inline
	quaternion normalized(const quaternion& q)
	{
		const float n = norm(q);
		assert(n != 0);
		return q / n;
	}

	inline
	float dist(const quaternion& a, const quaternion& b)
	{
		return norm(b - a);
	}

	inline
	quaternion nlerp(const quaternion& a, const quaternion& b, float x)
	{
		quaternion result = a * (1.f - x) + b * (dot(a, b) >= 0.f ? x : -x);
		normalize(result);
		return result;
	}

	inline
	vector3f rotate(const quaternion& q, const vector3f& v)
	{
		const quaternion v4 = { v.x, v.y, v.z, 0 };
		const quaternion rv = q * v4 * conjugate(q);
		vector3f result = { rv.x, rv.y, rv.z };
		return result;
	}

	inline
	quaternion compressedQuaternion::q() const
	{
		quaternion q = { (float)x, (float)y, (float)z, (float)w };
		const float n = norm(q);
		if (n != 0)
		{
			q *= (1.f / n);
		}
		return q;
	}
}

namespace Algebra
{
	inline
	quaternion mix(const quaternion& start,
				   const quaternion& end,
				   float x)
	{
		return slerp(start, end, x);
	}
}


#endif // QUATERNION_HXX
