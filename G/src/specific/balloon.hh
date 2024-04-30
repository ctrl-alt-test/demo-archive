#ifndef		BALLOON_HH
# define	BALLOON_HH

#include "node.hh"

namespace Balloon
{
  Node * create(date startDate, date endDate, Material::id material);
  void balloonAnimation(const Node & node, date d);
}

#endif
