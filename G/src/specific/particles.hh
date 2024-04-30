#ifndef		PARTICLES_HH
# define	PARTICLES_HH

#include "node.hh"

namespace Particles
{
  void generateMeshes();
  Node * create(date startDate, date endDate);
  void textureAnimation(const Node & node, date d);
}

#endif
