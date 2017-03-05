#include "array.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "node.hh"
#include "revolution.hh"
#include "revolutionfunctions.hh"
#include "shaders.hh"
#include "../snd/sync.hh"
#include "textureid.hh"
#include "textures.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vbos.hh"

namespace Drum
{
  float borderData[] =
  {
    0, 0, 110,
    0, 29, 110,
    0, 30, 100,
    0, 59, 100,
    0, 60, 95,
    0, 0, 95,
    0, 0, 110,
  };

  void generateDrummer()
  {
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution sphere(Mesh::sphereHFunc, Mesh::sphereRFunc);

    // baguette
    Array<vertex> & stick = Mesh::getTemp();
    Array<vertex> & tmp = Mesh::getTemp();
    sphere.generateMesh(stick, _TV(1.5f), _TV(1.5f), _TV(12), _TV(12));
    Mesh::translate(stick, _TV(0.f), _TV(4.5f), _TV(0.f));
    wheel.generateMesh(stick, _TV(7.f), _TV(0.5f), _TV(5), _TV(6));
    Mesh::translate(stick, _TV(0.f), _TV(3.5f), _TV(0.f));

    // barre tournante
    wheel.generateMesh(tmp, _TV(14.f), _TV(0.5f), _TV(5), _TV(8));
    Mesh::rotate(tmp, _TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(stick, tmp);
    VBO::setupData(VBO::drumStick, stick DBGARG("drumStick"));

    Array<vertex> & support = Mesh::getTemp();
    Mesh::generatePave(support, _TV(2.f), _TV(1.f), _TV(9.f));
    Mesh::translate(support, _TV(0.f), _TV(-3.5f), _TV(-3.f));
    Mesh::generatePave(support, _TV(1.f), _TV(7.f), _TV(1.f));
    Mesh::translate(support, _TV(-6.f), _TV(-3.f), _TV(6.f));
    glLoadIdentity();
    glTranslatef(_TV(12.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(support, support);

    VBO::setupData(VBO::drumSupport, support DBGARG("drumSupport"));
  }

  void generateMeshes()
  {
    float radius = _TV(12.f);
    float height = _TV(10.f);

    Mesh::clearPool();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);

    Array<vertex> & drumMain = Mesh::getTemp();
    cyl.generateMesh(drumMain, height, radius + _TV(0.5f), _TV(2), _TV(30));
    Mesh::rotate(drumMain, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::translate(drumMain, _TV(0.f), radius * 0.5f, -height);
    VBO::setupData(VBO::drumMain, drumMain DBGARG("drumMain"));

    Array<vertex> & drumSkin = Mesh::getTemp();
    wheel.generateMesh(drumSkin, _TV(2.f), radius + _TV(0.25f), _TV(8), _TV(12));
    Mesh::rotate(drumSkin, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::translate(drumSkin, _TV(0.f), radius * 0.5f, _TV(0.f));
    Mesh::reprojectTextureXYPlane(drumSkin, _TV(0.08f));
    Mesh::translateTexture(drumSkin, _TV(0.f), _TV(-0.4f));
    VBO::setupData(VBO::drumSkin, drumSkin DBGARG("drumSkin"));

    // parties métalliques
    Array<vertex> & metal = Mesh::getTemp();
    //Mesh::Revolution borderRev = Mesh::loadSplineFromFile("data/revolution.txt");
    Mesh::Revolution borderRev(NULL, NULL, borderData, ARRAY_LEN(borderData));
    borderRev.generateMesh(metal, _TV(2.f), radius, _TV(12), _TV(-24));
    Mesh::rotate(metal, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    glLoadIdentity();
    glRotatef(_TV(180.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glTranslatef(_TV(0.f), _TV(0.f), height);
    Mesh::addVerticesUsingCurrentProjection(metal, metal);

    Array<vertex> & metalStick = Mesh::getTemp();
    // le bout
    wheel.generateMesh(metalStick, _TV(0.5f), _TV(0.3f), _TV(4), _TV(10));
    Mesh::rotate(metalStick, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::translate(metalStick, _TV(0.f), _TV(5.75f), _TV(0.f));
    // la barre
    wheel.generateMesh(metalStick, height, _TV(0.25f), _TV(5), _TV(6));
    Mesh::rotate(metalStick, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::translate(metalStick, _TV(0.f), radius + _TV(0.75f), _TV(-5.f));
    int number = _TV(12);
    for (int i = 0; i < number; i++) {
      glLoadIdentity();
      glRotatef(i * _TV(360.f) / number, _TV(0.f), _TV(0.f), _TV(1.f));
      glTranslatef(_TV(0.f), _TV(0.f), _TV(0.f));
      Mesh::addVerticesUsingCurrentProjection(metal, metalStick);
    }

    Mesh::translate(metal, _TV(0.f), radius * 0.5f, _TV(0.f));
    VBO::setupData(VBO::drumMetal, metal DBGARG("drumMetal"));

    generateDrummer();
  }

  void stickAnimation(const Node & node, date d)
  {
    Array<Sync::Note> & arr = Sync::sheet[Instrument::Boum];
    int idx = Sync::getLastNote(Instrument::Boum, d);
    float progress = 0.f; // 0 : repos ; 1 : impact

    if (idx + 1 < arr.size) // rotation vers le tambour
    {
      int time = arr[idx + 1].time;
      progress = smoothStepI(time - _TV(100), time, d);
    }

    if (idx >= 0) // rotation après impact
    {
      int time = arr[idx].time;
      progress = max(progress,
                     1.f - smoothStepI(time, time + _TV(300), d));
    }

    glTranslatef(_TV(0.f), _TV(0.f), _TV(0.f));
    glRotatef(_TV(-24.f) * progress, _TV(1.f), _TV(0.f), _TV(0.f));
  }

  static void drumUniform(Shader::id sid, int objid)
  {
    int d = (int) intro.now.youtube;
    Array<Sync::Note> & arr = Sync::sheet[Instrument::Boum];
    int idx = Sync::getLastNote(Instrument::Boum, d);
    if (idx < 0 || idx + 1 == arr.size) return;
    int end = min(arr[idx+1].time, arr[idx].time + _TV(500));
    d = iclamp(d, arr[idx].time, end);
    float progress = interpolate((float) d, (float) arr[idx].time, (float) end);
    Shader::setUniform1f(sid, Shader::trans, progress);
  }

  Node* create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    Renderable drumMain(Shader::parallax, VBO::drumMain);
    drumMain.setTextures((Texture::id)(_TV(46)));
    drumMain.setShininess(_TV(20));
    root->attachRenderable(drumMain);

    Renderable drumSkin(Shader::drum, VBO::drumSkin);
    drumSkin.setCustomSetUniform(drumUniform);
    drumSkin.setTextures(Texture::none);
    drumSkin.setShininess(_TV(20));
    root->attachRenderable(drumSkin);

    Renderable metal(Shader::anisotropic, VBO::drumMetal);
    metal.setTextures(Texture::silver);
    metal.setShininess(_TV(40));
    root->attachRenderable(metal);

    Renderable support(Shader::parallax, VBO::drumSupport);
    support.setTextures(Texture::wood_Chene, Texture::woodBump1);
    support.setShininess(_TV(20));
    root->attachRenderable(support);

    {
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(-2.f), _TV(6.f));
      Node * stick = Node::New(worldStartDate, worldEndDate);
      Renderable drumStick(Shader::parallax, VBO::drumStick);
      drumStick.setTextures(Texture::wood_MerisierClair, Texture::woodBump1);
      drumStick.setShininess(_TV(20));
      stick->attachRenderable(drumStick);
      stick->setAnimation(Anim::drumStick);
      stick->attachToNode(root);
    }
    return root;
  }
}
