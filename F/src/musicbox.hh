#ifndef		MUSICBOX_HH
# define	MUSICBOX_HH
#include "vbos.hh"

namespace MusicBox
{
  void vibrationAnimation(const Node &node, date d);
  void rotationAnimation(const Node & node, date d);
  void cylinderAnimation(const Node & node, date d);
  void openingAnimation(const Node & node, date d);

  Node * create();
  void generateMeshes();
}

#endif MUSICBOX_HH
