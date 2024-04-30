#ifndef		CITY_HH
# define	CITY_HH

#include "node.hh"

namespace City
{
  void generateMeshes();
  Node * create(date startDate, date endDate);
  void lastSeagulAnimation(const Node & node, date d);
  void lastSeagulGroupAnimation(const Node & node, date d);
}

#endif
