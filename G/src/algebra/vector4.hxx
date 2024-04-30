//
// Vecteur 4D
//

#ifndef		VECTOR4_HXX
# define	VECTOR4_HXX

#include "vector4.hh"

#include "sys/msys_libc.h"
#include "sys/msys_debug.h"

template<typename T> const vector4<T> vector4<T>::zero(0, 0, 0, 0);
template<typename T> const vector4<T> vector4<T>::ux(1, 0, 0, 0);
template<typename T> const vector4<T> vector4<T>::uy(0, 1, 0, 0);
template<typename T> const vector4<T> vector4<T>::uz(0, 0, 1, 0);
template<typename T> const vector4<T> vector4<T>::uw(0, 0, 0, 1);

//
// Opérateurs arithmétiques unaires
//
template<typename T>
inline
vector4<T> & vector4<T>::operator += (const vector4<T> & v)
{
  x += v.x;
  y += v.y;
  z += v.z;
  w += v.w;
  return *this;
}

template<typename T>
inline
vector4<T> & vector4<T>::operator -= (const vector4<T> & v)
{
  x -= v.x;
  y -= v.y;
  z -= v.z;
  w -= v.w;
  return *this;
}

template<typename T>
inline
vector4<T> & vector4<T>::operator *= (const T & a)
{
  x *= a;
  y *= a;
  z *= a;
  w *= a;
  return *this;
}

template<typename T>
inline
vector4<T> & vector4<T>::operator /= (const T & a)
{
  *this *= (T)1 / a;
  return *this;
}

//
// Opérateurs de comparaison
//
template<typename T>
inline
bool operator == (const vector4<T> & lhs, const vector4<T> & rhs)
{
  return ((lhs.x == rhs.x) &&
	  (lhs.y == rhs.y) &&
	  (lhs.z == rhs.z) &&
	  (lhs.w == rhs.w));
}

template<typename T>
inline
bool operator != (const vector4<T> & lhs, const vector4<T> & rhs)
{
  return !(lhs == rhs);
}

//
// Opérateurs arithmétiques binaires
//
template<typename T>
inline
vector4<T> operator + (const vector4<T> & lhs, const vector4<T> & rhs)
{
  vector4<T> result = lhs;
  result += rhs;
  return result;
}

template<typename T>
inline
vector4<T> operator - (const vector4<T> & lhs, const vector4<T> & rhs)
{
  vector4<T> result = lhs;
  result -= rhs;
  return result;
}

template<typename T>
inline
vector4<T> operator * (const vector4<T> & lhs, const T & rhs)
{
  vector4<T> result = lhs;
  result *= rhs;
  return result;
}

template<typename T>
inline
vector4<T> operator / (const vector4<T> & lhs, const T & rhs)
{
  return lhs * ((T)1 / rhs);
}


template<typename T>
inline
vector4<T> operator * (const T & lhs, const vector4<T> & rhs)
{
  return rhs * lhs;
}

template<typename T>
inline
vector4<T> operator - (const vector4<T> & v)
{
  return vector4<T>(-v.x, -v.y, -v.z, -v.w);
}


// produit scalaire
template<typename T>
inline
T dot(const vector4<T> & u, const vector4<T> & v)
{
  return (u.x * v.x +
	  u.y * v.y +
	  u.z * v.z +
	  u.w * v.w);
}

//
// Norme
//
inline
float norm(const vector4f & v)
{
  return msys_sqrtf(dot(v, v));
}

inline
void normalize(vector4f & v)
{
  const float n = norm(v);
  assert(n != 0);
  v *= (1.f / n);
}

inline
vector4f mix(const vector4f & u, const vector4f & v, float x)
{
  return u + (v - u) * x;
}

inline
float angle(vector4f u, vector4f v)
{
  normalize(u);
  normalize(v);
  return msys_acosf(dot(u, v));
}

#endif // VECTOR4_HXX
