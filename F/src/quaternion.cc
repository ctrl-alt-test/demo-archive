//
// Quaternion
//

#include "sys/msys_debug.h"

#include "quaternion.hh"

quaternion operator * (const quaternion & a, const quaternion & b)
{
  return
    quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
	       a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
	       a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
	       a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

quaternion lerp(const quaternion & qa, const quaternion & qb, float x)
{
  quaternion result = qa + (qb - qa) * x;
  normalize(result);
  return result;
}

quaternion slerp(const quaternion & qa, const quaternion & qb, float x)
{
  // slerp(q, r, t) = (sin(theta.(1 - t))/sin(theta)).q + (sin(theta.t)/sin(theta)).r
  // cos(theta) = qx.rx + qy.ry + qz.rz + qw.rw
  //
  if (x <= 0)
  {
    return qa;
  }
  if (x >= 1.f)
  {
    return qb;
  }

  float cosTheta = dot(qa, qb);

  if (msys_fabsf(cosTheta) >= 0.99f)
  {
    // qa et qb sont pratiquement colinéaires
    return (qa + qb) * 0.5f;
  }

  // Correction pour toujours retourner le plus petit angle
  quaternion qb_ = qb;
  if (cosTheta < 0.f)
  {
    // q et -q representent la meme rotation, donc le side-effect
    // n'est pas genant
    qb_ = -qb;
    cosTheta = -cosTheta;
  }

  const float invSinTheta = 1.f / msys_sqrtf(1.f - cosTheta * cosTheta);
  const float theta = msys_acosf(cosTheta);
  const float xTheta = x * theta;

  const float ratioA = msys_sinf(theta - xTheta) * invSinTheta;
  float ratioB = msys_sinf(xTheta) * invSinTheta;

  return qa * ratioA + qb_ * ratioB;
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

  const float upper = 127.f / max(max(msys_fabsf(q.x),
				      msys_fabsf(q.y)),
				  max(msys_fabsf(q.z),
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
