
#include "materials.hh"
#include "node.hxx"
#include "renderable.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

namespace Lighthouse
{
  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);
    Renderable tower(Material::poppingBuilding, VBO_(lighthouse));
    root->attachRenderable(tower);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(44.f), _TV(0.f));
    Node * lightNode = Node::New(startDate + _TV(600), endDate);
    Renderable light(Material::light, VBO_(lighthouseLight));
    lightNode->attachToNode(root);
    lightNode->attachRenderable(light);
    glPopMatrix();
    return root;
  }
}
