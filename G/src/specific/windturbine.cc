
#include "algebra/vector3.hxx"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "node.hxx"
#include "renderable.hh"
#include "tweakval.hh"
#include "vbos.hh"
#include "windturbine.hh"

#include "sys/msys_glext.h"

namespace WindTurbine
{
//   static float towerRFunc(float t, float theta)
//   {
//     return _TV(-0.5f) * t + _TV(1.001f);
//   }

  void generateMesh()
  {
    Mesh::clearPool();

    const float towerHeight = _TV(80.f);
    const float towerDiameter = _TV(3.f);
    const float nacelleLength = _TV(9.f);
    const float nacelleDiameter = _TV(3.5f);
    const float bladeLength = _TV(35.f);

    //
    //   1__
    //  /   ---2
    // 0-------3
    //
    const vector2f points[] = {
      vector2f(_TV(0.00f), _TV(0.00f)) * bladeLength,
      vector2f(_TV(0.20f), _TV(0.12f)) * bladeLength,
      vector2f(_TV(1.00f), _TV(0.04f)) * bladeLength,
      vector2f(_TV(1.00f), _TV(0.00f)) * bladeLength,
    };

    Mesh::MeshStruct & blades = Mesh::getTempMesh();

    Mesh::MeshStruct & blade = Mesh::getTempMesh();
    blade.generate2DShape(points, 4, true, true);
    blade.rotate(DEG_TO_RAD * _TV(-120.f), 1.f, 0, 0);
    blade.computeBaryCoord();

    blades.add(blade);
    blade.rotate(DEG_TO_RAD * 120.f, 0, 0, 1.f);
    blades.add(blade);
    blade.rotate(DEG_TO_RAD * 120.f, 0, 0, 1.f);
    blades.add(blade);
    blades.setColor(Material::beautifulWhite);

    SETUP_VBO(windTurbineRotor, blades);

//     Mesh::MeshStruct & tower = Mesh::getTempMesh();
//     Mesh::Revolution(Mesh::pipeHFunc, towerRFunc)
//        .generateMesh(tower, towerHeight, towerDiameter, _TV(1), _TV(8));

    Mesh::MeshStruct & tower = Mesh::getTempMesh();
    Mesh::Pave(nacelleDiameter, nacelleDiameter, nacelleLength).generateMesh(tower);
    tower.translate(0, 0.5f * towerHeight, _TV(-0.3f) * towerDiameter);
    Mesh::Pave(towerDiameter, towerHeight, towerDiameter).generateMesh(tower);
    tower.translate(0, 0.5f * towerHeight, 0);
    tower.computeBaryCoord();
    tower.setColor(Material::beautifulWhite);

    SETUP_VBO(windTurbineTower, tower);
  }

  Node * create(date startDate, date endDate, date animShift)
  {
    Node * root = Node::New(startDate, endDate);

    Renderable tower(Material::retro, VBO_(windTurbineTower));
    root->attachRenderable(tower);

    const float towerHeight = _TV(80.f);
    const float towerDiameter = _TV(3.f);

    glLoadIdentity();
    glTranslatef(0, towerHeight, 2.f * towerDiameter);

    Node * rotor = Node::New(startDate, endDate);
    Renderable rotorMesh(Material::retro, VBO_(windTurbineRotor));
    rotor->attachRenderable(rotorMesh);
    rotor->setAnimationStepShift(animShift);
    rotor->setAnimation(rotorAnimation);
    rotor->attachToNode(root);

    return root;
  }

#undef _TV
#define _TV(x) x

  void rotorAnimation(const Node & node, date d)
  {
    const int circleDuration = _TV(6000);
    const float progression = ((int)d % circleDuration) / float(circleDuration);

    glRotatef(-360.f * progression, 0, 0, 1.f);
  }
}
