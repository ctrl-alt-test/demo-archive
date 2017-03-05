#ifndef		PUPPET_HH
# define	PUPPET_HH

#include "node.hh"

namespace Puppet
{
  Node * create();
  void generateMeshes();
  void leftArmAnimation(const Node & node, date d);
  void leftForearmAnimation(const Node & node, date d);
  void stickAnimation(const Node & node, date d);
  void leftArmClapAnimation(const Node & node, date d);
  void leftForearmClapAnimation(const Node & node, date d);
  void xyloArmAnimation(const Node & node, date d);
  void xyloForearmAnimation(const Node & node, date d);
  void headAnimation(const Node & node, date d);
  void hihatAnimation(const Node & node, date d);
}

#endif PUPPET_HH
