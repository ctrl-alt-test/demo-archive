#ifndef		PASCONTENT_HH
# define	PASCONTENT_HH

#include "node.hh"

namespace Pascontent
{
  Node * create();
  void generateMeshes();
  void bodyAnimation(const Node &node, date d);
  void sliderAnimation(const Node &node, date d);
  void pipeAnimation(const Node &node, date d);
}

#endif		// PASCONTENT_HH
