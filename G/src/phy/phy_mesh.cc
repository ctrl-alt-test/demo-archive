// Includes

#include "phy_mesh.hh"
#include "sys/msys_libc.h"

// Local functions

namespace
{
  inline vector3f
  make(float x, float y, float z)
  {
    return vector3f(x, y, z);
  }
}



namespace phy
{


  // Mesh class

  Mesh::Mesh(int p, int c, int f)
    : particles(p), constraints(c), faces(f)
  {
  }


  void
  Mesh::move_of(const vector3f& p)
  {
    for (int i = 0; i < particles.size; ++i)
      particles[i].move_of(p);
  }

  void
  Mesh::translate(const vector3f& p)
  {
    for (int i = 0; i < particles.size; ++i)
      particles[i].translate(p);
  }

  void
  Mesh::updateAABB()
  {
    min = particles[0].position();
    max = min;

    for (int i = 1; i < particles.size; ++i)
    {
      const point3f& p = particles[i].position();
      if (p.x < min.x) min.x = p.x;
      if (p.y < min.y) min.y = p.y;
      if (p.z < min.z) min.z = p.z;
      if (p.x >	max.x) max.x = p.x;
      if (p.y >	max.y) max.y = p.y;
      if (p.z >	max.z) max.z = p.z;
    }
  }



  // Constructors

  Mesh*
  chain(int size, float sep)
  {
    assert(size > 1);

    Mesh* res = new Mesh(size, size - 1, 4);
    float pos = 0.f;

    res->particles.add(Particle(make(0.f, 0.f, 0.f)));
    for (int i = 1; i < size; ++i)
    {
      pos -= sep;
      res->particles.add(Particle(make(0.f, 0.f, pos)));
      res->constraints.add(rigid_constraint(&res->particles[i - 1], &res->particles[i], sep));
    }

    return res;
  }

  Mesh*
  rope(int size, float sep_min, float sep_max)
  {
    assert(size > 1);

    Mesh* res = new Mesh(size, size - 1, 4);
    float pos = 0.f;

    res->particles.add(Particle(make(0.f, 0.f, 0.f)));
    for (int i = 1; i < size; ++i)
    {
      pos -= sep_max;
      res->particles.add(Particle(make(0.f, 0.f, pos)));
      res->constraints.add(supple_constraint(&res->particles[i - 1], &res->particles[i], sep_min, sep_max));
    }

    return res;
  }

  Mesh*
  cube(float size)
  {
    Mesh* res = new Mesh(8, 24, 24);
    float dsize = size * msys_sqrtf(2.f);

    res->particles.add(Particle(make(-size / 2.f, -size / 2.f, -size / 2.f)));
    res->particles.add(Particle(make(-size / 2.f, -size / 2.f,  size / 2.f)));
    res->particles.add(Particle(make(-size / 2.f,  size / 2.f, -size / 2.f)));
    res->particles.add(Particle(make(-size / 2.f,  size / 2.f,  size / 2.f)));
    res->particles.add(Particle(make( size / 2.f, -size / 2.f, -size / 2.f)));
    res->particles.add(Particle(make( size / 2.f, -size / 2.f,  size / 2.f)));
    res->particles.add(Particle(make( size / 2.f,  size / 2.f, -size / 2.f)));
    res->particles.add(Particle(make( size / 2.f,  size / 2.f,  size / 2.f)));

    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[1],  size));
    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[2],  size));
    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[4],  size));
    res->constraints.add(rigid_constraint(&res->particles[1], &res->particles[3],  size));
    res->constraints.add(rigid_constraint(&res->particles[1], &res->particles[5],  size));
    res->constraints.add(rigid_constraint(&res->particles[2], &res->particles[3],  size));
    res->constraints.add(rigid_constraint(&res->particles[2], &res->particles[6],  size));
    res->constraints.add(rigid_constraint(&res->particles[4], &res->particles[5],  size));
    res->constraints.add(rigid_constraint(&res->particles[4], &res->particles[6],  size));
    res->constraints.add(rigid_constraint(&res->particles[3], &res->particles[7],  size));
    res->constraints.add(rigid_constraint(&res->particles[5], &res->particles[7],  size));
    res->constraints.add(rigid_constraint(&res->particles[6], &res->particles[7],  size));
    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[3], dsize));
    res->constraints.add(rigid_constraint(&res->particles[1], &res->particles[2], dsize));
    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[5], dsize));
    res->constraints.add(rigid_constraint(&res->particles[1], &res->particles[4], dsize));
    res->constraints.add(rigid_constraint(&res->particles[0], &res->particles[6], dsize));
    res->constraints.add(rigid_constraint(&res->particles[2], &res->particles[4], dsize));
    res->constraints.add(rigid_constraint(&res->particles[1], &res->particles[7], dsize));
    res->constraints.add(rigid_constraint(&res->particles[3], &res->particles[5], dsize));
    res->constraints.add(rigid_constraint(&res->particles[2], &res->particles[7], dsize));
    res->constraints.add(rigid_constraint(&res->particles[3], &res->particles[6], dsize));
    res->constraints.add(rigid_constraint(&res->particles[4], &res->particles[7], dsize));
    res->constraints.add(rigid_constraint(&res->particles[5], &res->particles[6], dsize));

    res->faces.add(0); res->faces.add(1); res->faces.add(3); res->faces.add(2);
    res->faces.add(0); res->faces.add(4); res->faces.add(5); res->faces.add(1);
    res->faces.add(0); res->faces.add(2); res->faces.add(6); res->faces.add(4);
    res->faces.add(1); res->faces.add(5); res->faces.add(7); res->faces.add(3);
    res->faces.add(2); res->faces.add(3); res->faces.add(7); res->faces.add(6);
    res->faces.add(4); res->faces.add(6); res->faces.add(7); res->faces.add(5);

    return res;
  }



  // Functions

  float
  mobility(const Mesh* m)
  {
    float res = 0.0f;

    for (int i = 0; i < m->particles.size; ++i)
      res += m->particles[i].mobility();
    res /= m->particles.size;

    return res;
  }

  bool
  intersection(const Mesh* m1, const Mesh* m2)
  {
    assert(m1 && m2);

    const point3f& min1 = m1->min;
    const point3f& max1 = m1->max;
    const point3f& min2 = m2->min;
    const point3f& max2 = m2->max;

    return !(
      min1.x > max2.x ||
      min2.x > max1.x ||
      min1.y > max2.y ||
      min2.y > max1.y ||
      min1.z > max2.z ||
      min2.z > max1.z );
  }


}
