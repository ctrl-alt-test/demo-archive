//
// Vecteur 4D
//

#ifndef		VECTOR4_HH
# define	VECTOR4_HH

template<typename T>
struct vector4
{
  T x;
  T y;
  T z;
  T w;

  vector4(const T & vx, const T & vy, const T & vz, const T & vw):
    x(vx),
    y(vy),
    z(vz),
    w(vw)
  {}

  vector4(const T* v):
    x(v[0]),
    y(v[1]),
    z(v[2]),
    w(v[3])
  {}

  vector4() {}

  static const vector4 zero;
  static const vector4 ux;
  static const vector4 uy;
  static const vector4 uz;
  static const vector4 uw;

  vector4<T> &	operator += (const vector4<T> & v);
  vector4<T> &	operator -= (const vector4<T> & v);
  vector4<T> &	operator *= (const T & a);
  vector4<T> &	operator /= (const T & a);
};

typedef vector4<unsigned char>	vector4ub;
typedef vector4<int>		vector4i;
typedef vector4<float>		vector4f;


template<typename T> bool	operator == (const vector4<T> & lhs, const vector4<T> & rhs);
template<typename T> bool	operator != (const vector4<T> & lhs, const vector4<T> & rhs);

template<typename T> vector4<T>	operator + (const vector4<T> & lhs, const vector4<T> & rhs);
template<typename T> vector4<T>	operator - (const vector4<T> & lhs, const vector4<T> & rhs);
template<typename T> vector4<T>	operator * (const vector4<T> & lhs, const T & rhs);
template<typename T> vector4<T>	operator / (const vector4<T> & lhs, const T & rhs);

template<typename T> vector4<T>	operator * (const T & lhs, const vector4<T> & rhs);
template<typename T> vector4<T>	operator - (const vector4<T> & v);

template<typename T> T		dot(const vector4<T> & u, const vector4<T> & v);

float		norm(const vector4f & v);
void		normalize(vector4f & v);
vector4f	mix(const vector4f & u, const vector4f & v, float x);
float		angle(vector4f u, vector4f v);

#endif // VECTOR4_HH
