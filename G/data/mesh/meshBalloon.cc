#ifndef MESH_EXPOSE

#include "array.hxx"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "mesh/vertex.hh"
#include "spline.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "sys/msys_libc.h"

namespace Meshes
{

void buildBalloon()
{
  // http://www.air-montgolfiere.com/montgolfiere/montgolfiere-moderne.htm
  // Ballon de 20m de haut et 15m de diamètre
  float balloonData[] =
  {
    0, _TV( 0.0f), _TV(1.6f),
    0, _TV( 3.6f), _TV(4.4f),
    0, _TV( 7.4f), _TV(6.7f),
    0, _TV(11.3f), _TV(7.5f),
    0, _TV(15.2f), _TV(6.4f),
    0, _TV(17.8f), _TV(3.5f),
    0, _TV(18.5f), _TV(0.0f),
  };
  initSplineData(balloonData, ARRAY_LEN(balloonData));

  const int hFaces = _TV(15);
  const int rFaces = _TV(20);

  Mesh::MeshStruct & balloon = Mesh::getTempMesh();
  Variable v(balloonData, 3, ARRAY_LEN(balloonData) / 3, true);
  Mesh::Revolution balloonR(NULL, NULL, NULL, &v);
  balloonR.generateMesh(balloon, _TV(1.f), _TV(1.f), hFaces, rFaces);

  // Décallage vers le haut pour laisser la place à la nacelle
  balloon.translate(0., _TV(2.f), 0.);

  const vector3f colors[] = {
    Material::beautifulRed,
    Material::beautifulYellow,
  };
  for (int i = 0; i < balloon.vertices.size; ++i)
  {
    const int face = i / 4;
    const int row = face / rFaces;
    const vector3f & color = colors[(row != 0) * ((row + face) % 4)/2];
    balloon.vertices[i].r = color.x;
    balloon.vertices[i].g = color.y;
    balloon.vertices[i].b = color.z;
  }

  // Nacelle
  Mesh::MeshStruct & temp = Mesh::getTempMesh();
  Mesh::Pave(_TV(1.6f), _TV(1.2f), _TV(1.6f)).generateMesh(temp);
  temp.setColor(Material::trunkColor);

  balloon.add(temp);
  balloon.computeBaryCoord();
  SETUP_VBO(balloon, balloon);
}

}

#elif MESH_EXPOSE == MESH_BUILDER_HEADER

void buildBalloon();

#elif MESH_EXPOSE == MESH_BUILDER_INFO

{buildBalloon DBGARG(__FILE__)},

#endif
