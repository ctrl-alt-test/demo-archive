#ifndef		HILLS_HH
# define	HILLS_HH

#include "mesh/mesh.hh"
#include "node.hh"

namespace Hills
{
  void generateMeshes();
  Node * create(date startDate, date endDate);
  void houseAnimation(const Node & node, date d);
  void treeAnimation(const Node & node, date d);
  void midiAnimation(const Node & node, date d);
  void translationAnimation(const Node & node, date d);
  void roadAnimation(const Node & node, date d);
  void lastSeagulAnimation(const Node & node, date d);

  void generateTreeMesh(Mesh::MeshStruct & mesh,
			       Mesh::MeshStruct & temp,
			       int tFaces, int thetaFaces,
			       float height, float diameter,
			       float trunkHProportion, float trunkDProportion,
			       float topWeight, float bottomWeight,
			       float shapeNoise);
}

#endif
