//
// Tapis roulant
//

#ifndef		TREADMILL_HH
# define	TREADMILL_HH

#include "basicTypes.hh"
#include "renderlist.hh"
#include "vboid.hh"
#include "shaderid.hh"

namespace Treadmill
{
  extern unsigned int numberOfVerticesChunk0;
  extern unsigned int numberOfVerticesChunk1;
  extern unsigned int numberOfVerticesChunk2;
  extern unsigned int numberOfVerticesChunk3;

//   extern VBO::vertex * chunk;

  extern VBO::vertex * chunk0;
  extern VBO::vertex * chunk1;
  extern VBO::vertex * chunk2;
  extern VBO::vertex * chunk3;

  void generateMeshes();

  void addToList(RenderList & renderList,
		 date birthDate, date deathDate, date animStopDate,
		 unsigned int length, unsigned int width,
		 Shader::id shaderId = Shader::default);
}

#endif		// TREADMIL_HH
