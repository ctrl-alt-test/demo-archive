#ifndef		SEAGUL_HH
# define	SEAGUL_HH

#include "node.hh"

namespace Seagul
{
  void generateMesh();
  Node * create(date startDate, date endDate, date wingAnimShift);

  void textureAnimation(date d);
}

#endif
