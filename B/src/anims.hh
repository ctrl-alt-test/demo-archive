//
// Toutes les animations utilisées
//

#ifndef		ANIMS_HH
# define	ANIMS_HH

#include "loading.hh"
#include "anim.hh"

namespace Anim
{
  extern Anim * list;

  void loadAnims(Loading::ProgressDelegate * pd, int low, int high);
}

#endif		// ANIMS_HH
