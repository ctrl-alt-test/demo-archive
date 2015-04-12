//
// Écran fait de cubes
//

#ifndef		SCREEN_HH
# define	SCREEN_HH

#include "renderlist.hh"
#include "vbodata.hh"

namespace Screen
{
  extern unsigned int numberOfScreenVertices;
  extern VBO::vertex * screen;
  extern VBO::vertex * redCube;
  extern VBO::vertex * greenCube;
  extern VBO::vertex * blueCube;

  void generateMeshes();

  void createScreen(RenderList & renderList);
}

#endif		// SCREEN_HH
