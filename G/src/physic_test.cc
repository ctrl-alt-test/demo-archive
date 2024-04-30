#include "physic_test.hh"

#include "array.hxx"
#include "phy/gjk.hh"
#include "phy/phy_mesh.hh"
#include "phy/world.hh"
#include "randomness.hh"

#include "tweakval.hh"
#include "sys/msys_glext.h"

namespace
{
  void drawCube(phy::Mesh* cube)
  {
#if 0
    const Array<phy::Particle>& parts = cube->particles;

    // FIXME : il ne faudrait pas reconstruire le mesh à chaque frame
    // Il faut le stocker dans phy::Mesh
    const int size = cube->particles.size;
    mesh m(size, 4 * size);
    for (int i = 0; i < size; i++)
    {
      vertex v;
      v.p = cube->particles[i].position();
      m.vertices.add(v);
    }
    static const int indices[24] =
    { 0, 1, 3, 2,
      0, 4, 5, 1,
      0, 2, 6, 4,
      1, 5, 7, 3,
      2, 3, 7, 6,
      4, 6, 7, 5
    };
    for (int i = 0; i < 24; i++)
      m.indices.add(indices[i]);
    immediateMeshDraw(m, 0);
#endif
  }

  void drawChain(phy::Mesh* chain)
  {
    const Array<phy::Particle>& parts = chain->particles;

    glLineWidth(5);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < parts.size; ++i)
      glVertex3fv((float*) &parts[i].position());
    glEnd();
  }

  phy::Mesh* cube;
  phy::Mesh* chain;

  phy::Mesh* cube2;
  phy::Mesh* chain2;

  phy::World world;
}


void phy_draw()
{
  drawCube(cube);
  drawCube(cube2);
  drawChain(chain);
  drawChain(chain2);
}

void phy_init()
{
  vector3f v1(0.5f, 0.5f, 0.5f);
  vector3f v2(0.0f, 1.1f, 0.0f);
  cube = phy::cube(1.f);
  cube2 = phy::cube(1.f);
  chain = phy::chain(24, 0.2f);
  chain2 = phy::chain(24, 0.2f);
  world = *new phy::World;

  chain2->translate(v2);
  cube->translate(chain->particles.last().position() + v1);
  cube2->translate(chain2->particles.last().position() + v1);

  world.meshes.add(cube);
  world.meshes.add(cube2);
  world.meshes.add(chain);
  world.meshes.add(chain2);
  chain->particles[0].friction(0.f);
  chain->particles[0].mobility(0.f);
  chain2->particles[0].friction(0.f);
  chain2->particles[0].mobility(0.f);

  world.constraints.add(phy::breakable_constraint(&chain->particles.last(), &cube->particles[0], 0.826f));
  world.constraints.add(phy::breakable_constraint(&chain2->particles.last(), &cube2->particles[0], 0.826f));

  // phy::Mesh* m1 = phy::cube(4.f);
  // phy::Mesh* m2 = phy::cube(4.f);
  // vector3f d = {2.1f, 1.5f, 1.5f};
  // point3f p1;
  // point3f p2;
  // bool c;

  // m2->move_of(d);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

  // m2->move_of(d);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

  // m2->move_of(d);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

  // m2->move_of(d * -4);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

  // m2->move_of(d * -1);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);

  // m2->particles[7].move_of(d);
  // c = GJK(m1, m2, p1, p2);
  // DBG("[gjk] collides? %s => (%f, %f, %f) (%f, %f, %f)", (c ? "true" : "false"), p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

// donne un coup de pied
void phy_shoot(Rand & rand)
{
  float strength = _TV(0.1f);
  vector3f v(rand.fgen(), rand.fgen(), rand.fgen());
  v *= strength;
  chain->particles.last().move_of(v);
}

void phy_update(date t)
{
  static date lastUpdate = 0;

  int delta = t - lastUpdate;
  // le moteur physique ne supporte pas les retours dans le passé !
  if (delta > 0)
  {
    if (lastUpdate != 0)
      world.update(delta);
    lastUpdate = t;
  }
}
