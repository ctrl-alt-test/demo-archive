#include "algebra/vector3.hxx"
#include "demo.hh"
#include "interpolation.hxx"
#include "intro.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "node.hxx"
#include "particles.hh"
#include "randomness.hh"
#include "renderable.hh"
#include "shaders.hh"
#include "snd/midisync.hh"
#include "snd/sound.hh"
#include "textures.hh"
#include "timing.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys.h"

namespace Particles
{
  // Attention : ces deux nombres apparaissent dans fbos.cc et particules.vert
#define NUM_PARTICLES 131072 //65536
#define NUM_COLS 512 //256

  void generateParticles(int count, VBO::id vboid)
  {
    const float minParticleSize = _TV(0.03f);
    const float maxParticleSize = _TV(0.1f);

    //
    //   0 4
    //  /| |\
    // 3 | | 5
    // 2 | | 6
    //  \| |/
    //   1 7
    //
    const float cos30 = 0.8660254f;
    const vector2f points[] = {
      vector2f(  0.0f,  1.0f),
      vector2f(  0.0f, -1.0f),
      vector2f(-cos30, -0.5f),
      vector2f(-cos30,  0.5f),

      vector2f(  0.0f,  1.0f),
      vector2f( cos30,  0.5f),
      vector2f( cos30, -0.5f),
      vector2f(  0.0f, -1.0f),
    };

    Mesh::MeshStruct & particle = Mesh::getTempMesh();
    particle.generate2DShape(points, 8, true, false);

    Rand rand;
    Mesh::MeshStruct res(particle.vertices.size * count);
    for (int i = 0; i < count; i++)
    {
      const float size = rand.fgen(minParticleSize, maxParticleSize);

      matrix4 transform = matrix4::rotation(DEG_TO_RAD * 90.f, vector3f::ux)
	.scale(vector3f(size, size, size));

      particle.setId((float) i);
      res.add(particle, transform);
    }
    VBO::setupData(vboid, res DBGARG("particles"));
  }

  void generateMeshes()
  {
    generateParticles(NUM_PARTICLES, NEW_VBO(particles));
  }

  float getFFT(int objid)
  {
    int t = intro.now.story + _TV(-2000);
    Array<float*> * fft = Sound::getFFT();
    int pos = Demo::iPosFromDate(t);
    float* fftLine = (*fft)[pos];

    int from = _TV(10);
    int to = _TV(30);
    float sum = 0.f;
    for (int i = from; i < to; i++)
      sum += fftLine[i];
    return sum;
  }

  Node * create(date startDate, date endDate)
  {
    Node * node = Node::New(startDate, endDate);
    Renderable particles(Material::particles, VBO_(particles));
    node->setAnimation(textureAnimation);
    particles.setTransitionFct(getFFT);
    node->attachRenderable(particles);
    return node;
  }

  //
  // Strange attractors
  // http://www.3d-meier.de/tut19/Seite0.html

  // http://www.3d-meier.de/tut19/Seite3.html
  static vector3f Aizawa(const vector3f & p)
  {
    const float a = 0.95f;
    const float b = 0.7f;
    const float c = 0.6f;
    const float d = 3.5f;
    const float e = 0.25f;
    const float f = 0.1f;

    const float dt = 0.01f;
    return p + dt * vector3f((p.z - b) * p.x - d * p.y,
			     d * p. x + (p.z - b) * p.y,
			     c + a * p.z - (p.z*p.z*p.z) / 3.f - (p.x*p.x + p.y*p.y) * (1.f + e * p.z) + f * p.z * p.x*p.x*p.x);
  }

  // http://www.3d-meier.de/tut19/Seite92.html
  static vector3f Anishchenko(const vector3f & p)
  {
    const float m = 1.2f;
    const float g = 0.5f;

    const float dt = 0.01f;
    return p + dt * vector3f(m * p.x + p.y - p.x * p.z,
			     -p.x,
			     -g * p.z + (p.x > 0 ? g * p.x * p.x : 0.f));
  }

  // http://www.3d-meier.de/tut19/Seite4.html
  static vector3f Arneodo(const vector3f & p)
  {
    // Si les valeurs initiales sont trop grandes, elles s'éloignent
    // jusqu'à diverger complètement..

    const float a = -5.5f;
    const float b = 3.5f;
    const float c = -1.f;

    const float dt = 0.02f;
    return p + dt * vector3f(p.y,
			     p.z,
			     -a * p.x - b * p.y - p.z + c * p.x * p.x * p.x);
  }

  // http://www.3d-meier.de/tut19/Seite6.html
  static vector3f Burke_Shaw(const vector3f & p)
  {
    // Si les valeurs initiales sont trop grandes, elle reviennent
    // tout doucement le long d'une ligne pas très jolie.

    const float s = 10.f;
    const float b = 4.272f;

    const float dt = 0.002f;
    return p + dt * vector3f(-s * (p.x + p.y),
			     -p.y - s * p.x * p.z,
			     s * p.x * p.y + b);
  }

