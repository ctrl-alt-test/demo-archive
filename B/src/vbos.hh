//
// Tous les VBOs utilisés
//

#ifndef		VBOS_HH
# define	VBOS_HH

#include "city.hh"
#include "loading.hh"
#include "vbodata.hh"

namespace VBO
{
  extern Element * list;

  // FIXME : virer la ville de là
  void loadVBOs(City* city, Loading::ProgressDelegate * pd, int low, int high);
}

#endif		// VBOS_HH
