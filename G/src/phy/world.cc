// Includes

#define USE_COLLISIONS 0

#include "world.hh"
#if USE_COLLISIONS
# include "gjk.hh"
#endif

namespace phy
{


  // Local functions

  namespace
  {


    void
    make_update(Array<Particle>& ps, const vector3f& grav)
    {
      for (int i = 0; i < ps.size; ++i)
      {
        Particle& prt = ps[i];
        point3f p = prt.position() + (prt.speed() + grav * PHY_SQDELTA) * prt.friction();
        prt.step_to(p);
      }
    }

    void
    make_satisfy(Array<Constraint>& cs)
    {
      for (int i = 0; i < cs.size; ++i)
        cs[i].satisfy();
    }

#if USE_COLLISIONS
    void
    handle_collision(Mesh* m1, Mesh* m2)
    {
      point3f p1;
      point3f p2;

      if (GJK(m1, m2, p1, p2))
      {
	vector3f d = (p2 - p1);
	float im1 = mobility(m1);
	float im2 = mobility(m2);
	vector3f d1 = d * ( im1 / (im1 + im2));
	vector3f d2 = d * (-im2 / (im1 + im2));

	DBG("[col] d : (%f, %f, %f) len: %f", d .x, d .y, d .z, norm(d ));
	DBG("[col] d1: (%f, %f, %f) len: %f", d1.x, d1.y, d1.z, norm(d1));
	DBG("[col] d2: (%f, %f, %f) len: %f", d2.x, d2.y, d2.z, norm(d2));

	assert(im1 || im2);
	m1->move_of(d * ( im1 / (im1 + im2)));
	m2->move_of(d * (-im2 / (im1 + im2)));
      }
    }
#endif

  }



   // World class

  void
  World::update(unsigned ms)
  {
    time_ += ms;

    //while (time_ >= PHY_STEP)
    //{
      update();
    //  time_ -= PHY_STEP;
    //}
  }


  void
  World::update()
  {
    vector3f gravity(0.f, -9.81f * 10.f, 0.f);

    for (int i = 0; i < meshes.size; ++i)
      make_update(meshes[i]->particles, gravity);
    make_update(particles, gravity);

    for (int it = 0; it < 5; ++it) // FIXME
    {
      for (int i = 0; i < meshes.size; ++i)
        make_satisfy(meshes[i]->constraints);
      make_satisfy(constraints);

#if USE_COLLISIONS
      for (int i = 0; i < meshes.size; ++i)
        meshes[i]->updateAABB();

      for (int i = 0; i < meshes.size - 1; ++i)
        for (int j = i + 1; j < meshes.size; ++j)
          if (meshes[i]->faces.size > 0 &&
              meshes[j]->faces.size > 0 &&
              intersection(meshes[i], meshes[j]))
            handle_collision(meshes[i], meshes[j]);
#endif
    }

#if USE_COLLISIONS
    for (int i = 0; i < meshes.size; ++i)
      meshes[i]->updateAABB();
#endif
  }


}
