//
// Quaternion
//

#ifndef		QUATERNION_HXX
# define	QUATERNION_HXX

#include "quaternion.hh"
#include "sys/msys_debug.h"
#include "sys/msys_libc.h"

inline
quaternion & quaternion::operator += (const quaternion & q)
{
  x += q.x;
  y += q.y;
  z += q.z;
  w += q.w;
  return *this;
}

inline
quaternion & quaternion::operator -= (const quaternion & q)
{
  x -= q.x;
  y -= q.y;
  z -= q.z;
  w -= q.w;
  return *this;
}

inline
quaternion & quaternion::operator *= (float a)
{
  x *= a;
  y *= a;
  z *= a;
  w *= a;
  return *this;
}

inline
quaternion & quaternion::operator /= (float a)
{
  *this *= 1.f / a;
  return *this;
}


inline
quaternion operator + (const quaternion & lhs, const quaternion & rhs)
{
  quaternion result = lhs;
  result += rhs;
  return result;
}

inline
quaternion operator - (const quaternion & lhs, const quaternion & rhs)
{
  quaternion result = lhs;
  result -= rhs;
  return result;
}

inline
quaternion operator * (const quaternion & lhs, const float rhs)
{
  quaternion result = lhs;
  result *= rhs;
  return result;
}

inline
quaternion operator * (float lhs, const quaternion & rhs)
{
  return rhs * lhs;
}

inline
quaternion operator / (const quaternion & lhs, float rhs)
{
  return lhs * (1.f / rhs);
}


inline
quaternion operator - (const quaternion & q)
{
  return quaternion(-q.x, -q.y, -q.z, -q.w);
}

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
float dist(const quaternion & a, const quaternion & b)
{
  return norm(b - a);
}

inline
quaternion lerp(const quaternion & a, const quaternion & b, float x)
{
  quaternion result = a + (b - a) * x;
  normalize(result);
  return result;
}


#if DEBUG

inline
quaternion compressedQuaternion::q() const
{
  quaternion q(x, y, z, w);
  const float n = norm(q);
  if (n != 0)
  {
    q *= (1.f / n);
  }
  return q;
}

#endif // DEBUG

#endif // QUATERNION_HXX
