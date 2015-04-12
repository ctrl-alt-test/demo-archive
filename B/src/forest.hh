#ifndef FOREST_HH_
# define FOREST_HH_

#include "renderlist.hh"
#include "vbodata.hh"

namespace Forest
{
  extern Array<VBO::vertex> *ground;
  extern Array<VBO::vertex> *trees;
  extern const date cubeStopDuration;

  void generateMeshes();
  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position);
  void create(RenderList & renderList);
}

#endif
