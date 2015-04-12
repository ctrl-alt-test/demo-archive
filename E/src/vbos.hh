//
// Tous les VBOs utilisés
//

#ifndef		VBOS_HH
# define	VBOS_HH

#include "loading.hh"
#include "vbodata.hh"

namespace VBO
{
  extern Element * list; // TODO: a virer ?
  void loadVBOs(Loading::ProgressDelegate * pd, int low, int high);
}

#endif		// VBOS_HH
