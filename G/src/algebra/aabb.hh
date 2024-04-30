//
// Définitions pour les Axis Aligned Bounding Boxes
//

#ifndef		AABB_HH
# define	AABB_HH

#include "algebra/vector3.hh"

struct range
{
  float min;
  float max;

  range();
  range(float min, float max);

  float center() const;
  float size() const;

  bool contains(float x) const;
  bool contains(const range& r) const;
  bool intersects(const range & r) const;

  void expand(float x);
};

struct AABB
{
  vector3f min;
  vector3f max;

  vector3f center() const;
  vector3f size() const;

  range xRange() const;
  range yRange() const;
  range zRange() const;

  bool contains(const vector3f & p) const;
  bool contains(const AABB& box) const;
  bool intersects(const AABB & box) const;

  void expand(const vector3f& p);
};

#endif // AABB_HH
