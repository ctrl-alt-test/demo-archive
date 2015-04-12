//
// Quaternion
//

#ifndef		QUATERNION_HH
# define	QUATERNION_HH

#include "sys/msys.h"
#include "vector.hh"

typedef struct
{
  float x;
  float y;
  float z;
  float w;
} quaternion;

inline float dot(const quaternion & a, const quaternion & b)
{
  return (a.x * b.x +
	  a.y * b.y +
	  a.z * b.z +
	  a.w * b.w);
}

inline float norm(const quaternion & q)
{
  return msys_sqrtf(dot(q, q));
}

inline void normalize(quaternion & q)
{
  const float n = norm(q);
  assert(n != 0);
  const float invNorm = 1.f / n;
  q.x *= invNorm;
  q.y *= invNorm;
  q.z *= invNorm;
  q.w *= invNorm;
}

quaternion quaternionFromMatrix(const float * m);
void computeMatrix(float m[16], const quaternion & q, const vector3f & v);
void computeInvMatrix(float m[16], const quaternion & q, const vector3f & v);
void invMatrix(float m[16]);
quaternion slerp(const quaternion & qa, const quaternion & qb, float x);

#endif		// QUATERNION_HH
