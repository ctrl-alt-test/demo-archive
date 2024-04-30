#ifndef		WIND_TURBINE_HH
# define	WIND_TURBINE_HH

#include "node.hh"

namespace WindTurbine
{
  void generateMesh();
  Node * create(date startDate, date endDate, date animShift);
  void rotorAnimation(const Node & node, date d);
}

#endif
