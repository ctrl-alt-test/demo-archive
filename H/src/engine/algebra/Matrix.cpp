#include "Matrix.hxx"
#include "Vector3.hxx"
#include "Quaternion.hxx"
#include "engine/container/Algorithm.hxx"
// #include "Transformation.hh"

#define USE_SSE 0

#if USE_SSE
#include <emmintrin.h>
# ifdef __SSE3__
# include <pmmintrin.h>
# endif
#endif

using namespace Algebra;

const matrix4 matrix4::identity = {{{
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		}}};

//
// Unary arithmetic operators
//
matrix4& matrix4::operator += (const matrix4& mat)
{
	for (int i = 0; i < 16; ++i)
	{
		m[i] += mat.m[i];
	}
	return *this;
}

matrix4& matrix4::operator -= (const matrix4& mat)
{
	for (int i = 0; i < 16; ++i)
	{
		m[i] -= mat.m[i];
	}
	return *this;
}

matrix4& matrix4::operator *= (float a)
{
	for (int i = 0; i < 16; ++i)
	{
		m[i] *= a;
	}
	return *this;
}

//
// Compare operator
//
bool Algebra::operator == (const matrix4& lhs, const matrix4& rhs)
{
	for (int i = 0; i < 16; ++i)
	{
		if (lhs.m[i] != rhs.m[i])
		{
			return false;
		}
	}
	return true;
}

#if USE_SSE

//
// FIXME: plug SSE code
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

matrix4 Algebra::operator * (const matrix4& a, const matrix4& b)
{
	matrix4 result;
	mmul_sse(a.m, b.m, result.m);
	return result;
}

#else // !USE_SSE

//
// Matrix multiplication
// Reminder: A * B != B * A
//
matrix4 Algebra::operator * (const matrix4& a, const matrix4& b)
{
	matrix4 result;

	// SIZE-CODING:
	// Writing in this obfuscated way saves ~19 bytes on the compressed
	// binary, compared to the naive version after.
#if 1
	const matrix4 t = a.transposed();
	for (int i = 0; i < 16; ++i)
	{
		int j = (i&3)<<2;
		int k = (i>>2)<<2;
		result.m[i] = (t.m[j    ] * b.m[k    ] +
					   t.m[j + 1] * b.m[k + 1] +
					   t.m[j + 2] * b.m[k + 2] +
					   t.m[j + 3] * b.m[k + 3]);
	}
#else
	for (unsigned int j = 0; j < 16; j += 4)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			result.m[j + i] = (a.m[i     ] * b.m[j    ] +
							   a.m[i + 4 ] * b.m[j + 1] +
							   a.m[i + 8 ] * b.m[j + 2] +
							   a.m[i + 12] * b.m[j + 3]);
		}
	}
#endif

	return result;
}
#endif // !USE_SSE

vector4f Algebra::operator * (const matrix4& mat, const vector4f& v)
{
	vector4f result = {
		mat.m00 * v.x + mat.m01 * v.y + mat.m02 * v.z + mat.m03 * v.w,
		mat.m10 * v.x + mat.m11 * v.y + mat.m12 * v.z + mat.m13 * v.w,
		mat.m20 * v.x + mat.m21 * v.y + mat.m22 * v.z + mat.m23 * v.w,
		mat.m30 * v.x + mat.m31 * v.y + mat.m32 * v.z + mat.m33 * v.w
	};
	return result;
}

vector3f Algebra::operator * (const matrix4& mat, const vector3f& v)
{
	vector3f result = {
		mat.m00 * v.x + mat.m01 * v.y + mat.m02 * v.z,
		mat.m10 * v.x + mat.m11 * v.y + mat.m12 * v.z,
		mat.m20 * v.x + mat.m21 * v.y + mat.m22 * v.z
	};
	return result;
}

//
// Matrix from a vector (translation) and a quaternion (rotation).
// (rigid transformation)
//
matrix4 Algebra::computeMatrix(const vector3f& v, const quaternion& q)
{
	// When q is a unit quaternion:
	//
	//     |1 - 2qy2 - 2qz2   2qx.qy - 2qw.qz   2qx.qz + 2qw.qy   0|
	//     |2qx.qy + 2qw.qz   1 - 2qx2 - 2qz2   2qy.qz - 2qw.qx   0|
	// M = |2qx.qz - 2qw.qy   2qy.qz + 2qw.qx   1 - 2qx2 - 2qy2   0|
	//     |       0                 0                 0          1|
	//
	// Otherwise remplace s = 2 with s = 2 / n(q)

	const float sx = 2.0f * q.x;
	const float sy = 2.0f * q.y;
	const float sz = 2.0f * q.z;

	const float sxx = sx * q.x;
	const float syy = sy * q.y;
	const float szz = sz * q.z;
	const float sxw = sx * q.w;
	const float sxy = sx * q.y;
	const float sxz = sx * q.z;
	const float syw = sy * q.w;
	const float syz = sy * q.z;
	const float szw = sz * q.w;

	matrix4 result;
	result._0 = 1.f - syy - szz;
	result._1 = sxy + szw;
	result._2 = sxz - syw;
	result._3 = 0.f;

	result._4 = sxy - szw;
	result._5 = 1.f - sxx - szz;
	result._6 = syz + sxw;
	result._7 = 0.f;

	result._8 = sxz + syw;
	result._9 = syz - sxw;
	result._10 = 1.f - sxx - syy;
	result._11 = 0.f;

	result._12 = v.x;
	result._13 = v.y;
	result._14 = v.z;
	result._15 = 1.f;

	return result;
}

