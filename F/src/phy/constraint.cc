// Includes

#include "../sys/msys_debug.h"
#include "constraint.hh"



namespace phy
{


  namespace
  {

    void
    cstf_default(Particle* p1, Particle* p2, float& d, float n, const vector3f& dv)
    {
      assert(p1->mobility() || p2->mobility());
      d /= (p1->mobility() + p2->mobility());
      d /= n;

      p1->move_of(dv * d *  p1->mobility());
      p2->move_of(dv * d * -p2->mobility());
    }

  }


  // Constraint class

  Constraint::Constraint(ConstraintFunction fct)
    : function(fct), data1(0), data2(0)
  {
  }



  // Constraint functions

  bool
  cstf_rigid(Constraint& c)
  {
    Particle* p1 = c.particles[0];
    Particle* p2 = c.particles[1];

    assert(p1 && p2);

    vector3f dv = p2->position() - p1->position();
    float n = norm(dv);
    float d = n - c.data1;
    float l = c.data2;

    assert(n);
    cstf_default(p1, p2, d, n, dv);

    return true;
  }

  bool
  cstf_supple(Constraint& c)
  {
    Particle* p1 = c.particles[0];
    Particle* p2 = c.particles[1];

    assert(p1 && p2);

    vector3f dv = p2->position() - p1->position();
    float n = norm(dv);
    float d = (n < c.data1 ? n - c.data1 : (n > c.data2 ? n - c.data2 : 0));

    assert(n);
    cstf_default(p1, p2, d, n, dv);

    return true;
  }

  bool
  cstf_identity(Constraint& c)
  {
    Particle* p1 = c.particles[0];
    Particle* p2 = c.particles[1];

    assert(p1 && p2);

    vector3f dv = p2->position() - p1->position();
    float d = 1.f;

    cstf_default(p1, p2, d, 1.f, dv);

    return true;
  }

  bool
  cstf_breakable(Constraint& c)
  {
    Particle* p1 = c.particles[0];
    Particle* p2 = c.particles[1];

    assert(p1 && p2);

    vector3f dv = p2->position() - p1->position();
    float n = norm(dv);
    float d = 1.f;
    float l = c.data1;

    if (n > l || l < 0.f) // BREAK!
    {
      l = -1.f; // flag
      return false;
    }

    cstf_default(p1, p2, d, 1.f, dv);

    return true;
  }



  // Constructors

  Constraint
  rigid_constraint(Particle* p1, Particle* p2, float d)
  {
    Constraint res(cstf_rigid);

    res.particles[0] = p1;
    res.particles[1] = p2;
    res.data1 = d;
    res.data2 = 0.f;

    return res;
  }

  Constraint
  breakable_constraint(Particle* p1, Particle* p2, float limit)
  {
    Constraint res(cstf_breakable);

    res.particles[0] = p1;
    res.particles[1] = p2;
    res.data1 = limit;

    return res;
  }

  Constraint
  supple_constraint(Particle* p1, Particle* p2, float dmin, float dmax)
  {
    Constraint res(cstf_supple);

    res.particles[0] = p1;
    res.particles[1] = p2;
    res.data1 = dmin;
    res.data2 = dmax;

    return res;
  }

  Constraint
  identity(Particle* p1, Particle* p2)
  {
    Constraint res(cstf_identity);

    res.particles[0] = p1;
    res.particles[1] = p2;

    return res;
  }


}
