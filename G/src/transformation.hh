//
// 3D transformation
//

#ifndef		TRANSFORMATION_HH
# define	TRANSFORMATION_HH

#include "algebra/quaternion.hh"
#include "algebra/vector3.hh"

struct matrix4;

struct Transformation
{
  quaternion q;
  vector3f v;

  Transformation(const quaternion & q, const vector3f & v):
    q(q), v(v)
  {}
  Transformation() {}
};

Transformation operator * (const Transformation & t1,
			   const Transformation & t2);

Transformation interpolate(const Transformation & t1,
			   const Transformation & t2,
			   float weight);
Transformation interpolate(const Transformation & t1,
			   const Transformation & t2,
			   const Transformation & t3,
			   const Transformation & t4,
			   float weights[4]);

Transformation computeTransformation(const matrix4 & a);

void getFromModelView(Transformation & t);


#endif		// TRANSFORMATION_HH
