//
// Vecteur 2D
//

#ifndef		VECTOR2_HXX
# define	VECTOR2_HXX

#include "vector2.hh"

#include "sys/msys_libc.h"
#include "sys/msys_debug.h"

template<typename T> const vector2<T> vector2<T>::zero(0, 0);
template<typename T> const vector2<T> vector2<T>::ux(1, 0);
template<typename T> const vector2<T> vector2<T>::uy(0, 1);

//
// Opérateurs arithmétiques unaires
//
template<typename T>
inline
vector2<T> & vector2<T>::operator += (const vector2<T> & v)
{
  x += v.x;
  y += v.y;
  return *this;
}

template<typename T>
inline
vector2<T> & vector2<T>::operator -= (const vector2<T> & v)
{
  x -= v.x;
  y -= v.y;
  return *this;
}

template<typename T>
inline
vector2<T> & vector2<T>::operator *= (const T & a)
{
  x *= a;
  y *= a;
  return *this;
}

template<typename T>
inline
vector2<T> & vector2<T>::operator /= (const T & a)
{
  *this *= (T)1 / a;
  return *this;
}

//
// Opérateurs de comparaison
//
template<typename T>
inline
bool operator == (const vector2<T> & lhs, const vector2<T> & rhs)
{
  return ((lhs.x == rhs.x) &&
	  (lhs.y == rhs.y));
}

template<typename T>
inline
bool operator != (const vector2<T> & lhs, const vector2<T> & rhs)
{
  return !(lhs == rhs);
}

//
// Opérateurs arithmétiques binaires
//
template<typename T>
inline
vector2<T> operator + (const vector2<T> & lhs, const vector2<T> & rhs)
{
  vector2<T> result = lhs;
  result += rhs;
  return result;
}

template<typename T>
inline
vector2<T> operator - (const vector2<T> & lhs, const vector2<T> & rhs)
{
  vector2<T> result = lhs;
  result -= rhs;
  return result;
}

template<typename T>
inline
vector2<T> operator * (const vector2<T> & lhs, const T & rhs)
{
  vector2<T> result = lhs;
  result *= rhs;
  return result;
}

template<typename T>
inline
vector2<T> operator / (const vector2<T> & lhs, const T & rhs)
{
  return lhs * ((T)1 / rhs);
}


template<typename T>
inline
vector2<T> operator * (const T & lhs, const vector2<T> & rhs)
{
  return rhs * lhs;
}

template<typename T>
inline
vector2<T> operator - (const vector2<T> & v)
{
  return vector2<T>(-v.x, -v.y);
}


//
// Produit scalaire
//
template<typename T>
inline
T dot(const vector2<T> & u, const vector2<T> & v)
{
  return (u.x * v.x +
	  u.y * v.y);
}

//
// Norme
//
inline
float norm(const vector2f & v)
{
  return msys_sqrtf(dot(v, v));
}

inline
void normalize(vector2f & v)
{
  const float n = norm(v);
  assert(n != 0);
  v *= 1.f / n;
}

inline
vector2f mix(const vector2f & u, const vector2f & v, float x)
{
  return u + (v - u) * x;
}

inline
float angle(vector2f u, vector2f v)
{
  normalize(u);
  normalize(v);
  return msys_acosf(dot(u, v));
}

#endif // VECTOR2_HXX
