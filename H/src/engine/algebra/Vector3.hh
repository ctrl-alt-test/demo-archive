#ifndef VECTOR3_HH
#define	VECTOR3_HH

#undef min // conflict with some stupid Windows macro
#undef max // conflict with some stupid Windows macro

namespace Algebra
{
	template<typename T>
	/// <summary>
	/// 3D vector.
	/// </summary>
	struct vector3
	{
		T x;
		T y;
		T z;

		static const vector3 zero;
		static const vector3 ux;
		static const vector3 uy;
		static const vector3 uz;

		vector3<T>&	operator += (const vector3<T>& v);
		vector3<T>&	operator -= (const vector3<T>& v);
		vector3<T>&	operator += (const T& a);
		vector3<T>&	operator -= (const T& a);
		vector3<T>&	operator *= (const T& a);
		vector3<T>&	operator /= (const T& a);
	};

	typedef vector3<unsigned char>	vector3ub;
	typedef vector3<int>			vector3i;
	typedef vector3<float>			vector3f;

	typedef vector3ub				point3ub;
	typedef vector3i				point3i;
	typedef vector3f				point3f;

	template<typename T> bool		operator == (const vector3<T>& lhs, const vector3<T>& rhs);
	template<typename T> bool		operator != (const vector3<T>& lhs, const vector3<T>& rhs);

	template<typename T> vector3<T>	operator + (const vector3<T>& lhs, const vector3<T>& rhs);
	template<typename T> vector3<T>	operator - (const vector3<T>& lhs, const vector3<T>& rhs);
	template<typename T> vector3<T>	operator + (const vector3<T>& lhs, const T& rhs);
	template<typename T> vector3<T>	operator - (const vector3<T>& lhs, const T& rhs);
	template<typename T> vector3<T>	operator * (const vector3<T>& lhs, const T& rhs);
	template<typename T> vector3<T>	operator / (const vector3<T>& lhs, const T& rhs);
	template<typename T> vector3<T>	operator + (const T& lhs, const vector3<T>& rhs);
	template<typename T> vector3<T>	operator - (const T& lhs, const vector3<T>& rhs);
	template<typename T> vector3<T>	operator * (const T& lhs, const vector3<T>& rhs);
	template<typename T> vector3<T>	operator - (const vector3<T>& v);

	template<typename T> T			dot(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	cross(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	mul(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	div(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	abs(const vector3<T>& v);
	template<typename T> vector3<T>	min(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	max(const vector3<T>& u, const vector3<T>& v);
	template<typename T> vector3<T>	min(const vector3<T>& v, float x);
	template<typename T> vector3<T>	max(const vector3<T>& v, float x);
	template<typename T> vector3<T>	clamp(const vector3<T>& v, float min, float max);

	float		norm(const vector3f& v);
	vector3f&	normalize(vector3f& v);
	vector3f	normalized(const vector3f& v);
	float		dist(const vector3f& u, const vector3f& v);
	float		angle(vector3f u, vector3f v);
}

#endif // VECTOR3_HH
