//
// Définition du cube
//

#ifndef		CUBE_HH
# define	CUBE_HH

#include "vertex.hh"

namespace Cube
{
  extern const unsigned int numberOfVertices;
  extern const unsigned int numberOfVerticesPerFace;

  extern vertex vertices[];

#if DEBUG
  vertex myCube(unsigned int i);
  void debugCube();
#endif // !DEBUG

  /*
  extern float vertices[];
  extern float normals[];
  extern unsigned char colors[];
  extern float fcolors[];
  */
}

#endif		// CUBE_HH
