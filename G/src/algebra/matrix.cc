//
// Matrice 4x4
//

#include "matrix.hh"
#include "quaternion.hxx"
#include "transformation.hh"
#include "vector3.hxx"
#include "vector4.hxx"

#define USE_SSE 0

#if USE_SSE
#include <emmintrin.h>
# ifdef __SSE3__
# include <pmmintrin.h>
# endif
#endif

const matrix4 matrix4::identity = {
  {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f,
  }
};

//
// Base
//
vector3f matrix4::xAxis() const
{
  return vector3f(m00, m10, m20);
}

vector3f matrix4::yAxis() const
{
  return vector3f(m01, m11, m21);
}

vector3f matrix4::zAxis() const
{
  return vector3f(m02, m12, m22);
}

//
// Transposée
//
matrix4 matrix4::transpose() const
{
  matrix4 result = {
    _0, _4,  _8, _12,
    _1, _5,  _9, _13,
    _2, _6, _10, _14,
    _3, _7, _11, _15,
  };
  return result;
}


//
// Opérateurs arithmétiques unaires
//
matrix4 & matrix4::operator += (const matrix4 & mat)
{
  for (int i = 0; i < 16; ++i) { m[i] += mat.m[i]; }
  return *this;
}

matrix4 & matrix4::operator -= (const matrix4 & mat)
{
  for (int i = 0; i < 16; ++i) { m[i] -= mat.m[i]; }
  return *this;
}

matrix4 & matrix4::operator *= (const matrix4 & mat)
{
  *this = *this * mat;
  return *this;
}

matrix4 & matrix4::operator *= (float a)
{
  for (int i = 0; i < 16; ++i) { m[i] *= a; }
  return *this;
}

matrix4 & matrix4::operator /= (float a)
{
  *this *= 1.f / a;
  return *this;
}

//
// Opérateurs arithmétiques binaires
//
matrix4	operator + (const matrix4 & lhs, const matrix4 & rhs)
{
  matrix4 result = lhs;
  result += rhs;
  return result;
}

matrix4	operator - (const matrix4 & lhs, const matrix4 & rhs)
{
  matrix4 result = lhs;
  result -= rhs;
  return result;
}

#if USE_SSE

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

matrix4 operator * (const matrix4 & a, const matrix4 & b)
{
  matrix4 result;
  mmul_sse(a.m, b.m, result.m);
  return result;
}

#else // USE_SSE

//
// Produit de deux matrices
// Rappel : A * B != B * A
//
matrix4 operator * (const matrix4 & a, const matrix4 & b)
{
  matrix4 result;

  for (unsigned int j = 0; j < 16; j += 4)
    for (unsigned int i = 0; i < 4; ++i)
    {
      result.m[j + i] = (a.m[0  + i] * b.m[j    ] +
			 a.m[4  + i] * b.m[j + 1] +
			 a.m[8  + i] * b.m[j + 2] +
			 a.m[12 + i] * b.m[j + 3]);
    }

  return result;
}
#endif // USE_SSE

matrix4 operator * (const matrix4 & lhs, float rhs)
{
  matrix4 result = lhs;
  result *= rhs;
  return result;
}

matrix4 operator / (const matrix4 & lhs, float rhs)
{
  matrix4 result = lhs;
  result /= rhs;
  return result;
}

