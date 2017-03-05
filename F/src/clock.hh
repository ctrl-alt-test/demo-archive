#ifndef		CLOCK_HH
# define	CLOCK_HH

#include "node.hh"

namespace CatClock
{
  Node * create();
  void generateMeshes();
  void pendulumAnimation(const Node & node, date d);
  void clockHandAnimation(const Node & node, date d);
}

#endif CHIMNEY_HH
