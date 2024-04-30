//
// Vecteur 2D
//

#ifndef		VECTOR2_HH
# define	VECTOR2_HH

template<typename T>
struct vector2
{
  T x;
  T y;

  vector2(const T & vx, const T & vy):
    x(vx),
    y(vy)
  {}

  vector2(const T* v):
    x(v[0]),
    y(v[1])
  {}

  vector2() {}

  static const vector2 zero;
  static const vector2 ux;
  static const vector2 uy;

  vector2<T> &	operator += (const vector2<T> & v);
  vector2<T> &	operator -= (const vector2<T> & v);
  vector2<T> &	operator *= (const T & a);
  vector2<T> &	operator /= (const T & a);
};

typedef vector2<unsigned char>	vector2ub;
typedef vector2<int>		vector2i;
typedef vector2<float>		vector2f;


template<typename T> bool	operator == (const vector2<T> & lhs, const vector2<T> & rhs);
template<typename T> bool	operator != (const vector2<T> & lhs, const vector2<T> & rhs);

template<typename T> vector2<T>	operator + (const vector2<T> & lhs, const vector2<T> & rhs);
template<typename T> vector2<T>	operator - (const vector2<T> & lhs, const vector2<T> & rhs);
template<typename T> vector2<T>	operator * (const vector2<T> & lhs, const T & rhs);
template<typename T> vector2<T>	operator / (const vector2<T> & lhs, const T & rhs);

template<typename T> vector2<T>	operator * (const T & lhs, const vector2<T> & rhs);
template<typename T> vector2<T>	operator - (const vector2<T> & v);

template<typename T> T		dot(const vector2<T> & u, const vector2<T> & v);

float		norm(const vector2f & v);
void		normalize(vector2f & v);
vector2f	mix(const vector2f & u, const vector2f & v, float x);
float		angle(vector2f u, vector2f v);

#endif // VECTOR2_HH
