//
// Vecteur
//

#ifndef		VECTOR_H
# define	VECTOR_H

#include "sys/msys.h"

// ----------------------------------------------------------------------------
// Vecteur en deux dimensions

template<typename T>
struct vector2
{
  T x;
  T y;

  vector2(const T & vx, const T & vy):
    x(vx), y(vy)
  {}

  vector2() {}
};

typedef vector2<unsigned char> vector2ub;
typedef vector2<int> vector2i;
typedef vector2<float> vector2f ;

typedef vector2ub point2ub;
typedef vector2i point2i;
typedef vector2f point2f;


template<typename T>
inline
vector2<T> & operator += (vector2<T> & u, const vector2<T> & v)
{
  u.x += v.x;
  u.y += v.y;
  return u;
}

template<typename T>
inline
vector2<T> operator + (const vector2<T> & u, const vector2<T> & v)
{
  vector2<T> result = u;
  result += v;
  return result;
}

template<typename T>
inline
vector2<T> & operator -= (vector2<T> & u, const vector2<T> & v)
{
  u.x -= v.x;
  u.y -= v.y;
  return u;
}

template<typename T>
inline
vector2<T> operator - (const vector2<T> & u, const vector2<T> & v)
{
  vector2<T> result = u;
  result -= v;
  return result;
}

template<typename T>
inline
vector2<T> operator - (const vector2<T> & u)
{
  return vector2<T>(-u.x, -u.y);
}

template<typename T>
inline
vector2<T> & operator *= (vector2<T> & u, T a)
{
  u.x *= a;
  u.y *= a;
  return u;
}

template<typename T>
inline
vector2<T> operator * (const vector2<T> & u, T a)
{
  vector2<T> result = u;
  result *= a;
  return result;
}


template<typename T>
inline
T operator * (const vector2<T> & u, const vector2<T> & v)
{
  return (u.x * v.x +
	  u.y * v.y);
}

inline
float norm(const vector2f & v)
{
  return msys_sqrtf(v * v);
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

// ----------------------------------------------------------------------------
// Vecteur en trois dimensions

template<typename T>
struct vector3
{
  T x;
  T y;
  T z;

  vector3(const T & vx, const T & vy, const T & vz):
    x(vx), y(vy), z(vz)
  {}

  vector3() {}
};

typedef vector3<unsigned char> vector3ub;
typedef vector3<int> vector3i;
typedef vector3<float> vector3f;

typedef vector3ub point3ub;
typedef vector3i point3i;
typedef vector3f point3f;


template<typename T>
inline
vector3<T> & operator += (vector3<T> & u, const vector3<T> & v)
{
  u.x += v.x;
  u.y += v.y;
  u.z += v.z;
  return u;
}

template<typename T>
inline
vector3<T> operator + (const vector3<T> & u, const vector3<T> & v)
{
  vector3<T> result = u;
  result += v;
  return result;
}

template<typename T>
inline
vector3<T> & operator -= (vector3<T> & u, const vector3<T> & v)
{
  u.x -= v.x;
  u.y -= v.y;
  u.z -= v.z;
  return u;
}

template<typename T>
inline
vector3<T> operator - (const vector3<T> & u, const vector3<T> & v)
{
  vector3<T> result = u;
  result -= v;
  return result;
}

template<typename T>
inline
vector3<T> operator - (const vector3<T> & u)
{
  return vector3<T>(-u.x, -u.y, -u.z);
}

template<typename T>
inline
vector3<T> & operator *= (vector3<T> & u, T a)
{
  u.x *= a;
  u.y *= a;
  u.z *= a;
  return u;
}

template<typename T>
inline
vector3<T> operator * (const vector3<T> & u, T a)
{
  vector3<T> result = u;
  result *= a;
  return result;
}

// produit scalaire
template<typename T>
inline
T operator * (const vector3<T> & u, const vector3<T> & v)
{
  return (u.x * v.x +
	  u.y * v.y +
	  u.z * v.z);
}

// produit vectoriel
template<typename T>
inline
vector3<T> operator ^ (const vector3<T> & u, const vector3<T> & v)
{
  return vector3<T>(u.y * v.z - u.z * v.y,
		    u.z * v.x - u.x * v.z,
		    u.x * v.y - u.y * v.x);
}

inline
float norm(const vector3f & v)
{
  return msys_sqrtf(v * v);
}

inline
void normalize(vector3f & v)
{
  const float n = norm(v);
  assert(n != 0);
  v *= (1.f / n);
}

inline
vector3f mix(const vector3f & u, const vector3f & v, float x)
{
  return u + (v - u) * x;
}

inline
float angle(vector3f u, vector3f v)
{
  normalize(u);
  normalize(v);
  return msys_acosf(u * v);
}

#endif		// VECTOR_H
