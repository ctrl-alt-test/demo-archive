//
// Définitions pour un plan
//

#ifndef		PLANE_HXX
# define	PLANE_HXX

#include "algebra/plane.hh"

plane plane::create(const vector3f & p1, const vector3f & p2, const vector3f & p3)
{
  plane result;
  result.origin = p1;
  result.normal = cross(p2 - p1, p3 - p1);
  normalize(result.normal);
  return result;
}

inline
float dist(const plane & s, const vector3f & p)
{
  // Ne marche que parce que ||n|| = 1
  return dot(p - s.origin, s.normal);
  //return dist(p, project(s, p));
}

inline
vector3f project(const plane & s, const vector3f & p)
{
  return p + cross(s.origin - p, s.normal) * s.normal;
}

inline
ray project(const plane & s, const ray & r)
{
  ray result;
  result.o = project(s, r.origin);
  result.dir = project(s, r.origin + r.dir) - result.o;
  normalize(result.dir);
  return result;
}

inline
bool intersect(const plane & s, const ray & r, vector3f& p)
{
  const float dotln = dot(s.normal, r.dir);
  if (dotln < 0.000001f) return false;

  const float t = dot(s.origin - r.origin, s.normal) / dotln;
  p = r.origin + t * s.dir;
  return true;
}

inline
plane disc::plane() const
{
  plane result = { center, normal };
  return result;
}

inline
bool intersect(const disc & d, const ray & r, vector3f& p)
{
  if (intersect(d.plane, r, p) == false) return false;
  const vector cp = p - d.center;
  return dot(cp, cp) <= d.radius * d.radius;
}

inline
ray intersect(const plane & lhs, const plane & rhs)
{
  ray result;
  result.dir = cross(lhs, rhs);
  result.origin = project(lhs, rhs.origin);
  return result;
}

#endif // PLANE_HXX
