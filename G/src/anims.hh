//
// Toutes les animations utilisées
//

#ifndef		ANIMS_HH
# define	ANIMS_HH

#include "anim.hh"
#include "basicTypes.hh"

class Node;

namespace Anim
{
  typedef void (*animFunc)(const Node &, date);
  typedef char id;
  static const id none = -1;

  extern animFunc list[];
  id registerAnim(animFunc func);
}

#endif		// ANIMS_HH
