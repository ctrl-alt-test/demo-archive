//
// Quaternion
//

#ifndef		QUATERNION_HH
# define	QUATERNION_HH

#include "sys/msys.h"

struct quaternion
{
  float x;
  float y;
  float z;
  float w;

  quaternion(float qx, float qy, float qz, float qw):
    x(qx), y(qy), z(qz), w(qw)
  {}
  quaternion() {}
};

inline
quaternion & operator += (quaternion & q1, const quaternion & q2)
{
  q1.x += q2.x;
  q1.y += q2.y;
  q1.z += q2.z;
  q1.w += q2.w;
  return q1;
}

inline
quaternion operator + (const quaternion & q1, const quaternion & q2)
{
  quaternion result = q1;
  result += q2;
  return result;
}

inline
quaternion & operator -= (quaternion & q1, const quaternion & q2)
{
  q1.x -= q2.x;
  q1.y -= q2.y;
  q1.z -= q2.z;
  q1.w -= q2.w;
  return q1;
}

inline
quaternion operator - (const quaternion & q1, const quaternion & q2)
{
  quaternion result = q1;
  result -= q2;
  return result;
}

inline
quaternion operator - (const quaternion & q)
{
  return quaternion(-q.x, -q.y, -q.z, -q.w);
}

inline
quaternion & operator *= (quaternion & q, float a)
{
  q.x *= a;
  q.y *= a;
  q.z *= a;
  q.w *= a;
  return q;
}

inline
quaternion operator * (const quaternion & q, const float a)
{
  quaternion result = q;
  result *= a;
  return result;
}

quaternion operator * (const quaternion & a, const quaternion & b);

inline
quaternion conjugate(const quaternion & q)
{
  return quaternion(-q.x, -q.y, -q.z, q.w);
}

inline
float dot(const quaternion & a, const quaternion & b)
{
  return (a.x * b.x +
	  a.y * b.y +
	  a.z * b.z +
	  a.w * b.w);
}

inline
float norm(const quaternion & q)
{
  return msys_sqrtf(dot(q, q));
}

inline
void normalize(quaternion & q)
{
  const float n = norm(q);
  assert(n != 0);
  q *= (1.f / n);
}

inline
float dist(const quaternion & q1, const quaternion & q2)
{
  return norm(q2 - q1);
}

quaternion lerp(const quaternion & qa, const quaternion & qb, float x);
quaternion slerp(const quaternion & qa, const quaternion & qb, float x);


#if DEBUG

struct compressedQuaternion
{
  char x;
  char y;
  char z;
  char w;

  quaternion q() const
  {
    quaternion q(x, y, z, w);
    const float n = norm(q);
    if (n != 0)
    {
      q *= (1.f / n);
    }
    return q;
  }
};

void testCompressQuaternion(const quaternion & q);

compressedQuaternion compressQuaternion(const quaternion & q, bool debug = false);

#endif

#endif		// QUATERNION_HH
