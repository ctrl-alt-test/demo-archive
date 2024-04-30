//
// Définitions pour les Axis Aligned Bounding Boxes
//

#ifndef		AABB_HXX
# define	AABB_HXX

#include "aabb.hh"
#include "sys/msys_libc.h"

#define BIG_FLOAT 524288.f

inline
range::range()
{
  min = BIG_FLOAT;
  max = -BIG_FLOAT;
}

inline
range::range(float _min, float _max)
{
  min = _min;
  max = _max;
}

inline
float range::center() const
{
  return 0.5f * (min + max);
}

inline
float range::size() const
{
  return max - min;
}

inline
bool range::contains(float x) const
{
  return x >= min && x <= max;
}

inline
bool range::contains(const range& r) const
{
  return contains(r.min) && contains(r.max);
}

inline
bool range::intersects(const range& r) const
{
  return contains(r.min) || contains(r.max);
}

inline
void range::expand(float x)
{
  min = msys_min(min, x);
  max = msys_max(max, x);
}



inline
vector3f AABB::center() const
{
  return 0.5f * (min + max);
}

inline
vector3f AABB::size() const
{
  return max - min;
}

inline range AABB::xRange() const { return range(min.x, max.x); }
inline range AABB::yRange() const { return range(min.y, max.y); }
inline range AABB::zRange() const { return range(min.z, max.z); }


inline
bool AABB::contains(const vector3f& p) const
{
  return (xRange().contains(p.x) &&
	  yRange().contains(p.y) &&
	  zRange().contains(p.z));
}

inline
bool AABB::contains(const AABB& box) const
{
  return (xRange().contains(box.xRange()) &&
	  yRange().contains(box.yRange()) &&
	  zRange().contains(box.zRange()));
}

inline
bool AABB::intersects(const AABB& box) const
{
  return (xRange().intersects(box.xRange()) &&
	  yRange().intersects(box.yRange()) &&
	  zRange().intersects(box.zRange()));
}

inline
void AABB::expand(const vector3f& p)
{
  min.x = msys_min(min.x, p.x);
  min.y = msys_min(min.y, p.y);
  min.z = msys_min(min.z, p.z);

  max.x = msys_max(max.x, p.x);
  max.y = msys_max(max.y, p.y);
  max.z = msys_max(max.z, p.z);
}

#endif // AABB_HXX