vector4f operator * (const matrix4 & mat, const vector4f & v)
{
  return vector4f(mat.m00 * v.x + mat.m01 * v.y + mat.m02 * v.z + mat.m03 * v.w,
		  mat.m10 * v.x + mat.m11 * v.y + mat.m12 * v.z + mat.m13 * v.w,
		  mat.m20 * v.x + mat.m21 * v.y + mat.m22 * v.z + mat.m23 * v.w,
		  mat.m30 * v.x + mat.m31 * v.y + mat.m32 * v.z + mat.m33 * v.w);
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

//
// Attention, on considère que v NE SUBIT PAS la translation !
//
void applyMatrixToVector(const matrix4 & m, vector3f & v)
{
  vector4f v4 = m * vector4f(v.x, v.y, v.z, 0.f);;
  v.x = v4.x;
  v.y = v4.y;
  v.z = v4.z;
}

//
// Attention, on considère que v SUBIT la translation !
//
void applyMatrixToPoint(const matrix4 & m, point3f & p)
{
  vector4f v4 = m * vector4f(p.x, p.y, p.z, 1.f);
  p.x = v4.x;
  p.y = v4.y;
  p.z = v4.z;
}


//
// Transformations
//

matrix4 matrix4::basis(const vector3f & ux,
		       const vector3f & uy,
		       const vector3f & uz)
{
  matrix4 result = {
    ux.x, ux.y, ux.z, 0.f,
    uy.x, uy.y, uy.z, 0.f,
    uz.x, uz.y, uz.z, 0.f,
    0.f,  0.f,  0.f,  1.f,
  };
  return result;
}

matrix4 matrix4::orthonormalBasis(const vector3f & x,
				  const vector3f & y,
				  const vector3f & z)
{
  // FIXME: choisir le plus efficace/précis
  vector3f ux = x; normalize(ux);
  vector3f uy = y; normalize(ux);
  vector3f uz = cross(ux, uy);

  return basis(ux, uy, uz);
}

// FIXME: à tester
matrix4 matrix4::lookAt(const vector3f & pos,
			const vector3f & target,
			const vector3f & up)
{
  vector3f uz = target - pos; normalize(uz);
  vector3f uy = up; normalize(uy);
  vector3f ux = cross(uy, uz);

  matrix4 result = basis(ux, uy, uz);

  result.m03 = pos.x;
  result.m13 = pos.y;
  result.m23 = pos.z;

  return result;
}

// FIXME: a l'air de marcher mais à tester quand même
matrix4 matrix4::rotation(const float angle, vector3f axis)
{
  normalize(axis);

  float c = msys_cosf(angle);
  float s = msys_sinf(angle);
  float C = 1.f - c;

  float xs = axis.x * s;
  float ys = axis.y * s;
  float zs = axis.z * s;

  float xx = axis.x * axis.x;
  float yy = axis.y * axis.y;
  float zz = axis.z * axis.z;

  float xy = axis.x * axis.y;
  float yz = axis.y * axis.z;
  float zx = axis.z * axis.x;

  matrix4 result = {
    xx*C+c,   xy*C+zs,  zx*C-ys,  0,
    xy*C-zs,  yy*C+c,   yz*C+xs,  0,
    zx*C+ys,  yz*C-xs,  zz*C+c,   0,
    0,        0,        0,        1.f,
  };
  return result;
}

matrix4 matrix4::translation(const vector3f & t)
{
  matrix4 result = identity;

  result.m03 = t.x;
  result.m13 = t.y;
  result.m23 = t.z;

  return result;
}

// FIXME: à tester
matrix4 matrix4::scaling(const vector3f & s)
{
  matrix4 result = identity;

  result.m00 = s.x;
  result.m11 = s.y;
  result.m22 = s.z;

  return result;
}


// FIXME: à tester
matrix4 & matrix4::rotate(const float angle, const vector3f & axis)
{
  matrix4 mat = rotation(angle, axis);
  *this *= mat;
  return *this;
}

// FIXME: à tester
matrix4 & matrix4::translate(const vector3f & t)
{
  m03 += m00 * t.x + m01 * t.y + m02 * t.z;
  m13 += m10 * t.x + m11 * t.y + m12 * t.z;
  m23 += m20 * t.x + m21 * t.y + m22 * t.z;
  m33 += m30 * t.x + m31 * t.y + m32 * t.z;

  return *this;
}

// FIXME: à tester
matrix4 & matrix4::scale(const vector3f & s)
{
  m00 *= s.x;
  m10 *= s.x;
  m20 *= s.x;
  m30 *= s.x;

  m01 *= s.y;
  m11 *= s.y;
  m21 *= s.y;
  m31 *= s.y;

  m02 *= s.z;
  m12 *= s.z;
  m22 *= s.z;
  m32 *= s.z;

  return *this;
}
