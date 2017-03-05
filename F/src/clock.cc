#include "sys/msys.h"

#include "interpolation.hh"
#include "mesh.hh"
#include "node.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "textureid.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace CatClock
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    Renderable clock(Shader::parallax, VBO::clockBody);
    root->attachRenderable(clock.setTextures(Texture::wood_Noyer));

    Renderable clockFace(Shader::parallax, VBO::clockFace);
    root->attachRenderable(clockFace.setTextures(Texture::clock));

    // pendule
    {
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(10.f), _TV(0.f));
      Node * node = Node::New(worldStartDate, worldEndDate);
      Renderable clockPendulum(Shader::anisotropic, VBO::clockPendulum);
      node->setAnimation(Anim::pendulum);
      node->attachRenderable(clockPendulum.setTextures(Texture::bronze));
      node->attachToNode(root);
    }

    // les aiguilles
    for (int i = 0; i < 3; i++)
    {
      const VBO::id ids[3] = { VBO::clockHandH, VBO::clockHandM, VBO::clockHandS };
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(17.f), _TV(-7.f));
      Node * node = Node::New(worldStartDate, worldEndDate);
      Renderable clockHand(Shader::parallax, ids[i]);
      clockHand.setId(i);
      node->setAnimation(Anim::clockHand);
      node->attachRenderable(clockHand.setTextures(Texture::black));
      node->attachToNode(root);
    }
    return root;
  }

  // Pour une raison que je ne comprends pas,
  // les minutes et les secondes avancent un poil trop vite
  // (avancer d'une heure avance un peu l'aiguille des minutes)
  void clockHandAnimation(const Node & node, date d)
  {
    int startTime = _TV(7) * 3600 + _TV(55) * 60; // heure au début de la démo
    d += startTime * 1000;

    int id = node.visiblePart()[0].id();
    float invSpeed = 1.f;
    if (id == 0) invSpeed = 720.f; // vitesse des heures (12 fois plus lent que les minutes)
    if (id == 1) invSpeed = 60.f; // vitesse des minutes
    invSpeed *= 1000.f;

    glRotatef(d * (PI * 2.f) / invSpeed, _TV(0.f), _TV(0.f), _TV(1.f));
    glTranslatef(_TV(0.f), _TV(-1.f), _TV(0.f));
  }

  void pendulumAnimation(const Node & node, date d)
  {
    glRotatef(_TV(10.f) * sin(_TV(0.00314159f) * d), _TV(0.f), _TV(0.f), _TV(1.f));
  }

  float earRFunc(float t, float theta) { return _TV(-1.f) * t + _TV(1.00001f); }

  void generateMeshes()
  {
    Mesh::clearPool();
    Array<vertex> & clock = Mesh::getTemp();

    // boite
    Mesh::generateCustomCubicTorus(clock, _TV(0.1f), _TV(0.1f), _TV(0.9f), _TV(0.9f));
    Mesh::translate(clock, _TV(-0.5f), _TV(-0.5f), _TV(-0.5f));
    Mesh::generatePave(clock, _TV(1.f), _TV(1.f), _TV(0.25f));
    glLoadIdentity();
    glScalef(_TV(20.f), _TV(34.f), _TV(10.f));
    Mesh::applyCurrentProjection(clock);

    // le tour du cadran
    Array<vertex> & torus = Mesh::getTemp();
    glLoadIdentity();
    Mesh::Revolution torusR(Mesh::torusHFunc, Mesh::torusRFunc);
    torusR.generateMesh(torus, _TV(5.f), _TV(14.f), _TV(6), _TV(18));
    glTranslatef(_TV(0.f), _TV(16.5f), _TV(-5.f));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(clock, torus);

    // les oreilles
    Array<vertex> & earMesh = Mesh::getTemp();
    Mesh::Revolution ear(Mesh::pipeHFunc, earRFunc);
    ear.generateMesh(earMesh, _TV(14.f), _TV(5.f), _TV(2), _TV(8));
    glLoadIdentity();
    glTranslatef(_TV(5.f), _TV(20.f), _TV(-1.5f));
    glRotatef(_TV(-36.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(clock, earMesh);
    glLoadIdentity();
    glTranslatef(_TV(-5.f), _TV(20.f), _TV(-1.5f));
    glRotatef(_TV(36.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(clock, earMesh);

    VBO::setupData(VBO::clockBody, clock DBGARG("clockBody"));

    // le cadran
    float faceShift = _TV(17.f);
    Array<vertex> & clockFace = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(clockFace, _TV(3.f), _TV(11.f), _TV(4), _TV(30));
    for (int i = 0; i < clockFace.size; i++) // calcul des coordonnées de texture
    {
      clockFace[i].u = clockFace[i].p.x * _TV(0.045f);
      clockFace[i].v = clockFace[i].p.z * _TV(0.045f);
    }
    glLoadIdentity();
    glTranslatef(_TV(0.f), faceShift, _TV(-5.f));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::applyCurrentProjection(clockFace);
    VBO::setupData(VBO::clockFace, clockFace DBGARG("clockBody"));

    // aiguilles
    Array<vertex> & clockHand = Mesh::getTemp();
    ear.generateMesh(clockHand, _TV(7.f), _TV(0.75f), _TV(2), _TV(3));
    VBO::setupData(VBO::clockHandH, clockHand DBGARG("clockHandH"));
    clockHand.empty();
    ear.generateMesh(clockHand, _TV(8.f), _TV(0.5f), _TV(2), _TV(3));
    VBO::setupData(VBO::clockHandM, clockHand DBGARG("clockHandM"));
    clockHand.empty();
    ear.generateMesh(clockHand, _TV(10.f), _TV(0.25f), _TV(2), _TV(3));
    VBO::setupData(VBO::clockHandS, clockHand DBGARG("clockHandS"));


    // le pendule
    Array<vertex> & clockPendulum = Mesh::getTemp();
    Array<vertex> & sphereTmp = Mesh::getTemp();
    Mesh::Revolution pipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution sphere(Mesh::sphereHFunc, Mesh::sphereRFunc);

    pipe.generateMesh(clockPendulum, _TV(18.f), _TV(0.75f), _TV(2), _TV(6));
    Mesh::translate(clockPendulum, _TV(0.f), _TV(-18.f), _TV(-1.f));

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(-18.f), _TV(-1.f));
    sphere.generateMesh(sphereTmp, _TV(2.5f), _TV(2.5f), _TV(12), _TV(12));
    Mesh::addVerticesUsingCurrentProjection(clockPendulum, sphereTmp);

    VBO::setupData(VBO::clockPendulum, clockPendulum DBGARG("clockPendulum"));
  }
}
