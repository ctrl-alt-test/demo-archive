//
// 3D transformation
//

#include "transformation.hh"

#include "interpolation.hh"
#include "algebra/matrix.hh"
#include "algebra/quaternion.hxx"
#include "algebra/vector3.hxx"

inline
static vector3f _transform(const quaternion & q, const vector3f & v)
{
  const float xx = q.x * q.x;
  const float yy = q.y * q.y;
  const float zz = q.z * q.z;
  const float ww = q.w * q.w;
  const float sxy = 2.f * q.x * q.y;
  const float sxz = 2.f * q.x * q.z;
  const float sxw = 2.f * q.x * q.w;
  const float syz = 2.f * q.y * q.z;
  const float syw = 2.f * q.y * q.w;
  const float szw = 2.f * q.z * q.w;

  return
    vector3f(v.x * (xx - yy - zz + ww) + v.y * (sxy - szw)          + v.z * (sxz + syw),
	     v.x * (sxy + szw)         + v.y * (-xx + yy - zz + ww) + v.z * (-sxw + syz),
	     v.x * (sxz - syw)         + v.y * (sxw + syz)          + v.z * (-xx - yy + zz + ww));
}

//
// FIXME : vérifier que ce n'est pas fumeux
//
Transformation operator * (const Transformation & t1, const Transformation & t2)
{
  return Transformation(t1.q * t2.q,
			t1. v + _transform(t1.q, t2.v));
}

Transformation interpolate(const Transformation & t1,
			   const Transformation & t2,
			   float weight)
{
  assert(weight >= 0 && weight <= 1.f);

  return Transformation(slerp(t1.q, t2.q, weight),
			mix(t1.v, t2.v, weight));
}

Transformation interpolate(const Transformation & t1,
			   const Transformation & t2,
			   const Transformation & t3,
			   const Transformation & t4,
			   float weights[4])
{
  // FIXME : slerp avec 4 poids
  quaternion q = t1.q * weights[0] +
		 t2.q * weights[1] +
		 t3.q * weights[2] +
		 t4.q * weights[3];
  normalize(q);
  Transformation t(q,
		   t1.v * weights[0] +
		   t2.v * weights[1] +
		   t3.v * weights[2] +
		   t4.v * weights[3]);
  normalize(t.q);

  return t;
}

Transformation computeTransformation(const matrix4 & a)
{
  const quaternion q = computeQuaternion(a);
  const vector3f v(a.m[12], a.m[13], a.m[14]);
  return Transformation(q, v);
}

void getFromModelView(Transformation & t)
{
  matrix4 matrix;
  getFromModelView(matrix);

  t = computeTransformation(matrix);
}
