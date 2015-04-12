//
// Quaternion
//

#include "sys/msys_debug.h"

#include "quaternion.hh"

quaternion quaternionFromMatrix(const float * m)
{
  quaternion q;

  const float trace = m[0] + m[5] + m[10];
  if (trace > 0)
  {
    const float s = 2.f * msys_sqrtf(1.f + trace);
    const float invS = 1.f / s;

    q.x = (m[9] - m[6]) * invS;
    q.y = (m[2] - m[8]) * invS;
    q.z = (m[4] - m[1]) * invS;
    q.w = 0.25f * s;
  }
  else if (m[0] > m[5] && m[0] > m[10])
  {
    // m[0]
    const float s = 2.f * msys_sqrtf(1.f + m[0] - m[5] - m[10]);
    const float invS = 1.f / s;

    q.x = 0.25f * s;
    q.y = (m[1] + m[4]) * invS;
    q.z = (m[2] + m[8]) * invS;
    q.w = (m[9] - m[6]) * invS;
  }
  else if (m[5] > m[10])
  {
    // m[5]
    const float s = 2.f * msys_sqrtf( 1.f - m[0] + m[5] - m[10]);
    const float invS = 1.f / s;

    q.x = (m[1] + m[4]) * invS;
    q.y = 0.25f * s;
    q.z = (m[6] + m[9]) * invS;
    q.w = (m[2] - m[8]) * invS;
  }
  else
  {
    // m[10]
    const float s = 2.f * msys_sqrtf(1.f - m[0] - m[5] + m[10]);
    const float invS = 1.f / s;

    q.x = (m[2] + m[8]) * invS;
    q.y = (m[6] + m[9]) * invS;
    q.z = 0.25f * s;
    q.w = (m[4] - m[1]) * invS;
  }

  normalize(q);

  return q;
}

//
// Matrice à partir d'un quaternion et d'une position
// (pour un objet)
//
void computeMatrix(float m[16], const quaternion & q, const vector3f & v)
{
  // Pour q unitaire :
  //
  //     |1 - 2(qy2 + qz2)   2(qx.qy - qw.qz)   2(qx.qz + qw.qy)   0|
  //     |2(qx.qy + qw.qz)   1 - 2(qx2 + qz2)   2(qy.qz - qw.qx)   0|
  // M = |2(qx.qz - qw.qy)   2(qy.qz + qw.qx)   1 - 2(qx2 + qy2)   0|
  //     |       0                  0                   0          1|
  //
  // Sinon remplacer s = 2 par s = 2 / n(q)

  const float s = 2.f;

  const float sx = s * q.x;
  const float sy = s * q.y;
  const float sz = s * q.z;
  const float sxx = sx * q.x;
  const float syy = sy * q.y;
  const float szz = sz * q.z;

  m[0] =  1.f - syy - szz;
  m[5] =  1.f - sxx - szz;
  m[10] = 1.f - sxx - syy;

  {
    const float sxy = sx * q.y;
    const float szw = sz * q.w;
    m[4] = sxy + szw;
    m[1] = sxy - szw;
  }
  {
    const float sxz = sx * q.z;
    const float syw = sy * q.w;
    m[8] = sxz - syw;
    m[2] = sxz + syw;
  }
  {
    const float syz = sy * q.z;
    const float sxw = sx * q.w;
    m[9] = syz + sxw;
    m[6] = syz - sxw;
  }

  m[3] = 0;
  m[7] = 0;
  m[11] = 0;

  m[12] = v.x;
  m[13] = v.y;
  m[14] = v.z;
  m[15] = 1.f;
}

//
// Matrice inverse à partir d'un quaternion et d'une position
// (pour une caméra)
//
void computeInvMatrix(float m[16], const quaternion & q, const vector3f & v)
{
  computeMatrix(m, q, v);
  invMatrix(m);
}

