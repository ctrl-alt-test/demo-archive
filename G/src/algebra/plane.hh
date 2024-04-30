//
// Définitions pour un plan
//

#ifndef		PLANE_HH
# define	PLANE_HH

#include "algebra/ray.hh"

struct plane
{
  vector3f origin;
  vector3f normal; // Toujours de norme 1

  static plane create(const vector3f & p1, const vector3f & p2, const vector3f & p3);
};

float		dist(const plane & s, const vector3f & p);

vector3f	project(const plane & s, const vector3f & p);
ray		project(const plane & s, const ray & r);

bool		intersect(const plane & s, const ray & r, vector3f& p);
ray		intersect(const plane & lhs, const plane & rhs);


struct disc
{
  plane plane;
  float radius;
};

bool		intersect(const disc & d, const ray & r, vector3f& p);

#endif // PLANE_HH
