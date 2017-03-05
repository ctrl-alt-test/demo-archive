#ifndef		LOUSIANNE_HH
# define	LOUSIANNE_HH
#include "vbos.hh"

namespace Lousianne
{
  Node * create();
  void generateMeshes();
  void fluteAnimation(const Node & node, date d);
  void rotateAnimation(const Node & node, date d);
  void rotateWheelsAnimation(const Node & node, date d);
}

#endif LOUSIANNE_HH
