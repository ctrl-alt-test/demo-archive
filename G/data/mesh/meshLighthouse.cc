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

static float coneRFunc(float t, float theta)
{
  return _TV(-1.f) * t + _TV(1.001f);
}

static float towerRFunc(float t, float theta)
{
  return _TV(-0.5f) * t + _TV(1.001f);
}

void buildLighthouse()
{
  Mesh::clearPool();

  const float towerHeight = _TV(40.f);
  const float towerDiameter = _TV(8.f);

  Mesh::MeshStruct & tower = Mesh::getTempMesh();
  Mesh::Revolution(Mesh::pipeHFunc, towerRFunc)
    .generateMesh(tower, towerHeight, towerDiameter, _TV(1), _TV(10));
  tower.setColor(Material::beautifulWhite);

  Mesh::MeshStruct & torus = Mesh::getTempMesh();
  Mesh::Revolution(Mesh::torusHFunc, Mesh::torusRFunc)
    .generateMesh(torus, _TV(15.f), towerDiameter - _TV(2.f), _TV(6), _TV(10));
  torus.setColor(Material::beautifulRed);

  Mesh::MeshStruct & top = Mesh::getTempMesh();
  Mesh::Revolution(Mesh::pipeHFunc, coneRFunc)
    .generateMesh(top, _TV(3.f), towerDiameter - _TV(2.f), _TV(2), _TV(10));
  top.setColor(Material::beautifulRed);

  Mesh::MeshStruct & pillars = Mesh::getTempMesh();
  Mesh::Pave(_TV(0.5f), _TV(10.f), _TV(0.5f)).generateMesh(pillars);

  float distPillars = towerDiameter * _TV(0.5f);

  pillars.add(pillars, matrix4::translation(vector3f(distPillars, 0, 0)));
  pillars.add(pillars, matrix4::translation(vector3f(0, 0, distPillars)));

  pillars.translate(-distPillars * 0.5f, 0, -distPillars * 0.5f);
  pillars.setColor(Material::beautifulRed);


  torus.translate(_TV(0.f), towerHeight, _TV(0.f));
  tower.add(torus);

  pillars.translate(_TV(0.f), towerHeight + _TV(0.f), _TV(0.f));
  tower.add(pillars);

  top.translate(_TV(0.f), towerHeight + _TV(5.f), _TV(0.f));
  tower.add(top);

  tower.computeBaryCoord();
  SETUP_VBO(lighthouse, tower);


  Mesh::MeshStruct & light = Mesh::getTempMesh();
  Mesh::Pave(_TV(4.f), _TV(4.f), _TV(4.f)).generateMesh(light);
  SETUP_VBO(lighthouseLight, light);
}

}

#elif MESH_EXPOSE == MESH_BUILDER_HEADER

void buildLighthouse();

#elif MESH_EXPOSE == MESH_BUILDER_INFO

{buildLighthouse DBGARG(__FILE__)},

#endif
