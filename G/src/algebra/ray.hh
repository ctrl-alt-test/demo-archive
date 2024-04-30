//
// Définitions pour un rayon
//

#ifndef		RAY_HH
# define	RAY_HH

#include "algebra/vector3.hh"

struct ray
{
  vector3f origin;
  vector3f dir; // Toujours de norme 1

  static ray create(const vector3f & p1, const vector3f & p2);
};

float		dist(const ray & r, const vector3f & p);

vector3f	project(const ray & r, const vector3f & p);

#endif // RAY_HH
