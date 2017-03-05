#include "sys/msys.h"

#include "interpolation.hh"
#include "mesh.hh"
#include "node.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "../snd/sync.hh"
#include "textureid.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Armchair
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    Renderable armchair(Shader::parallax, VBO::armchair);
    armchair.setShininess(_TV(10));
    root->attachRenderable(armchair.setTextures(Texture::armchair, Texture::armchairBump));

    return root;
  }


  void generateMeshes()
  {
    Mesh::clearPool();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);

    Array<vertex> & armchair = Mesh::getTemp();
    Array<vertex> & arm = Mesh::getTemp();
    Array<vertex> & seat = Mesh::getTemp();
    Array<vertex> & coussin = Mesh::getTemp();

    // dossier
    wheel.generateMesh(armchair, _TV(15.f), _TV(28.f), _TV(16), _TV(30));
    Mesh::expandPave(armchair, _TV(30.f), _TV(0.f), _TV(6.f));
    glLoadIdentity();
    glRotatef(_TV(98.f), 0, 0, 1.f);
    Mesh::applyCurrentProjection(armchair);
    Mesh::scaleTexture(armchair, _TV(4.f), _TV(4.f));

    // accoudoire
    wheel.generateMesh(arm, _TV(24.f), _TV(7.f), _TV(18), _TV(14));
    glLoadIdentity();
    glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::applyCurrentProjection(arm);
    Mesh::expandPave(arm, _TV(18.f), _TV(28.f), _TV(0.5f));
    Mesh::scaleTexture(arm, _TV(2.f), _TV(2.f));

    // siège
    wheel.generateMesh(seat, _TV(30.f), _TV(12.f), _TV(12), _TV(16));
    Mesh::expandPave(seat, _TV(18.f), _TV(0.f), _TV(20.f));

    // coussin
    wheel.generateMesh(coussin, _TV(15.f), _TV(10.f), _TV(16), _TV(16));
    Mesh::expandPave(coussin, _TV(0.f), _TV(18.f), _TV(18.f));
    glLoadIdentity();
    glRotatef(_TV(94.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::applyCurrentProjection(coussin);

    // assemblage
    glLoadIdentity();
    glTranslatef(_TV(34.f), _TV(-20.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(armchair, seat);
    glTranslatef(_TV(0.f), _TV(15.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(armchair, coussin);
    glTranslatef(_TV(0.f), _TV(-5.f), _TV(-35.f));
    Mesh::addVerticesUsingCurrentProjection(armchair, arm);
    glTranslatef(_TV(0.f), _TV(0.f), _TV(70.f));
    Mesh::addVerticesUsingCurrentProjection(armchair, arm);

    VBO::setupData(VBO::armchair, armchair DBGARG("armchair"));
  }
}
