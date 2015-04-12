//
// Vecteur
//

#ifndef		VECTOR_H
# define	VECTOR_H

#include "sys/msys.h"

// Vecteur en deux dimensions

typedef struct
{
  unsigned char x;
  unsigned char y;
} vector2ub;


typedef struct
{
  int x;
  int y;
} vector2i;


typedef struct
{
  float x;
  float y;
} vector2f;


// Vecteur en trois dimensions

typedef struct
{
  unsigned char x;
  unsigned char y;
  unsigned char z;
} vector3ub;

typedef struct
{
  int x;
  int y;
  int z;
} vector3i;

typedef struct
{
  float x;
  float y;
  float z;
} vector3f;

inline vector3f operator + (const vector3f & u, const vector3f & v)
{
  const vector3f ret =
    {u.x + v.x,
     u.y + v.y,
     u.z + v.z};
  return ret;
}

inline vector3f operator - (const vector3f & u, const vector3f & v)
{
  const vector3f ret =
    {u.x - v.x,
     u.y - v.y,
     u.z - v.z};
  return ret;
}

inline vector3f & operator *= (vector3f & u, const float a)
{
  u.x *= a;
  u.y *= a;
  u.z *= a;
  return u;
}

inline float operator * (const vector3f & u, const vector3f & v)
{
  return (u.x * v.x +
	  u.y * v.y +
	  u.z * v.z);
}

inline vector3f operator ^ (const vector3f & u, const vector3f & v)
{
  const vector3f ret =
    {u.y * v.z - u.z * v.y,
     u.z * v.x - u.x * v.z,
     u.x * v.y - u.y * v.x};
  return ret;
}

inline float norm(const vector3f & v)
{
  return msys_sqrtf(v * v);
}

inline void normalize(vector3f & v)
{
  const float n = norm(v);
  assert(n != 0);
  const float invNorm = 1.f / n;
  v *= invNorm;
}

void computeMatrixPointProduct(const float m[16], vector3f & v);
void computeMatrixVectorProduct(const float m[16], vector3f & v);

#endif		// VECTOR_H