  // http://www.3d-meier.de/tut19/Seite8.html
  static vector3f Chen_Lee(const vector3f & p)
  {
    // Fonctionnel et stable, mais le résultat n'est pas très
    // intéressant.

    const float a = 5.f;
    const float b = -10.f;
    const float c = -0.38f;

    const float dt = 0.01f;
    return p + dt * vector3f(a * p.x - p.y * p.z,
			     b * p.y + p.x * p.z,
			     c * p.z + p.x * p.y / 3.f);
  }

  // http://www.3d-meier.de/tut19/Seite9.html
  static vector3f Dequan_Li(const vector3f & p)
  {
    // Part trop loin

    const float a = 40.f;
    const float c = 1.833f;
    const float d = 0.16f;
    const float e = 0.65f;
    const float k = 55.f;
    const float f = 20.f;

    const float dt = 0.001f;
    return p + dt * vector3f(a * (p.y - p.x) + d * p.x * p.z,
			     k * p.x + f * p.y - p.x * p.z,
			     c * p.z + p.x * p.y - e * p.x * p.x);
  }

  // http://www.3d-meier.de/tut19/Seite13.html
  static vector3f Halvorsen(const vector3f & p)
  {
    // Forme en triskel, très chouette. Ne pas mettre de dt trop grand
    // sinon ça devient instable.

    const float a = 1.4f;

    const float dt = 0.002f;
    return p + dt * vector3f(-a * p.x - 4.f * p.y - 4.f * p.z - p.y * p.y,
			     -a * p.y - 4.f * p.z - 4.f * p.x - p.z * p.z,
			     -a * p.z - 4.f * p.x - 4.f * p.y - p.x * p.x);
  }

  static vector3f Lorenz(const vector3f & p)
  {
    const float a = 10.f;
    const float b = 28.f;
    const float c = 2.667f;

    const float dt = 0.002f;
    return p + dt * vector3f(a * (p.y - p.x),
			     p.x * (b - p.z) - p.y,
			     p.x * p.y - c * p.z);
  }

  static vector3f Rossler(const vector3f & p)
  {
    const float a = 0.2f;
    const float b = 0.2f;
    const float c = 5.7f;

    const float dt = 0.01f;
    return p + dt * vector3f(-(p.y + p.z),
			     p.x + a * p.y,
			     b + p.z * (p.x - c));
  }

  static float Chua_g(float x)
  {
    const float d = -1.f;
    const float e = 0.f;
    return e * x + (d + e) * (msys_fabsf(x + 1) - msys_fabsf(x - 1));
  }

  static vector3f Chua(const vector3f & p)
  {
    // Il semble y avoir un petit espace de valeurs initiales de v qui
    // fonctionnent. En dehors, ça diverge rapidement.

    const float a = 15.6f;
    const float b = 1.f;
    const float c = 25.58f;

    const float dt = 0.01f;
    return p + dt * vector3f(a * (p.y - p.x - Chua_g(p.x)),
			     b * (p.x - p.y + p.z),
			     -c * p.y);
  }

  // http://www.3d-meier.de/tut19/Seite15.html
  static vector3f Nose_Hoover(const vector3f & p)
  {
    const float a = 1.5f;

    const float dt = 0.02f;
    return p + dt * vector3f(p.y,
			     -p.x + p.y * p.z,
			     a - p.y * p.y);
  }

  // http://www.3d-meier.de/tut19/Seite41.html
  static vector3f Thomas(const vector3f & p)
  {
    const float b = 0.19f;

    const float dt = 0.06f;
    return p + dt * vector3f(-b * p.x + msys_sinf(p.y),
			     -b * p.y + msys_sinf(p.z),
			     -b * p.z + msys_sinf(p.x));
  }

  // On fixe la fréquence de rafraichissement à 60Hz
#define UPDATE_DT 16
#define FOOTPRINT 8

  float data[NUM_PARTICLES * FOOTPRINT];

  void textureAnimation(const Node & node, date d)
  {
    const int elevationStart = _TV(14000);
    const int elevationDuration = _TV(6000);

    const int convergeStart = _TV(75000);
    const int convergeEnd = _TV(88000);

    if ((int)d >= convergeStart)
    {
      glTranslatef(_TV(-45.f), _TV(-9.f), _TV(122.5f));
    }
    else if ((int)d >= elevationStart)
    {
      const float control = smoothStep(elevationStart, elevationStart + elevationDuration, (int)d);
      glTranslatef(0.f, _TV(74.f) * control * control, 0.f);
    }
  }
}
