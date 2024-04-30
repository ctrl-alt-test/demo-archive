#include "balloon.hh"
#include "interpolation.hxx"
#include "materials.hh"
#include "node.hxx"
#include "renderable.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "sys/msys_libc.h"

namespace Balloon
{
  static float coneRFunc(float t, float theta)
  {
    return _TV(-1.f) * t + _TV(1.001f);
  }

  static float towerRFunc(float t, float theta)
  {
    return _TV(-0.5f) * t + _TV(1.001f);
  }

  Node * create(date startDate, date endDate, Material::id material)
  {
    Node * root = Node::New(startDate, endDate);
    Renderable balloonMesh(material, VBO_(balloon));
    root->attachRenderable(balloonMesh);
    root->setAnimation(balloonAnimation);
    return root;
  }

  void balloonAnimation(const Node & node, date d)
  {
    float angle = d * _TV(0.02f);
    float height = interpolate(d, _TV(15100), _TV(18000)) * _TV(50.f);
    glTranslatef(0.f, height, 0.f);
    glRotatef(angle, _TV(0.f), _TV(-1.f), _TV(0.f));
  }
}
