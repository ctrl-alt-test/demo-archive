#ifndef VECTOR2_HXX
#define VECTOR2_HXX

#include "Vector2.hh"

#include <cassert>
#include "engine/core/msys_temp.hh"
#undef min
#undef max

namespace Algebra
{
	template<typename T> const vector2<T> vector2<T>::zero = { 0, 0 };
	template<typename T> const vector2<T> vector2<T>::ux = { 1, 0 };
	template<typename T> const vector2<T> vector2<T>::uy = { 0, 1 };

	//
	// GLSL like syntax.
	//
	inline
	vector2f vec2(float x, float y)
	{
		vector2f result = { x, y };
		return result;
	}

	inline
	vector2f vec2(float x)
	{
		return vec2(x, x);
	}

	//
	// Unary arithmetic operators.
	//
	template<typename T>
	inline
	vector2<T>& vector2<T>::operator += (const vector2<T>& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	template<typename T>
	inline
	vector2<T>& vector2<T>::operator -= (const vector2<T>& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	template<typename T>
	inline
	vector2<T>& vector2<T>::operator += (const T& a)
	{
		x += a;
		y += a;
		return *this;
	}

	template<typename T>
	inline
	vector2<T>& vector2<T>::operator -= (const T& a)
	{
		x -= a;
		y -= a;
		return *this;
	}

	template<typename T>
	inline
	vector2<T>& vector2<T>::operator *= (const T& a)
	{
		x *= a;
		y *= a;
		return *this;
	}

	template<typename T>
	inline
	vector2<T>& vector2<T>::operator /= (const T& a)
	{
		*this *= (T)1 / a;
		return *this;
	}

	//
	// Compare operators.
	//
	template<typename T>
	inline
	bool operator == (const vector2<T>& lhs, const vector2<T>& rhs)
	{
		return ((lhs.x == rhs.x) &&
				(lhs.y == rhs.y));
	}

	template<typename T>
	inline
	bool operator != (const vector2<T>& lhs, const vector2<T>& rhs)
	{
		return !(lhs == rhs);
	}

	//
	// Binary arithmetic operators.
	//
	template<typename T>
	inline
	vector2<T> operator + (const vector2<T>& lhs, const vector2<T>& rhs)
	{
		vector2<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator - (const vector2<T>& lhs, const vector2<T>& rhs)
	{
		vector2<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator + (const vector2<T>& lhs, const T& rhs)
	{
		vector2<T> result = lhs;
		result += rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator - (const vector2<T>& lhs, const T& rhs)
	{
		vector2<T> result = lhs;
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator * (const vector2<T>& lhs, const T& rhs)
	{
		vector2<T> result = lhs;
		result *= rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator / (const vector2<T>& lhs, const T& rhs)
	{
		return lhs * ((T)1 / rhs);
	}

	template<typename T>
	inline
	vector2<T> operator + (const T& lhs, const vector2<T>& rhs)
	{
		return rhs + lhs;
	}

	template<typename T>
	inline
	vector2<T> operator - (const T& lhs, const vector2<T>& rhs)
	{
		vector2<T> result = { lhs, lhs };
		result -= rhs;
		return result;
	}

	template<typename T>
	inline
	vector2<T> operator * (const T& lhs, const vector2<T>& rhs)
	{
		return rhs * lhs;
	}

	template<typename T>
	inline
	vector2<T> operator - (const vector2<T>& v)
	{
		vector2<T> result = { -v.x, -v.y };
		return result;
	}

	//
	// Dot (scalar) product.
	//
	template<typename T>
	inline
	T dot(const vector2<T>& u, const vector2<T>& v)
	{
		return (u.x * v.x +
				u.y * v.y);
	}

	//
	// Member-wise product.
	//
	template<typename T>
	inline
	vector2<T> mul(const vector2<T>& u, const vector2<T>& v)
	{
		vector2<T> result = {
			u.x * v.x,
			u.y * v.y,
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> div(const vector2<T>& u, const vector2<T>& v)
	{
		vector2<T> result = {
			u.x / v.x,
			u.y / v.y,
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> min(const vector2<T>& u, const vector2<T>& v)
	{
		vector2<T> result = {
			msys_min(u.x, v.x),
			msys_min(u.y, v.y),
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> max(const vector2<T>& u, const vector2<T>& v)
	{
		vector2<T> result = {
			msys_max(u.x, v.x),
			msys_max(u.y, v.y),
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> min(const vector2<T>& v, float x)
	{
		vector2<T> result = {
			msys_min(v.x, x),
			msys_min(v.y, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> max(const vector2<T>& v, float x)
	{
		vector2<T> result = {
			msys_max(v.x, x),
			msys_max(v.y, x),
		};
		return result;
	}

	template<typename T>
	inline
	vector2<T> clamp(const vector2<T>& v, float min, float max)
	{
		vector2<T> result = {
			clamp(v.x, min, max),
			clamp(v.y, min, max),
		};
		return result;
	}

	//
	// Norm.
	//
	inline
	float norm(const vector2f& v)
	{
		return msys_sqrtf(dot(v, v));
	}

	inline
	vector2f& normalize(vector2f& v)
	{
		const float n = norm(v);
		assert(n != 0);
		v *= 1.f / n;
		return v;
	}

	inline
	vector2f normalized(const vector2f& v)
	{
		const float n = norm(v);
		assert(n != 0);
		return v / n;
	}

	inline
	float dist(const vector2f& u, const vector2f& v)
	{
		return norm(v - u);
	}

	inline
	float angle(vector2f u, vector2f v)
	{
		normalize(u);
		normalize(v);
		return msys_acosf(dot(u, v));
	}
}

#endif // VECTOR2_HXX
