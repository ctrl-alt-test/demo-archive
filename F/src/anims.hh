//
// Toutes les animations utilisées
//

#ifndef		ANIMS_HH
# define	ANIMS_HH

#include "anim.hh"
#include "basicTypes.hh"
#include "node.hh"

namespace Anim
{
  typedef void (*animFunc)(const Node &, date);

  extern animFunc * list;

// #if DEBUG
//   extern Anim<Transformation> * nodeIndependantAnims;
// #endif

  void loadAnims();
}

#endif		// ANIMS_HH
