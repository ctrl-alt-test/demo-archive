#ifndef VECTOR4_HH
#define VECTOR4_HH

#undef min // conflict with some stupid Windows macro
#undef max // conflict with some stupid Windows macro

namespace Algebra
{
	template<typename T>
	/// <summary>
	/// 4D vector.
	/// </summary>
	struct vector4
	{
		T x;
		T y;
		T z;
		T w;

		static const vector4 zero;
		static const vector4 ux;
		static const vector4 uy;
		static const vector4 uz;
		static const vector4 uw;

		vector4<T>&	operator += (const vector4<T>& v);
		vector4<T>&	operator -= (const vector4<T>& v);
		vector4<T>&	operator += (const T& a);
		vector4<T>&	operator -= (const T& a);
		vector4<T>&	operator *= (const T& a);
		vector4<T>&	operator /= (const T& a);
	};

	typedef vector4<unsigned char>	vector4ub;
	typedef vector4<int>			vector4i;
	typedef vector4<float>			vector4f;

	template<typename T> bool		operator == (const vector4<T>& lhs, const vector4<T>& rhs);
	template<typename T> bool		operator != (const vector4<T>& lhs, const vector4<T>& rhs);

	template<typename T> vector4<T>	operator + (const vector4<T>& lhs, const vector4<T>& rhs);
	template<typename T> vector4<T>	operator - (const vector4<T>& lhs, const vector4<T>& rhs);
	template<typename T> vector4<T>	operator + (const vector4<T>& lhs, const T& rhs);
	template<typename T> vector4<T>	operator - (const vector4<T>& lhs, const T& rhs);
	template<typename T> vector4<T>	operator * (const vector4<T>& lhs, const T& rhs);
	template<typename T> vector4<T>	operator / (const vector4<T>& lhs, const T& rhs);
	template<typename T> vector4<T>	operator + (const T& lhs, const vector4<T>& rhs);
	template<typename T> vector4<T>	operator - (const T& lhs, const vector4<T>& rhs);
	template<typename T> vector4<T>	operator * (const T& lhs, const vector4<T>& rhs);
	template<typename T> vector4<T>	operator - (const vector4<T>& v);

	template<typename T> T			dot(const vector4<T>& u, const vector4<T>& v);
	template<typename T> vector4<T>	mul(const vector4<T>& u, const vector4<T>& v);
	template<typename T> vector4<T>	div(const vector4<T>& u, const vector4<T>& v);
	template<typename T> vector4<T>	min(const vector4<T>& u, const vector4<T>& v);
	template<typename T> vector4<T>	max(const vector4<T>& u, const vector4<T>& v);
	template<typename T> vector4<T>	min(const vector4<T>& v, float x);
	template<typename T> vector4<T>	max(const vector4<T>& v, float x);
	template<typename T> vector4<T>	clamp(const vector4<T>& v, float min, float max);

	float		norm(const vector4f& v);
	vector4f	normalize(vector4f& v);
	vector4f	normalized(const vector4f& v);
	float		dist(const vector4f& u, const vector4f& v);
	float		angle(vector4f u, vector4f v);
}

#endif // VECTOR4_HH