/*
//
// Invert matrix of a solid transformation
// (for a camera)
//
matrix4 computeInvertedMatrix(const Transformation & t)
{
return computeMatrix(t).triviallyInverted();
}
*/

//
// Invert a matrix representing a rotation and translation
// [  /!\  Will give garbage in other cases!  ]
//
// In the case rotation + translation:
// M = TxR
// inv(R) = transpose(R)
// inv(T) = -T
// inv(M) = inv(R) x inv(T) = -inv(R)T
//

// FIXME support scaling
//
// Invert a matrix representing a scaling, rotation and translation
// [  /!\  Will give garbage in other cases!  ]
//
// In the case scaling + rotation + translation:
// M = TxRxS
// inv(S) =                 <===   ???
// inv(R) = transpose(R)
// inv(T) = -T
// inv(M) = inv(S) x inv(R) x inv(T) = inv(S) x -inv(R)T
//
// How to deduce scaling?
//
void Algebra::triviallyInvert(matrix4& a)
{
	// Inverting the rotation: transpose
	// |0 4 8 |     |0 1 2 |
	// |1 5 9 | --> |4 5 6 |
	// |2 6 10|     |8 9 10|
	Container::swap(a._4, a._1);
	Container::swap(a._8, a._2);
	Container::swap(a._9, a._6);

	// Inverting the translation
	// |. . . 12|
	// |. . . 13|
	// |. . . 14|
	// |. . . . |
	const float vx = -a._12;
	const float vy = -a._13;
	const float vz = -a._14;
	a._12 = (vx * a._0 + vy * a._4 + vz * a._8);
	a._13 = (vx * a._1 + vy * a._5 + vz * a._9);
	a._14 = (vx * a._2 + vy * a._6 + vz * a._10);
}

void Algebra::invert(matrix4& a)
{
	float xx = a._0;
	float xy = a._1;
	float xz = a._2;
	float xw = a._3;

	float yx = a._4;
	float yy = a._5;
	float yz = a._6;
	float yw = a._7;

	float zx = a._8;
	float zy = a._9;
	float zz = a._10;
	float zw = a._11;

	float wx = a._12;
	float wy = a._13;
	float wz = a._14;
	float ww = a._15;

	float det = 0.f;
	det += xx * (yy*(zz*ww - zw*wz) - yz*(zy*ww - zw*wy) + yw*(zy*wz - zz*wy) );
	det -= xy * (yx*(zz*ww - zw*wz) - yz*(zx*ww - zw*wx) + yw*(zx*wz - zz*wx) );
	det += xz * (yx*(zy*ww - zw*wy) - yy*(zx*ww - zw*wx) + yw*(zx*wy - zy*wx) );
	det -= xw * (yx*(zy*wz - zz*wy) - yy*(zx*wz - zz*wx) + yz*(zx*wy - zy*wx) );

	assert(det != 0.f);
	float invDet = 1.0f / det;

	a._0  = +(yy * (zz*ww - wz*zw) - yz * (zy*ww - wy*zw) + yw * (zy*wz - wy*zz)) * invDet;
	a._1  = -(xy * (zz*ww - wz*zw) - xz * (zy*ww - wy*zw) + xw * (zy*wz - wy*zz)) * invDet;
	a._2  = +(xy * (yz*ww - wz*yw) - xz * (yy*ww - wy*yw) + xw * (yy*wz - wy*yz)) * invDet;
	a._3  = -(xy * (yz*zw - zz*yw) - xz * (yy*zw - zy*yw) + xw * (yy*zz - zy*yz)) * invDet;

	a._4  = -(yx * (zz*ww - wz*zw) - yz * (zx*ww - wx*zw) + yw * (zx*wz - wx*zz)) * invDet;
	a._5  = +(xx * (zz*ww - wz*zw) - xz * (zx*ww - wx*zw) + xw * (zx*wz - wx*zz)) * invDet;
	a._6  = -(xx * (yz*ww - wz*yw) - xz * (yx*ww - wx*yw) + xw * (yx*wz - wx*yz)) * invDet;
	a._7  = +(xx * (yz*zw - zz*yw) - xz * (yx*zw - zx*yw) + xw * (yx*zz - zx*yz)) * invDet;

	a._8  = +(yx * (zy*ww - wy*zw) - yy * (zx*ww - wx*zw) + yw * (zx*wy - wx*zy)) * invDet;
	a._9  = -(xx * (zy*ww - wy*zw) - xy * (zx*ww - wx*zw) + xw * (zx*wy - wx*zy)) * invDet;
	a._10 = +(xx * (yy*ww - wy*yw) - xy * (yx*ww - wx*yw) + xw * (yx*wy - wx*yy)) * invDet;
	a._11 = -(xx * (yy*zw - zy*yw) - xy * (yx*zw - zx*yw) + xw * (yx*zy - zx*yy)) * invDet;

	a._12 = -(yx * (zy*wz - wy*zz) - yy * (zx*wz - wx*zz) + yz * (zx*wy - wx*zy)) * invDet;
	a._13 = +(xx * (zy*wz - wy*zz) - xy * (zx*wz - wx*zz) + xz * (zx*wy - wx*zy)) * invDet;
	a._14 = -(xx * (yy*wz - wy*yz) - xy * (yx*wz - wx*yz) + xz * (yx*wy - wx*yy)) * invDet;
	a._15 = +(xx * (yy*zz - zy*yz) - xy * (yx*zz - zx*yz) + xz * (yx*zy - zx*yy)) * invDet;
}

