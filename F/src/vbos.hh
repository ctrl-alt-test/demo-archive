//
// Tous les VBOs utilisés
//

#ifndef		VBOS_HH
# define	VBOS_HH

#include "loading.hh"
#include "vbodata.hh"
#include "vboid.hh"

namespace VBO
{
  extern Element * list; // TODO: a virer ?
  extern int elementSize[numberOfVBOs];
  void setupData(id id, const Array <vertex> & chunk
		 DBGARG(char * description));
  void generatePave(VBO::id id, float x, float y, float z);
  void loadVBOs();
}

#endif		// VBOS_HH
