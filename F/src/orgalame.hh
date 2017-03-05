#ifndef		ORGALAME_HH
# define	ORGALAME_HH

#include "node.hh"

namespace Orgalame
{
  Node * create();
  void generateMeshes();
  void keyAnimation(const Node & node, date d);
  void bladeAnimation(const Node & node, date d);
}

#endif		// ORGALAME_HH
