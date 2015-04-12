//
// Sphère - Cube
//

#ifndef		SPHERE_HH
# define	SPHERE_HH

#include "lsystem.hh"
#include "renderlist.hh"
#include "vboid.hh"

namespace Sphere
{
  extern unsigned int numberOfVertices;
  extern VBO::vertex * vertices;

  void generateMesh();
  void create(RenderList & renderList);
  void addToList(RenderList & renderList,
		 date birthDate, date startDieDate, date deathDate);

  class Cube
  {
  public:
    Cube(date start, date morphEnd, date end, RenderList & renderList);
    ~Cube();

    void queueRendering(date renderDate);

    date start;
    date morphEnd;
    date end;

  private:
    RenderList & _renderList;
    LSystem::System * _system;
    LSystem::state _state;
  };
}

#endif		// SPHERE_HH
