#ifndef		SEA_HH
# define	SEA_HH

#include "node.hh"

namespace Sea
{
  void generateMeshes();
  Node * create(date startDate, date endDate);
  void firstSeagulAnimation(const Node & node, date d);
}

#endif
