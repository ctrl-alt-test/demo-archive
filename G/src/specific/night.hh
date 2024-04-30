#ifndef		NIGHT_HH
# define	NIGHT_HH

#include "node.hh"

namespace Night
{
  void generateMesh();
  Node * create(date startDate, date endDate);
  void celestialSphereAnimation(const Node & node, date d);
}

#endif
