//
// 3D transformation
//

#ifndef		TRANSFORMATION_HH
# define	TRANSFORMATION_HH

#include "quaternion.hh"
#include "vector.hh"

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

void getFromModelView(Transformation & t);


#endif		// TRANSFORMATION_HH
