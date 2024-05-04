#ifndef VECTOR3_HXX
#define VECTOR3_HXX

#include "Vector3.hh"

#include <cassert>
#include "engine/core/msys_temp.hh"
#undef min
#undef max

namespace Algebra
{
	template<typename T> const vector3<T> vector3<T>::zero = { 0, 0, 0 };
	template<typename T> const vector3<T> vector3<T>::ux = { 1, 0, 0 };
	template<typename T> const vector3<T> vector3<T>::uy = { 0, 1, 0 };
	template<typename T> const vector3<T> vector3<T>::uz = { 0, 0, 1 };

	//
	// GLSL like syntax.
	//
	inline
	vector3f vec3(float x, float y, float z)
	{
		vector3f result = { x, y, z };
		return result;
	}

	inline
	vector3f vec3(float x)
	{
		return vec3(x, x, x);
	}

	//
	// Unary arithmetic operators.
	//
	template<typename T>
	inline
	vector3<T>& vector3<T>::operator += (const vector3<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	template<typename T>
	inline
	vector3<T>& vector3<T>::operator -= (const vector3<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	template<typename T>
	inline
	vector3<T>& vector3<T>::operator += (const T& a)
	{
		x += a;
		y += a;
		z += a;
		return *this;
	}

	template<typename T>
	inline
	vector3<T>& vector3<T>::operator -= (const T& a)
	{
		x -= a;
		y -= a;
		z -= a;
		return *this;
	}

	template<typename T>
	inline
	vector3<T>& vector3<T>::operator *= (const T& a)
	{
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	template<typename T>
	inline
	vector3<T>& vector3<T>::operator /= (const T& a)
	{
		*this *= (T)1 / a;
		return *this;
	}

	//
	// Compare operators.
	//
	template<typename T>
	inline
	bool operator == (const vector3<T>& lhs, const vector3<T>& rhs)
	{
		return ((lhs.x == rhs.x) &&
				(lhs.y == rhs.y) &&
				(lhs.z == rhs.z));
	}

	template<typename T>
	inline
	bool operator != (const vector3<T>& lhs, const vector3<T>& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators.
	//
	template<typename T>
	inline
	vector3<T> operator + (const vector3<T>& lhs, const vector3<T>& rhs)
	{
		vector3<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator - (const vector3<T>& lhs, const vector3<T>& rhs)
	{
		vector3<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator + (const vector3<T>& lhs, const T& rhs)
	{
		vector3<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator - (const vector3<T>& lhs, const T& rhs)
	{
		vector3<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator * (const vector3<T>& lhs, const T& rhs)
	{
		vector3<T> result = lhs;
		result *= rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator / (const vector3<T>& lhs, const T& rhs)
	{
		return lhs * ((T)1 / rhs);
	}

	template<typename T>
	inline
	vector3<T> operator + (const T& lhs, const vector3<T>& rhs)
	{
		return rhs + lhs;
	}

	template<typename T>
	inline
	vector3<T> operator - (const T& lhs, const vector3<T>& rhs)
	{
		vector3<T> result = { lhs, lhs };
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector3<T> operator * (const T& lhs, const vector3<T>& rhs)
	{
		return rhs * lhs;
	}

	template<typename T>
	inline
	vector3<T> operator - (const vector3<T>& v)
	{
		vector3<T> result = { -v.x, -v.y, -v.z };
		return result;
	}

	//
	// Dot (scalar) product.
	//
	template<typename T>
	inline
	T dot(const vector3<T>& u, const vector3<T>& v)
	{
		return (u.x * v.x +
				u.y * v.y +
				u.z * v.z);
	}

	//
	// Cross (vector) product.
	//
	template<typename T>
	inline
	vector3<T> cross(const vector3<T>& u, const vector3<T>& v)
	{
		vector3<T> result = {
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x
		};
		return result;
	}

	//
	// Member-wise product.
	//
	template<typename T>
	inline
	vector3<T> mul(const vector3<T>& u, const vector3<T>& v)
	{
		vector3<T> result = {
			u.x * v.x,
			u.y * v.y,
			u.z * v.z
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> div(const vector3<T>& u, const vector3<T>& v)
	{
		vector3<T> result = {
			u.x / v.x,
			u.y / v.y,
			u.z / v.z
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> abs(const vector3<T>& v)
	{
		vector3<T> result = {abs(v.x), abs(v.y), abs(v.z)};
		return result;
	}

	template<typename T>
	inline
	vector3<T> min(const vector3<T>& u, const vector3<T>& v)
	{
		vector3<T> result = {
			msys_min(u.x, v.x),
			msys_min(u.y, v.y),
			msys_min(u.z, v.z),
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> max(const vector3<T>& u, const vector3<T>& v)
	{
		vector3<T> result = {
			msys_max(u.x, v.x),
			msys_max(u.y, v.y),
			msys_max(u.z, v.z),
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> min(const vector3<T>& v, float x)
	{
		vector3<T> result = {
			msys_min(v.x, x),
			msys_min(v.y, x),
			msys_min(v.z, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> max(const vector3<T>& v, float x)
	{
		vector3<T> result = {
			msys_max(v.x, x),
			msys_max(v.y, x),
			msys_max(v.z, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector3<T> clamp(const vector3<T>& v, float min, float max)
	{
		vector3<T> result = {
			clamp(v.x, min, max),
			clamp(v.y, min, max),
			clamp(v.z, min, max),
		};
		return result;
	}

	//
	// Norm.
	//
	inline
	float norm(const vector3f& v)
	{
		return msys_sqrtf(dot(v, v));
	}

	inline
	float dist(const vector3f& u, const vector3f& v)
	{
		return norm(v - u);
	}

	inline
	float angle(vector3f u, vector3f v)
	{
		normalize(u);
		normalize(v);
		return msys_acosf(dot(u, v));
	}
}

#endif // VECTOR3_HXX
