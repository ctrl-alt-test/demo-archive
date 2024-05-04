#ifndef VECTOR2_HH
#define VECTOR2_HH

#undef min // conflict with some stupid Windows macro
#undef max // conflict with some stupid Windows macro

namespace Algebra
{
	template<typename T>
	/// <summary>
	/// 2D vector.
	/// </summary>
	struct vector2
	{
		T x;
		T y;

		static const vector2 zero;
		static const vector2 ux;
		static const vector2 uy;

		vector2<T>&	operator += (const vector2<T>& v);
		vector2<T>&	operator -= (const vector2<T>& v);
		vector2<T>&	operator += (const T& a);
		vector2<T>&	operator -= (const T& a);
		vector2<T>&	operator *= (const T& a);
		vector2<T>&	operator /= (const T& a);
	};

	typedef vector2<unsigned char>	vector2ub;
	typedef vector2<int>			vector2i;
	typedef vector2<float>			vector2f;

	template<typename T> bool		operator == (const vector2<T>& lhs, const vector2<T>& rhs);
	template<typename T> bool		operator != (const vector2<T>& lhs, const vector2<T>& rhs);

	template<typename T> vector2<T>	operator + (const vector2<T>& lhs, const vector2<T>& rhs);
	template<typename T> vector2<T>	operator - (const vector2<T>& lhs, const vector2<T>& rhs);
	template<typename T> vector2<T>	operator + (const vector2<T>& lhs, const T& rhs);
	template<typename T> vector2<T>	operator - (const vector2<T>& lhs, const T& rhs);
	template<typename T> vector2<T>	operator * (const vector2<T>& lhs, const T& rhs);
	template<typename T> vector2<T>	operator / (const vector2<T>& lhs, const T& rhs);
	template<typename T> vector2<T>	operator + (const T& lhs, const vector2<T>& rhs);
	template<typename T> vector2<T>	operator - (const T& lhs, const vector2<T>& rhs);
	template<typename T> vector2<T>	operator * (const T& lhs, const vector2<T>& rhs);
	template<typename T> vector2<T>	operator - (const vector2<T>& v);

	template<typename T> T			dot(const vector2<T>& u, const vector2<T>& v);
	template<typename T> vector2<T>	mul(const vector2<T>& u, const vector2<T>& v);
	template<typename T> vector2<T>	div(const vector2<T>& u, const vector2<T>& v);
	template<typename T> vector2<T>	min(const vector2<T>& u, const vector2<T>& v);
	template<typename T> vector2<T>	max(const vector2<T>& u, const vector2<T>& v);
	template<typename T> vector2<T>	min(const vector2<T>& v, float x);
	template<typename T> vector2<T>	max(const vector2<T>& v, float x);
	template<typename T> vector2<T>	clamp(const vector2<T>& v, float min, float max);

	float		norm(const vector2f& v);
	vector2f&	normalize(vector2f& v);
	vector2f	normalized(const vector2f& v);
	float		dist(const vector2f& u, const vector2f& v);
	float		angle(vector2f u, vector2f v);
}

#endif // VECTOR2_HH
