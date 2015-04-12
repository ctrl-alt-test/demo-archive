//
// Caisses en bois
//

#ifndef		CRATE_STACK_HH
# define	CRATE_STACK_HH

#include "basicTypes.hh"
#include "renderlist.hh"

namespace CrateStack
{
//   extern unsigned int numberOfVertices;
//   extern VBO::vertex * chunk;

//   void generateMeshes();

  void addToList(RenderList & renderList,
		 date birthDate, date deathDate,
		 unsigned int length, unsigned int width);
}

#endif		// CRATE_STACK_HH
