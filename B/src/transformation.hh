//
// 3D transformation
//

#ifndef		TRANSFORMATION_HH
# define	TRANSFORMATION_HH

#include "vector.hh"
#include "quaternion.hh"

#define DEFAULT_DOF 0.6f

struct Transformation
{
  float s; // ou fov
  float dof;
  float textureFade;
  quaternion q;
  vector3f v;
  bool use_spline;
};

#endif		// TRANSFORMATION_HH