//
// Inversion de matrice
//
// DAns le cas de la rotation + translation on a :
// M = TxR
// inv(R) = transpose(R)
// inv(T) = -T
// inv(M) = inv(R) x inv(T) = -inv(R)T
//
void invMatrix(float m[16])
{
  {
    // Inversion de la matrice de rotation : transposée
    // |0 4 8 |     |0 1 2 |
    // |1 5 9 | --> |4 5 6 |
    // |2 6 10|     |8 9 10|
    //
    float temp;
    temp = m[4]; m[4] = m[1]; m[1] = temp; // FIXME : factoriser
    temp = m[8]; m[8] = m[2]; m[2] = temp;
    temp = m[9]; m[9] = m[6]; m[6] = temp;
  }

  {
    // Inversion de la translation
    // |. . . 12|
    // |. . . 13|
    // |. . . 14|
    // |. . . . |
    //
    const float vx = -m[12];
    const float vy = -m[13];
    const float vz = -m[14];
    m[12] = (vx * m[0] + vy * m[4] + vz * m[8]);
    m[13] = (vx * m[1] + vy * m[5] + vz * m[9]);
    m[14] = (vx * m[2] + vy * m[6] + vz * m[10]);
  }
}

/*
// Inutile apparemment

#if DEBUG

vector3f operator ^ (const vector3f & v1, const vector3f & v2)
{
  //   Ux   Vx   Uy.Vz - Uz.Vy
  //   Uy ^ Vy = Uz.Vx - Ux.Vz
  //   Uz   Vz   Ux.Vy - Uy.Vx

  const vector3f v = { v1.y * v2.z - v1.z * v2.y,
		       v1.z * v2.x - v1.x * v2.z,
		       v1.x * v2.y - v1.y * v2.x };
  return v;
}

void normalize(vector3f & v)
{
  const float norm = msys_sqrtf(v.x * v.x + v.y + v.y + v.z * v.z);
  const float invNorm = 1.f / norm;
  v.x *= invNorm;
  v.y *= invNorm;
  v.z *= invNorm;
}

void orthoNormalize(float m[16])
{
  vector3f vx = {m[0], m[1], m[3]};
  //  vector3f vy = {m[4], m[5], m[6]};
  vector3f vz = {m[8], m[9], m[10]};

  normalize(vz);
  normalize(vx);
  vector3f vy = vz ^ vx;

  m[0] = vx.x;
  m[1] = vy.x;
  m[2] = vz.x;

  m[4] = vx.y;
  m[5] = vy.y;
  m[6] = vz.y;

  m[8] = vx.z;
  m[9] = vy.z;
  m[10] = vz.z;
}

#endif // DEBUG
*/

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

  float cosTheta = (qa.w * qb.w +
		    qa.x * qb.x +
		    qa.y * qb.y +
		    qa.z * qb.z);

  if (msys_fabsf(cosTheta) >= 1.f)
  {
    // qa et qb sont colinéaires
    quaternion q;
    q.x = 0.5f * (qa.x + qb.x);
    q.y = 0.5f * (qa.y + qb.y);
    q.z = 0.5f * (qa.z + qb.z);
    q.w = 0.5f * (qa.w + qb.w);
    return q;
  }

  // Correction pour toujours retourner le plus petit angle
  quaternion qb_ = qb;
  if (cosTheta < 0.f)
  {
    // q et -q representent la meme rotation, donc le side-effect n'est pas genant.
    qb_.w = -qb.w;
    qb_.x = -qb.x;
    qb_.y = -qb.y;
    qb_.z = -qb.z;
    cosTheta = -cosTheta;
  }

  const float sinTheta = msys_sqrtf(1.f - cosTheta * cosTheta);

  const float invSinTheta = 1.f / sinTheta;
  const float theta = msys_acosf(cosTheta);
  const float xTheta = x * theta;

  const float ratioA = sin(theta - xTheta) * invSinTheta;
  float ratioB = sin(xTheta) * invSinTheta;

  quaternion q;
  q.x = (qa.x * ratioA + qb_.x * ratioB);
  q.y = (qa.y * ratioA + qb_.y * ratioB);
  q.z = (qa.z * ratioA + qb_.z * ratioB);
  q.w = (qa.w * ratioA + qb_.w * ratioB);

  return q;
}
