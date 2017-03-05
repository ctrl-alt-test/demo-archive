#ifndef		XYLOTRAIN_HH
# define	XYLOTRAIN_HH

#include "node.hh"
#include "vbos.hh"

namespace Xylotrain
{
  Node * create();
  void generateMeshes();
  void wheelsAnimation(const Node & node, date d);
  void trainAnimation(const Node &node, date d);
}

#endif XYLOTRAIN_HH
