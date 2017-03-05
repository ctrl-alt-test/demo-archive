//
// Matrice 4x4
//

#include "sys/msys_debug.h"

#include "matrix.hh"

#define USE_SSE 0

#if USE_SSE
#include <emmintrin.h>
#ifdef __SSE3__
# include <pmmintrin.h>
#endif

//
// FIXME : brancher les SSE
//

//
// http://fhtr.blogspot.com/2010/02/4x4-float-matrix-multiplication-using.html
//
inline
static void mmul_sse(const float * a, const float * b, float * r)
{
  __m128 a_line, b_line, r_line;
  for (int i = 0; i < 16; i += 4)
  {
    // unroll the first step of the loop to avoid having to initialize
    // r_line to zero
    a_line = _mm_load_ps(a);         // a_line = vec4(column(a, 0))
    b_line = _mm_set1_ps(b[i]);      // b_line = vec4(b[i][0])
    r_line = _mm_mul_ps(a_line, b_line); // r_line = a_line * b_line
    for (int j = 1; j < 4; ++j)
    {
      a_line = _mm_load_ps(&a[j*4]); // a_line = vec4(column(a, j))
      b_line = _mm_set1_ps(b[i+j]);  // b_line = vec4(b[i][j])
                                     // r_line += a_line * b_line
      r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);
    }
    _mm_store_ps(&r[i], r_line);     // r[i] = r_line
  }
}

#endif

//
// Produit de deux matrices
// Rappel : A * B != B * A
//
matrix4 operator * (const matrix4 & a, const matrix4 & b)
{
  matrix4 result;

#if USE_SSE

  mmul_sse(a.m, b.m, result.m);

#else

  for (unsigned int j = 0; j < 16; j += 4)
    for (unsigned int i = 0; i < 4; ++i)
    {
      result.m[j + i] = (a.m[0  + i] * b.m[j    ] +
			 a.m[4  + i] * b.m[j + 1] +
			 a.m[8  + i] * b.m[j + 2] +
			 a.m[12 + i] * b.m[j + 3]);
    }

#endif // USE_SSE

  return result;
}

//
// Matrice à partir d'un quaternion et d'une position
// (pour un objet)
//
matrix4 computeMatrix(const Transformation & t)
{
  matrix4 ret;

  const quaternion & q = t.q;
  const vector3f & v = t.v;

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

  ret.m[0] =  1.f - syy - szz;
  ret.m[5] =  1.f - sxx - szz;
  ret.m[10] = 1.f - sxx - syy;

  {
    const float sxy = sx * q.y;
    const float szw = sz * q.w;
    ret.m[4] = sxy + szw;
    ret.m[1] = sxy - szw;
  }
  {
    const float sxz = sx * q.z;
    const float syw = sy * q.w;
    ret.m[8] = sxz - syw;
    ret.m[2] = sxz + syw;
  }
  {
    const float syz = sy * q.z;
    const float sxw = sx * q.w;
    ret.m[9] = syz + sxw;
    ret.m[6] = syz - sxw;
  }

  ret.m[3] = 0;
  ret.m[7] = 0;
  ret.m[11] = 0;

  ret.m[12] = v.x;
  ret.m[13] = v.y;
  ret.m[14] = v.z;
  ret.m[15] = 1.f;

  //
  // FIXME : ajouter le scaling !
  //

  return ret;
}

//
// Matrice inverse d'une transformation
// (pour une caméra)
//
matrix4 computeInvMatrix(const Transformation & t)
{
  matrix4 result = computeMatrix(t);
  invMatrix(result);
  return result;
}

//
// Inversion d'une matrice de rotation + translation
// [  /!\  Fera de la merde dans les autres cas !  ]
//
// Dans le cas de la rotation + translation on a :
// M = TxR
// inv(R) = transpose(R)
// inv(T) = -T
// inv(M) = inv(R) x inv(T) = -inv(R)T
//

