#ifndef VECTOR4_HXX
#define VECTOR4_HXX

#include "Vector4.hh"

#include <cassert>
#include "engine/core/msys_temp.hh"
#undef min
#undef max

namespace Algebra
{
	template<typename T> const vector4<T> vector4<T>::zero = { 0, 0, 0, 0 };
	template<typename T> const vector4<T> vector4<T>::ux = { 1, 0, 0, 0 };
	template<typename T> const vector4<T> vector4<T>::uy = { 0, 1, 0, 0 };
	template<typename T> const vector4<T> vector4<T>::uz = { 0, 0, 1, 0 };
	template<typename T> const vector4<T> vector4<T>::uw = { 0, 0, 0, 1 };

	//
	// GLSL like syntax.
	//
	inline
	vector4f vec4(float x, float y, float z, float w)
	{
		vector4f result = { x, y, z, w };
		return result;
	}

	inline
	vector4f vec4(float x)
	{
		return vec4(x, x, x, x);
	}

	//
	// Unary arithmetic operators.
	//
	template<typename T>
	inline
	vector4<T>& vector4<T>::operator += (const vector4<T>& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		return *this;
	}

	template<typename T>
	inline
	vector4<T>& vector4<T>::operator -= (const vector4<T>& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		return *this;
	}

	template<typename T>
	inline
	vector4<T>& vector4<T>::operator += (const T& a)
	{
		x += a;
		y += a;
		z += a;
		w += a;
		return *this;
	}

	template<typename T>
	inline
	vector4<T>& vector4<T>::operator -= (const T& a)
	{
		x -= a;
		y -= a;
		z -= a;
		w -= a;
		return *this;
	}

	template<typename T>
	inline
	vector4<T>& vector4<T>::operator *= (const T& a)
	{
		x *= a;
		y *= a;
		z *= a;
		w *= a;
		return *this;
	}

	template<typename T>
	inline
	vector4<T>& vector4<T>::operator /= (const T& a)
	{
		*this *= (T)1 / a;
		return *this;
	}

	//
	// Compare operators.
	//
	template<typename T>
	inline
	bool operator == (const vector4<T>& lhs, const vector4<T>& rhs)
	{
		return ((lhs.x == rhs.x) &&
				(lhs.y == rhs.y) &&
				(lhs.z == rhs.z) &&
				(lhs.w == rhs.w));
	}

	template<typename T>
	inline
	bool operator != (const vector4<T>& lhs, const vector4<T>& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators.
	//
	template<typename T>
	inline
	vector4<T> operator + (const vector4<T>& lhs, const vector4<T>& rhs)
	{
		vector4<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator - (const vector4<T>& lhs, const vector4<T>& rhs)
	{
		vector4<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator + (const vector4<T>& lhs, const T& rhs)
	{
		vector4<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator - (const vector4<T>& lhs, const T& rhs)
	{
		vector4<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator * (const vector4<T>& lhs, const T& rhs)
	{
		vector4<T> result = lhs;
		result *= rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator / (const vector4<T>& lhs, const T& rhs)
	{
		return lhs * ((T)1 / rhs);
	}

	template<typename T>
	inline
	vector4<T> operator + (const T& lhs, const vector4<T>& rhs)
	{
		return rhs + lhs;
	}

	template<typename T>
	inline
	vector4<T> operator - (const T& lhs, const vector4<T>& rhs)
	{
		vector4<T> result = { lhs, lhs };
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector4<T> operator * (const T& lhs, const vector4<T>& rhs)
	{
		return rhs * lhs;
	}

	template<typename T>
	inline
	vector4<T> operator - (const vector4<T>& v)
	{
		vector4<T> result = { -v.x, -v.y, -v.z, -v.w };
		return result;
	}

	//
	// Dot (scalar) product.
	//
	template<typename T>
	inline
	T dot(const vector4<T>& u, const vector4<T>& v)
	{
		return (u.x * v.x +
				u.y * v.y +
				u.z * v.z +
				u.w * v.w);
	}

	//
	// Member-wise product.
	//
	template<typename T>
	inline
	vector4<T> mul(const vector4<T>& u, const vector4<T>& v)
	{
		vector4<T> result = {
			u.x * v.x,
			u.y * v.y,
			u.z * v.z,
			u.w * v.w
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> div(const vector4<T>& u, const vector4<T>& v)
	{
		vector4<T> result = {
			u.x / v.x,
			u.y / v.y,
			u.z / v.z,
			u.w / v.w
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> min(const vector4<T>& u, const vector4<T>& v)
	{
		vector4<T> result = {
			msys_min(u.x, v.x),
			msys_min(u.y, v.y),
			msys_min(u.z, v.z),
			msys_min(u.w, v.w),
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> max(const vector4<T>& u, const vector4<T>& v)
	{
		vector4<T> result = {
			msys_max(u.x, v.x),
			msys_max(u.y, v.y),
			msys_max(u.z, v.z),
			msys_max(u.w, v.z),
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> min(const vector4<T>& v, float x)
	{
		vector4<T> result = {
			msys_min(v.x, x),
			msys_min(v.y, x),
			msys_min(v.z, x),
			msys_min(v.w, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> max(const vector4<T>& v, float x)
	{
		vector4<T> result = {
			msys_max(v.x, x),
			msys_max(v.y, x),
			msys_max(v.z, x),
			msys_max(v.w, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector4<T> clamp(const vector4<T>& v, float min, float max)
	{
		vector4<T> result = {
			clamp(v.x, min, max),
			clamp(v.y, min, max),
			clamp(v.z, min, max),
			clamp(v.w, min, max),
		};
		return result;
	}

	//
	// Norm.
	//
	inline
	float norm(const vector4f& v)
	{
		return msys_sqrtf(dot(v, v));
	}

	inline
	vector4f normalize(vector4f& v)
	{
		const float n = norm(v);
		assert(n != 0);
		v *= 1.f / n;
		return v;
	}

	inline
	vector4f normalized(const vector4f& v)
	{
		const float n = norm(v);
		assert(n != 0);
		return v / n;
	}

	inline
	float dist(const vector4f& u, const vector4f& v)
	{
		return norm(v - u);
	}

	inline
	float angle(vector4f u, vector4f v)
	{
		normalize(u);
		normalize(v);
		return msys_acosf(dot(u, v));
	}
}

#endif // VECTOR4_HXX
