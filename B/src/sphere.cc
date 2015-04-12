//
// Sphère - Cube
//

#include "sphere.hh"

#include "anim.hh"
#include "interpolation.hh"
#include "shaderid.hh"
#include "vboid.hh"

#include "tweakval.h"

#define RESOLUTION_LAT	24
#define RESOLUTION_LON	36

namespace Sphere
{
  unsigned int numberOfVertices = 0;
  VBO::vertex * vertices = NULL;

  static VBO::vertex sphereVertex(float x, float y, float z)
  {
    VBO::vertex p = { x, y, z,		// Sommet
		      x, y, z,		// Normale
		      0, 0, 0,		// Tangente
		      1.f, 1.f, 1.f,	// Couleur
		      0, 0 };		// U V
    return p;
  }

  void generateMesh()
  {
    assert(NULL == vertices);
    numberOfVertices = 4 * RESOLUTION_LON * RESOLUTION_LAT;
    vertices = (VBO::vertex *)msys_mallocAlloc(numberOfVertices * sizeof(VBO::vertex));

    float y1 = -0.5f;
    float l1 = 0;
    unsigned int index = 0;
    for (unsigned int j = 1; j <= RESOLUTION_LAT; ++j)
    {
      const float lat = PI * (float(j) / RESOLUTION_LAT - 0.5f);
      const float y2 = 0.5f * msys_sinf(lat);
      const float l2 = 0.5f * msys_cosf(lat);
      float lon1 = 0;
      for (unsigned int i = 1; i <= RESOLUTION_LON; ++i)
      {
	const float lon2 = (2.f * PI * i) / RESOLUTION_LON;
	const float x11 = l1 * msys_cosf(lon1);
	const float x12 = l1 * msys_cosf(lon2);
	const float x21 = l2 * msys_cosf(lon1);
	const float x22 = l2 * msys_cosf(lon2);

	const float z11 = l1 * msys_sinf(lon1);
	const float z12 = l1 * msys_sinf(lon2);
	const float z21 = l2 * msys_sinf(lon1);
	const float z22 = l2 * msys_sinf(lon2);

	vertices[index++] = sphereVertex(x21, y2, z21);
	vertices[index++] = sphereVertex(x22, y2, z22);
	vertices[index++] = sphereVertex(x12, y1, z12);
	vertices[index++] = sphereVertex(x11, y1, z11);
	lon1 = lon2;
      }
      y1 = y2;
      l1 = l2;
    }
  }

  void addToList(RenderList & renderList,
		 date birthDate, date startDieDate, date deathDate)
  {
    Renderable sphere(deathDate, Shader::sphere, birthDate,
		      1.f, numberOfVertices, VBO::sphere);
    sphere.setAnimation(Anim::sphereDie, startDieDate);
    renderList.add(sphere);
  }

  // --------------------------------------------------------------------------

  Cube::Cube(date start, date morphEnd, date end, RenderList & renderList):
    start(start), morphEnd(morphEnd), end(end), _renderList(renderList)
  {
    _system = new LSystem::System(renderList, end, start, Shader::sphere,
				  Anim::none, Texture::none, 1.f, 1.f, 2);
    _state = _system->iterate("F^X+X", "F", "G", "H", 7);
  }

  Cube::~Cube()
  {
    delete _system;
    msys_mallocFree(_state);
  }

  void Cube::queueRendering(date renderDate)
  {
    const float progression = clamp(interpolate((float)renderDate, (float)start, (float)morphEnd));
    const float x = progression - 1.f;
    const float angle = _TV(50.f) + _TV(40.f) * (x < 0 ? 1.f + x * x * x : 1.f);
    const float size = mix(0.048f, 0.14f, progression);

    glPushMatrix();
    glScalef(size, size, size);
    _system->setDates(renderDate, renderDate + 1);
    _system->setAngle(angle);
    _system->addStateToList(_state);
    glPopMatrix();
  }
}
