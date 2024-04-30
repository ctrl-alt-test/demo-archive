#ifndef MATRIX_HXX
#define	MATRIX_HXX

#include "Matrix.hh"
#include "Plane.hh"
#include "Vector3.hh"
#include "Vector4.hh"

namespace Algebra
{
	//
	// Base
	//
	inline
	vector3f matrix4::xAxis() const
	{
		vector3f result = { m00, m10, m20 };
		return result;
	}

	inline
	vector3f matrix4::yAxis() const
	{
		vector3f result = { m01, m11, m21 };
		return result;
	}

	inline
	vector3f matrix4::zAxis() const
	{
		vector3f result = { m02, m12, m22 };
		return result;
	}

	//
	// Transpose
	//
	inline
	matrix4 matrix4::transposed() const
	{
		matrix4 result = {{{
					_0, _4,  _8, _12,
					_1, _5,  _9, _13,
					_2, _6, _10, _14,
					_3, _7, _11, _15,
				}}};
		return result;
	}

	inline
	void transpose(matrix4& m)
	{
		m = m.transposed();
	}

	inline
	matrix4 matrix4::triviallyInverted() const
	{
		matrix4 result = *this;
		triviallyInvert(result);
		return result;
	}

	inline
	matrix4 matrix4::inverted() const
	{
		matrix4 result = *this;
		invert(result);
		return result;
	}

	inline
	matrix4& matrix4::operator *= (const matrix4& mat)
	{
		*this = *this * mat;
		return *this;
	}

	inline
	matrix4& matrix4::operator /= (float a)
	{
		*this *= 1.f / a;
		return *this;
	}

	//
	// Compare operator
	//
	inline
	bool operator != (const matrix4& lhs, const matrix4& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators
	//
	inline
	matrix4	operator + (const matrix4& lhs, const matrix4& rhs)
	{
		matrix4 result = lhs;
		result += rhs;
		return result;
	}

	inline
	matrix4	operator - (const matrix4& lhs, const matrix4& rhs)
	{
		matrix4 result = lhs;
		result -= rhs;
		return result;
	}

	inline
	matrix4 operator * (const matrix4& lhs, float rhs)
	{
		matrix4 result = lhs;
		result *= rhs;
		return result;
	}

	inline
	matrix4 operator / (const matrix4& lhs, float rhs)
	{
		matrix4 result = lhs;
		result /= rhs;
		return result;
	}

	inline
	matrix4& matrix4::rotate(float angle, const vector3f& axis)
	{
		matrix4 mat = rotation(angle, axis);
		*this *= mat;
		return *this;
	}

	inline
	matrix4& matrix4::rotate(float angle, float x, float y, float z)
	{
		vector3f axis = { x, y, z };
		return rotate(angle, axis);
	}

	inline
	matrix4& matrix4::translate(const vector3f& t)
	{
		return translate(t.x, t.y, t.z);
	}

	inline
	matrix4& matrix4::scale(const vector3f& s)
	{
		return scale(s.x, s.y, s.z);
	}

	inline
	matrix4& matrix4::scale(float s)
	{
		return scale(s, s, s);
	}

	inline
	matrix4 matrix4::rotation(float angle, float x, float y, float z)
	{
		vector3f axis = { x, y, z };
		return rotation(angle, axis);
	}

	inline
	matrix4 matrix4::translation(float x, float y, float z)
	{
		matrix4 result = identity;

		result.m03 = x;
		result.m13 = y;
		result.m23 = z;

		return result;
	}

	inline
	matrix4 matrix4::translation(const vector3f& t)
	{
		return translation(t.x, t.y, t.z);
	}

	inline
	matrix4 matrix4::scaling(float x, float y, float z)
	{
		matrix4 result = identity;

		result.m00 = x;
		result.m11 = y;
		result.m22 = z;

		return result;
	}

	inline
	matrix4 matrix4::scaling(const vector3f& s)
	{
		return scaling(s.x, s.y, s.z);
	}

	inline
	matrix4 matrix4::scaling(float s)
	{
		return scaling(s, s, s);
	}

	inline
	void applyMatrixToDirection(const matrix4& m, vector3f& v)
	{
		v = m * v;
	}
}

#endif // MATRIX_HXX