// FIXME : Modification en cours, gestion du scaling
//
// Inversion d'une matrice de scaling + rotation + translation
// [  /!\  Fera de la merde dans les autres cas !  ]
//
// Dans le cas du scaling + rotation + translation on a :
// M = TxRxS
// inv(S) = ???       <===   FIXME
// inv(R) = transpose(R)
// inv(T) = -T
// inv(M) = inv(S) x inv(R) x inv(T) = inv(S) x -inv(R)T
//
// FIXME : comment on déduit le scaling ?
//
void invMatrix(matrix4 & a)
{
  {
    // Inversion de la matrice de rotation : transposée
    // |0 4 8 |     |0 1 2 |
    // |1 5 9 | --> |4 5 6 |
    // |2 6 10|     |8 9 10|
    //
    float temp;
    temp = a.m[4]; a.m[4] = a.m[1]; a.m[1] = temp; // FIXME : factoriser
    temp = a.m[8]; a.m[8] = a.m[2]; a.m[2] = temp;
    temp = a.m[9]; a.m[9] = a.m[6]; a.m[6] = temp;
  }

  {
    // Inversion de la translation
    // |. . . 12|
    // |. . . 13|
    // |. . . 14|
    // |. . . . |
    //
    const float vx = -a.m[12];
    const float vy = -a.m[13];
    const float vz = -a.m[14];
    a.m[12] = (vx * a.m[0] + vy * a.m[4] + vz * a.m[8]);
    a.m[13] = (vx * a.m[1] + vy * a.m[5] + vz * a.m[9]);
    a.m[14] = (vx * a.m[2] + vy * a.m[6] + vz * a.m[10]);
  }
}

/*
// Inutile apparemment

void orthoNormalize(float m[16])
{
  vector3f vx(m[0], m[1], m[3]);
  //  vector3f vy(m[4], m[5], m[6]);
  vector3f vz(m[8], m[9], m[10]);

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
*/


static quaternion _computeQuaternion(const matrix4 & a)
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

Transformation computeTransformation(const matrix4 & a)
{
  const quaternion q = _computeQuaternion(a);
  const vector3f v(a.m[12], a.m[13], a.m[14]);
  return Transformation(q, v);
}

//
// Attention, on considère que v NE SUBIT PAS la translation !
//
void applyMatrixToVector(const matrix4 & m, vector3f & v)
{
  const float x = v.x * m.m[0] + v.y * m.m[4] + v.z * m.m[8];
  const float y = v.x * m.m[1] + v.y * m.m[5] + v.z * m.m[9];
  const float z = v.x * m.m[2] + v.y * m.m[6] + v.z * m.m[10];
  v.x = x;
  v.y = y;
  v.z = z;
}

//
// Attention, on considère que v SUBIT la translation !
//
void applyMatrixToPoint(const matrix4 & m, vector3f & v)
{
  applyMatrixToVector(m, v);
  v.x += m.m[12];
  v.y += m.m[13];
  v.z += m.m[14];
}

// http://en.wikipedia.org/wiki/Rotation_matrix#Axis_and_angle
matrix4 rotationMatrix(const vector3f & a, const float angle)
{
  vector3f axis = a;
  if (norm(a) > 0.f) normalize(axis);
  float c = msys_cosf(angle);
  float s = msys_sinf(angle);
  float C = 1.f - c;
  float x = axis.x;
  float y = axis.y;
  float z = axis.z;

  matrix4 m = { x*x*C+c,   x*y*C-z*s, x*z*C+y*s, 0,
		y*x*C+z*s, y*y*C+c,   y*z*C-x*s, 0,
		z*x*C-y*s, z*y*C+x*s, z*z*C+c,   0,
		0,         0,         0,         1.f};
  return m;
}

matrix4 rotationMatrix(const vector3f & Xaxis,
		       const vector3f & Yaxis,
		       const vector3f & Zaxis)
{
  matrix4 m = {
    Xaxis.x, Xaxis.y, Xaxis.z, 0,
    Yaxis.x, Yaxis.y, Yaxis.z, 0,
    Zaxis.x, Zaxis.y, Zaxis.z, 0,
    0,       0,       0,       1.f
  };
  return m;
}