//
// Transformations
//
matrix4 matrix4::basis(const vector3f& ux,
					   const vector3f& uy,
					   const vector3f& uz)
{
	matrix4 result = {{{
				ux.x, ux.y, ux.z, 0.f,
				uy.x, uy.y, uy.z, 0.f,
				uz.x, uz.y, uz.z, 0.f,
				0.f,  0.f,  0.f,  1.f,
			}}};
	return result;
}

matrix4 matrix4::orthonormalBasis(const vector3f& x,
								  const vector3f& y,
								  const vector3f& /*z unused*/)
{
	// FIXME: choose the most efficient/precise?
	const vector3f ux = normalized(x);
	const vector3f uy = normalized(y);
	const vector3f uz = cross(ux, uy);

	return basis(ux, uy, uz);
}

// FIXME: test it
matrix4 matrix4::lookAt(const vector3f& pos,
						const vector3f& target,
						const vector3f& up)
{
	const vector3f uz = normalized(target - pos);
	const vector3f uy = normalized(up);
	const vector3f ux = cross(uy, uz);

	matrix4 result = basis(ux, uy, uz);

	result.m03 = pos.x;
	result.m13 = pos.y;
	result.m23 = pos.z;

	return result;
}

matrix4 matrix4::ortho(float left, float right,
					   float top, float bottom,
					   float near, float far)
{
	matrix4 result = {{{
				2.0f/(right-left), 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f/(top-bottom), 0.0f, 0.0f,
				0.0f, 0.0f, -2.0f/(far-near),  0.0f,
				-(right+left)/(right-left) , -(top+bottom)/(top-bottom), -(far+near)/(far-near), 1.0f,
			}}};
	return result;
}

matrix4 matrix4::perspective(float fov, float aspectRatio, float near, float far)
{
	const float top = near * msys_tanf(0.5f * fov);
	const float right = top * aspectRatio;
	return frustum(-right, right, -top, top, near, far);
}

matrix4 matrix4::frustum(float left, float right,
						 float bottom, float top,
						 float near, float far)
{
	const float q = -(far + near) / (far - near);
	const float qn = -2.0f * (far * near) / (far - near);
	const float w = 2.0f * near / (right-left);
	const float h = 2.0f * near / (top-bottom);

	matrix4 result = {{{
				w,    0.0f, 0.0f, 0.0f,
				0.0f, h,    0.0f, 0.0f,
				0.0f, 0.0f, q,   -1.0f,
				0.0f, 0.0f, qn,   0.0f,
			}}};
	return result;
}

matrix4 matrix4::rotation(float angle, vector3f axis)
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

	matrix4 result = {{{
				xx*C+c,   xy*C+zs,  zx*C-ys,  0,
				xy*C-zs,  yy*C+c,   yz*C+xs,  0,
				zx*C+ys,  yz*C-xs,  zz*C+c,   0,
				0,        0,        0,        1.f,
			}}};
	return result;
}

matrix4& matrix4::translate(float x, float y, float z)
{
	m03 += m00 * x + m01 * y + m02 * z;
	m13 += m10 * x + m11 * y + m12 * z;
	m23 += m20 * x + m21 * y + m22 * z;
	m33 += m30 * x + m31 * y + m32 * z;

	return *this;
}

matrix4& matrix4::scale(float x, float y, float z)
{
	m00 *= x;
	m10 *= x;
	m20 *= x;
	m30 *= x;

	m01 *= y;
	m11 *= y;
	m21 *= y;
	m31 *= y;

	m02 *= z;
	m12 *= z;
	m22 *= z;
	m32 *= z;

	return *this;
}

void Algebra::applyMatrixToPoint(const matrix4& m, point3f& p)
{
	vector3f result = m * p;
	result.x += m.m03;
	result.y += m.m13;
	result.z += m.m23;
	p = result;
}

void Algebra::applyMatrixToPlane(const matrix4& m, plane& s)
{
	vector3f n = s.normal;
	applyMatrixToDirection(m.triviallyInverted().transposed(), n);

	vector3f o = s.normal * s.dist;
	applyMatrixToPoint(m, o);

	s.normal = n;
	s.dist = dot(n, o);
}
