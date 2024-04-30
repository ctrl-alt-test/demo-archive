//
// Quaternion
//

#include "quaternion.hxx"

#include "matrix.hh"

#include "sys/msys_debug.h"

quaternion operator * (const quaternion & a, const quaternion & b)
{
  return
    quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
	       a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
	       a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
	       a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

quaternion slerp(const quaternion & a, const quaternion & b, float x)
{
  // slerp(q, r, t) = (sin(theta.(1 - t))/sin(theta)).q + (sin(theta.t)/sin(theta)).r
  // cos(theta) = qx.rx + qy.ry + qz.rz + qw.rw
  //
  if (x <= 0.f) return a;
  if (x >= 1.f) return b;

  float cosTheta = dot(a, b);

  // Correction pour toujours retourner le plus petit angle
  quaternion b_ = b;
  if (cosTheta < 0.f)
  {
    // q et -q representent la meme rotation, donc le side-effect
    // n'est pas genant
    b_ = -b;
    cosTheta = -cosTheta;
  }
  if (msys_fabsf(cosTheta) >= 0.99f)
  {
    // a et b sont suffisamment proches pour interpoler linéairement
    return a + (b - a) * x;
  }

  const float sinTheta = msys_sqrtf(1.f - cosTheta * cosTheta);
  const float theta = msys_acosf(cosTheta);

  return (a * msys_sinf(theta * (1.f - x)) + b_ * msys_sinf(theta * x)) / sinTheta;
}

quaternion computeQuaternion(const matrix4 & a)
{
  quaternion q;

  const float trace = a.m[0] + a.m[5] + a.m[10];
  if (trace > 0)
  {
    const float s = 2.f * msys_sqrtf(1.f + trace);
    const float invS = 1.f / s;

    q.x = (a.m[9] - a.m[6]) * invS;
    q.y = (a.m[2] - a.m[8]) * invS;
    q.z = (a.m[4] - a.m[1]) * invS;
    q.w = 0.25f * s;
  }
  else if (a.m[0] > a.m[5] && a.m[0] > a.m[10])
  {
    // a.m[0]
    const float s = 2.f * msys_sqrtf(1.f + a.m[0] - a.m[5] - a.m[10]);
    const float invS = 1.f / s;

    q.x = 0.25f * s;
    q.y = (a.m[1] + a.m[4]) * invS;
    q.z = (a.m[2] + a.m[8]) * invS;
    q.w = (a.m[9] - a.m[6]) * invS;
  }
  else if (a.m[5] > a.m[10])
  {
    // a.m[5]
    const float s = 2.f * msys_sqrtf(1.f - a.m[0] + a.m[5] - a.m[10]);
    const float invS = 1.f / s;

    q.x = (a.m[1] + a.m[4]) * invS;
    q.y = 0.25f * s;
    q.z = (a.m[6] + a.m[9]) * invS;
    q.w = (a.m[2] - a.m[8]) * invS;
  }
  else
  {
    // a.m[10]
    const float s = 2.f * msys_sqrtf(1.f - a.m[0] - a.m[5] + a.m[10]);
    const float invS = 1.f / s;

    q.x = (a.m[2] + a.m[8]) * invS;
    q.y = (a.m[6] + a.m[9]) * invS;
    q.z = 0.25f * s;
    q.w = (a.m[4] - a.m[1]) * invS;
  }

  normalize(q);

  return q;
}


#if DEBUG

void testCompressQuaternion(const quaternion & q)
{
  DBG("TEST TEST TEST");

  compressQuaternion(q, true);

  DBG("TEST TEST TEST");
}

#define SCALE_TESTS 2048

compressedQuaternion compressQuaternion(const quaternion & q, bool debug)
{
  compressedQuaternion bestBet = {
    char(q.x),
    char(q.y),
    char(q.z),
    char(q.w)
  };
  float bestDist = dist(q, bestBet.q());

  const float upper = 127.f / msys_max(msys_max(msys_fabsf(q.x),
				      msys_fabsf(q.y)),
				      msys_max(msys_fabsf(q.z),
				      msys_fabsf(q.w)));

  for (int i = 2; i <= SCALE_TESTS; ++i)
  {
    const float scale = upper * float(i)/float(SCALE_TESTS);
    const compressedQuaternion candidate = {
      char(scale * q.x),
      char(scale * q.y),
      char(scale * q.z),
      char(scale * q.w)
    };
    const quaternion q2 = candidate.q();
    const float candidateDist = dist(q, q2);

    if (debug)
    {
      const float error = 100.f * candidateDist;

      DBG("Erreur: %.2f%%\t {%d, %d, %d, %d}\t-> {%.2f, %.2f, %.2f, %.2f}\t/ {%.2f, %.2f, %.2f, %.2f}",
	  error,
	  candidate.x, candidate.y, candidate.z, candidate.w,
	  q2.x, q2.y, q2.z, q2.w,
	  q.x, q.y, q.z, q.w);
    }

    if (candidateDist < bestDist)
    {
      bestBet = candidate;
      bestDist = candidateDist;
    }
  }

  return bestBet;
}

#endif
