#ifndef		carousel_HH
# define	carousel_HH

namespace Carousel
{
  void physicsAnimation(const Node &node, date d);

  Node * create();
  void generateMeshes();
  void update(date t);
}

#endif XYLOTRAIN_HH
