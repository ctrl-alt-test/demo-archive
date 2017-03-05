#ifndef		DRUM_HH
# define	DRUM_HH
#include "vbos.hh"

namespace Drum
{
  Node * create();
  void generateMeshes();
  void stickAnimation(const Node & node, date d);
}

#endif // DRUM_HH
