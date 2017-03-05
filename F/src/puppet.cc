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

namespace Puppet
{
  enum kind { eClap, eHihat, eXylo }; // type de marionnette

  static float bodyData[] =
  {
    0, 0, 0,
    0, 5, 10,
    0, 10, 40,
    0, 80, 25,
    0, 100, 20,
    0, 105, 10,
    0, 115, 10,
    0, 120, 0,
  };

  static float headData[] =
  {
    0, 0, 0,
    0, 3, 10,
    0, 10, 20,
    0, 30, 26,
    0, 40, 25,
    0, 50, 24,
    0, 58, 18,
    0, 60, 0,
  };

  static float hatData[] =
  {
    0, 0, 0,
    0, 2, 35,
    0, 8, 35,
    0, 8, 15,
    0, 30, 15,
    0, 32, 5,
    0, 33, 0,
  };

  static float legData[] =
  {
    0, -61, 0,
    0, -60, 8,
    0, 0, 10,
    0, 10, 10,
    0, 58, 10,
    0, 48, 16,
    0, 110, 16,
    0, 120, 0,
  };

  static float forearmData[] =
  {
    0, -5, 10,
    0, 0, 30,
    0, 15, 70,
    0, 87, 45,
    0, 100, 80,
    0, 112, 85,
    0, 120, 0,
  };

  static bool isDead(date d)
  {
    return (int) d < _TV(90000) || (int) d > _TV(216000);
  }

  void hihatAnimation(const Node & node, date d)
  {
    Array<Sync::Note> & arr = Sync::sheet[Instrument::Hihat];
    int idx = Sync::getLastNote(Instrument::Hihat, d);

    if (idx < 0 || idx + 1 == arr.size) return;
    int end = min(arr[idx+1].time, arr[idx].time + _TV(700));
    float progress = smoothStepI(arr[idx].time, end, d);

    float angle = msys_sinf(d * _TV(0.04f)) * (1.f - progress);
    glRotatef(angle * _TV(7.f), _TV(0.f), _TV(0.f), _TV(1.f));
  }

  // animation baguette
  void leftArmAnimation(const Node & node, date d)
  {
    float c = (node.visiblePart()[0].id() == 1) ? -1.f : 1.f; // bras droit ou gauche
    if (isDead(d) || c > 0.f) {
      glRotatef(c * _TV(10.f), _TV(0.f), _TV(1.f), _TV(0.f));
      glRotatef(_TV(-60.f), _TV(0.f), _TV(0.f), _TV(1.f));
    }
    else {
      float p = Sync::getProgress(Instrument::Hihat, d, _TV(100), _TV(1500), _TV(1000));
      float angle = mix(_TV(-10.f), _TV(10.f), p);
      glRotatef(angle, _TV(0.f), _TV(0.f), _TV(1.f));
    }
  }

  void leftForearmAnimation(const Node & node, date d)
  {
    if (isDead(d) || node.visiblePart()[0].id() == 0) {
      glRotatef(_TV(40.f), _TV(0.f), _TV(0.f), _TV(1.f));
    }
    else  // bras gauche et en vie
    {
      float p = Sync::getProgress(Instrument::Hihat, d, _TV(100), _TV(1500), _TV(1000));
      float angle = mix(_TV(80.f), _TV(0.f), p);
      glRotatef(_TV(255) + angle, _TV(0.f), _TV(0.f), _TV(-1.f));
    }
  }

  // animation applaudissement
  void leftArmClapAnimation(const Node & node, date d)
  {
    float c = (node.visiblePart()[0].id() == 1) ? -1.f : 1.f; // bras gauche ou droit
    if (isDead(d)) {
      glRotatef(c * _TV(10.f), _TV(0.f), _TV(1.f), _TV(0.f));
      glRotatef(_TV(-60.f), _TV(0.f), _TV(0.f), _TV(1.f));
      return;
    }
    float p = Sync::getProgress(Instrument::Clap, d, _TV(200), _TV(500), _TV(1));
    float angle = c * mix(_TV(25.f), _TV(-15.f), p);
    glRotatef(angle, _TV(0.f), _TV(1.f), _TV(0.f));
  }

  void leftForearmClapAnimation(const Node & node, date d)
  {
    if (isDead(d)) {
      glRotatef(_TV(40.f), _TV(0.f), _TV(0.f), _TV(1.f));
      return;
    }
    float p = Sync::getProgress(Instrument::Clap, d, _TV(200), _TV(500), _TV(1));
    float angle = mix(_TV(0.f), _TV(20.f), p);
    if (node.visiblePart()[0].id() == 1) angle *= _TV(0);
    glRotatef(_TV(60.f), _TV(0.f), _TV(0.f), _TV(1.f));
    glRotatef(_TV(0.f) + angle, _TV(1.f), _TV(0.f), _TV(0.f));
  }

  // dates où le baton part et atterrit.
  static int jugglingStart() { return _TV(169500); }
  static int jugglingEnd() { return _TV(170300); }

  // Baton du xylophone
  void stickAnimation(const Node & node, date d)
  {
    int begin = jugglingStart();
    int end = jugglingEnd();
    if ((int) d < begin || (int) d > end)
    {
      // on suit le bras, pas d'animation en plus
      return;
    }

    // Pirouette en l'air !
    float progress = interpolate((float) d, (float) begin, (float) end);

    float y = 1.f - 4.f * (progress - 0.5f) * (progress - 0.5f); // parabole
    float angle = mix(_TV(0.f), _TV(720.f), progress);

    glTranslatef(_TV(0.f), y * _TV(12.f), _TV(0.f));
    glTranslatef(_TV(0.f), _TV(4.5f), _TV(0.f)); // décalage du centre de rotation
    glRotatef(_TV(0.f) + angle, _TV(0.f), _TV(0.f), _TV(1.f));
    glTranslatef(_TV(0.f), _TV(-4.5f), _TV(0.f));
  }

  void xyloArmAnimation(const Node & node, date d)
  {
    // jonglage
    int begin = jugglingStart() - _TV(600);
    int end = jugglingEnd() + _TV(300);
    if ((int) d > begin && (int) d < end)
    {
      float goingUp = smoothStepI(begin, jugglingStart(), d);
      float goingDown = 1.f - smoothStepI(jugglingEnd(), end, d);
      float angle = mix(_TV(-20.f), _TV(-70.f), min(goingUp, goingDown));
      glRotatef(angle * _TV(1.f) + _TV(0.f), _TV(0.f), _TV(0.f), _TV(1.f));
      glRotatef(angle * _TV(0.f) + _TV(0.f), _TV(0.f), _TV(0.f), _TV(1.f));
      return;
    }

    float p = Sync::getProgress(Instrument::Xylo, d, _TV(100), _TV(200), _TV(200));
    float angle = mix(_TV(0.f), _TV(10.f), p);
    glRotatef(_TV(-20.f), _TV(0.f), _TV(0.f), _TV(1.f));
    glRotatef(_TV(-5.f) + angle, _TV(1.f), _TV(0.f), _TV(0.f));
  }

  void xyloForearmAnimation(const Node & node, date d)
  {
    // jonglage
    int begin = jugglingStart() - _TV(100);
    int end = jugglingEnd() + _TV(500);
    if ((int) d > begin && (int) d < end)
    {
      float goingUp = smoothStepI(begin, begin + _TV(200), d);
      float goingDown = 1.f - smoothStepI(jugglingEnd() + _TV(0), end, d);
      float angle = mix(_TV(46.f), _TV(90.f), min(goingUp, goingDown));
      glRotatef(angle, _TV(0.f), _TV(0.f), _TV(1.f));
      return;
    }

    float p = Sync::getProgress(Instrument::Xylo, d, _TV(100), _TV(200), _TV(200));
    float angle = mix(_TV(30.f), _TV(60.f), p);
    glRotatef(angle, _TV(0.f), _TV(0.f), _TV(1.f));
  }

  void headAnimation(const Node & node, date d)
  {
    int doorTime = _TV(214800);// date à laquelle on commence à tourner la tête
    kind k = (kind) (node.birthDate()); // HACK
    glTranslatef(_TV(0.f), _TV(11.f), _TV(0.f));
    if (isDead(d))
      glRotatef(_TV(-45.f), _TV(0.f), _TV(-0.25f), _TV(1.f));
    else if (k == eHihat && (int) d > doorTime - _TV(100)) { // regarde la porte
      float angle = smoothStepI(doorTime, doorTime + _TV(300), d);
      glRotatef(mix(_TV(0.f), _TV(-30.f), angle), _TV(0.f), _TV(1.f), _TV(0.f));
    }
    else if ((int) d > _TV(108000)) { // balancement de la tête
      float angle = 0.5f + 0.5f * msys_sinf(d * _TV(0.006283f));
      glRotatef(angle * _TV(-10.f), _TV(0.f), _TV(0.f), _TV(1.f));
    }
    glTranslatef(_TV(0.f), _TV(-11.f), _TV(0.f));
  }

  // Création des noeuds

  static void createArm(Node * root, bool left, kind kind)
  {
    Node * leftArm = Node::New(worldStartDate, worldEndDate);
    Renderable arm(Shader::parallax, VBO::puppetArm);
    arm.setId(left);
    arm.setTextures(Texture::wood_MerisierClair);
    arm.setShininess(_TV(20));
    leftArm->attachRenderable(arm);

    // avant-bras
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(-5.5f), _TV(0.f));
    glRotatef(_TV(180.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Node * leftForearm = Node::New(worldStartDate, worldEndDate);
    Renderable forearm(Shader::parallax, VBO::puppetForearm);
    forearm.setId(left);
    forearm.setShininess(_TV(20));
    leftForearm->attachRenderable(forearm.setTextures(Texture::wood_MerisierClair));


    if (kind == eHihat)
    {
      if (left) {      // baguette
        Renderable stick(Shader::anisotropic, VBO::hihatstick);
        glTranslatef(_TV(4.f), _TV(-13.f), _TV(0.f));
        glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(-1.f));
        leftForearm->attachRenderableNode(stick.setTextures(Texture::silver));
      }

      // animation
      leftArm->setAnimation(Anim::leftArm);
      leftForearm->setAnimation(Anim::leftForearm);
    }
    else if (kind == eClap)
    {
      leftArm->setAnimation(Anim::leftArmClap);
      leftForearm->setAnimation(Anim::leftForearmClap);
    }
    else // xylo
    {
      if (left) {
        Renderable stick(Shader::parallax, VBO::xylostick);
        glTranslatef(_TV(0.f), _TV(-13.f), _TV(0.f));
        glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(-1.f));
        stick.setTextures(Texture::wood_MerisierFonce);
        stick.setShininess(_TV(60));

        Node * stickNode = Node::New(worldStartDate, worldEndDate);
        stickNode->setAnimation(Anim::xyloStick);
        stickNode->attachRenderable(stick);
        stickNode->attachToNode(leftForearm);

        //leftForearm->attachRenderableNode(stick);
        leftArm->setAnimation(Anim::xyloArm);
        leftForearm->setAnimation(Anim::xyloForearm);
      }
      else { // bras droit inanimé
        leftArm->setAnimation(Anim::leftArm);
        leftForearm->setAnimation(Anim::leftForearm);
      }
    }

    leftForearm->attachToNode(leftArm);
    leftArm->attachToNode(root);
  }

  static void createHihat(Node * root)
  {
    int metalShininess = _TV(80);

    glLoadIdentity();
    glTranslatef(_TV(16.5f), _TV(5.f), _TV(-5.f));
    Renderable body(Shader::anisotropic, VBO::hihatBody);
    body.setShininess(metalShininess);
    root->attachRenderableNode(body.setTextures(Texture::silver));

    glTranslatef(_TV(0.f), _TV(7.f), _TV(0.f));
    Node * headNode = Node::New(worldStartDate, worldEndDate);
    Renderable head(Shader::anisotropic, VBO::hihatHead);
    head.setShininess(metalShininess);
    headNode->setAnimation(Anim::hihat);
    headNode->attachRenderable(head.setTextures(Texture::silver));
    headNode->attachToNode(root);
  }

  void createPuppet(Node * root, kind kind)
  {
    Renderable body(Shader::parallax, VBO::puppetBody);
    root->attachRenderable(body.setTextures(Texture::puppetBody, Texture::puppetBump));

    int woodShininess = _TV(30);
    { // tête
      glLoadIdentity();
      // HACK ! L'id du node est mis dans BirthDate !
      Node * headNode = Node::New(kind, worldEndDate);
      headNode->setAnimation(Anim::head);
      Renderable head(Shader::parallax, VBO::puppetHead);
      head.setShininess(woodShininess);
      glTranslatef(_TV(0.f), _TV(11.f), _TV(0.f));
      headNode->attachRenderableNode(head.setTextures(Texture::puppetHead, Texture::puppetBump));

      glTranslatef(_TV(0.f), _TV(5.25f), _TV(0.f));
      Renderable hat(Shader::parallax, VBO::puppetHat);
      Texture::id textures[3] = {Texture::candle, Texture::silver, Texture::gold};
      hat.setTextures(textures[kind]);
      hat.setShininess(_TV(50));
      headNode->attachRenderableNode(hat);
      headNode->attachToNode(root);
    }

    // jambes
    float ecartJambes = _TV(2.f);
    glLoadIdentity();
    glTranslatef(_TV(10.f), _TV(-1.f), -ecartJambes);
    glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Renderable leg(Shader::parallax, VBO::puppetLeg);
    leg.setShininess(woodShininess);
    leg.setTextures(Texture::wood_Noyer);
    root->attachRenderableNode(leg);
    glTranslatef(_TV(0.f), _TV(0.f), 2.f * ecartJambes);
    root->attachRenderableNode(leg);

    // pieds
    glLoadIdentity();
    glTranslatef(_TV(16.f), _TV(0.75f), -ecartJambes);
    glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Renderable foot(Shader::parallax, VBO::puppetFoot);
    foot.setTextures(Texture::wood_MerisierClair);
    foot.setShininess(woodShininess);
    root->attachRenderableNode(foot);
    glTranslatef(_TV(0.f), _TV(0.f), 2.f * ecartJambes);
    root->attachRenderableNode(foot);

    // épaules
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(9.f), _TV(-3.5f));
    Renderable shoulder(Shader::parallax, VBO::puppetShoulder);
    shoulder.setTextures(Texture::wood);
    shoulder.setShininess(woodShininess);
    root->attachRenderableNode(shoulder);

    {
      // bras gauche
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(9.f), _TV(-3.5f));
      glRotatef(_TV(70.f), _TV(0.f), _TV(0.f), _TV(1.f));
      createArm(root, true, kind);

      // bras droit
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(9.f), _TV(3.5f));
      glRotatef(_TV(70.f), _TV(0.f), _TV(0.f), _TV(1.f));
      createArm(root, false, kind);
    }
  }

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    createPuppet(root, eHihat);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(_TV(40.f), _TV(0.f), _TV(0.f));
    glRotatef(_TV(190.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Node * p2 = Node::New(worldStartDate, worldEndDate);
    createPuppet(p2, eClap);
    p2->attachToNode(root);
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(_TV(83.f), _TV(-34.f), _TV(-8.f));
    glRotatef(_TV(-108.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Node * p3 = Node::New(worldStartDate, worldEndDate);
    createPuppet(p3, eXylo);
    p3->attachToNode(root);
    glPopMatrix();

    createHihat(root);
    return root;
  }


  // Génération des meshs

  static float coneRFunc(float t, float theta)
  {
    return _TV(-1.f) * t + _TV(1.001f);
  }

  void generateHihat()
  {
    Array<vertex> & tmp = Mesh::getTemp();

    Mesh::Revolution foot(Mesh::pipeHFunc, coneRFunc);
    foot.generateMesh(tmp, _TV(2.f), _TV(3.f), _TV(2), _TV(12));
    Mesh::translate(tmp, _TV(0.f), _TV(-8.f), _TV(0.f));

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(tmp, _TV(18.f), _TV(0.25f), _TV(5), _TV(6));
    VBO::setupData(VBO::hihatBody, tmp DBGARG("hihatBody"));
    tmp.empty();

    wheel.generateMesh(tmp, _TV(0.5f), _TV(4.f), _TV(6), _TV(14));
    VBO::setupData(VBO::hihatHead, tmp DBGARG("hihatHead"));
  }

  void generateMeshes()
  {
    //Mesh::Revolution body = Mesh::loadSplineFromFile("data/revolution.txt");

    Mesh::clearPool();
    Array<vertex> & tmp = Mesh::getTemp();
    Mesh::Revolution body(NULL, NULL, bodyData, ARRAY_LEN(bodyData));
    body.generateMesh(tmp, _TV(10.f), _TV(10.f), _TV(16), _TV(10));
    VBO::setupData(VBO::puppetBody, tmp DBGARG("puppetBody"));
    tmp.empty();

    // nez et tête
    Mesh::Revolution sphere(Mesh::sphereHFunc, Mesh::sphereRFunc);
    sphere.generateMesh(tmp, _TV(0.5f), _TV(0.5f), _TV(6), _TV(6));
    Mesh::scale(tmp, _TV(1.4f), _TV(1.f), _TV(1.f));
    Mesh::translate(tmp, _TV(2.5f), _TV(3.2f), _TV(0.f));
    Mesh::reprojectTextureXZPlane(tmp, _TV(0.1f));

    Mesh::Revolution head(NULL, NULL, headData, ARRAY_LEN(headData));
    head.generateMesh(tmp, _TV(10.f), _TV(10.f), _TV(16), _TV(10));

    VBO::setupData(VBO::puppetHead, tmp DBGARG("puppetHead"));
    tmp.empty();

    // chapeau
    Mesh::Revolution hat(NULL, NULL, hatData, ARRAY_LEN(hatData));
    hat.generateMesh(tmp, _TV(10.f), _TV(10.f), _TV(10), _TV(14));
    VBO::setupData(VBO::puppetHat, tmp DBGARG("puppetHat"));
    tmp.empty();

    Mesh::Revolution leg(NULL, NULL, legData, ARRAY_LEN(legData));
    //Mesh::Revolution leg = Mesh::loadSplineFromFile("data/revolution.txt");
    leg.generateMesh(tmp, _TV(10.f), _TV(10.f), _TV(10), _TV(14));
    VBO::setupData(VBO::puppetLeg, tmp DBGARG("puppetLeg"));
    tmp.empty();

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(tmp, _TV(2.f), _TV(2.f), _TV(10), _TV(14));
    Mesh::scale(tmp, _TV(2.f), _TV(0.5f), _TV(0.5f));
    VBO::setupData(VBO::puppetFoot, tmp DBGARG("puppetFoot"));
    tmp.empty();

    Mesh::Revolution shoulder(Mesh::pipeHFunc, Mesh::pipeRFunc);
    shoulder.generateMesh(tmp, _TV(7.f), _TV(0.25f), _TV(2), _TV(5));
    glLoadIdentity();
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::applyCurrentProjection(tmp);
    VBO::setupData(VBO::puppetShoulder, tmp DBGARG("puppetShoulder"));
    tmp.empty();

    Mesh::Revolution arm(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(tmp, _TV(6.f), _TV(0.75f), _TV(10), _TV(14));
    Mesh::translate(tmp, _TV(0.f), _TV(-2.25f), _TV(0.f));
    VBO::setupData(VBO::puppetArm, tmp DBGARG("puppetArm"));
    tmp.empty();

    Mesh::Revolution forearm(NULL, NULL, forearmData, ARRAY_LEN(forearmData));
    forearm.generateMesh(tmp, _TV(7.f), _TV(1.25f), _TV(10), _TV(16));
    VBO::setupData(VBO::puppetForearm, tmp DBGARG("puppetForearm"));
    tmp.empty();

    wheel.generateMesh(tmp, _TV(10.f), _TV(0.25f), _TV(3), _TV(6));
    VBO::setupData(VBO::hihatstick, tmp DBGARG("hihatstick"));
    tmp.empty();

    sphere.generateMesh(tmp, _TV(1.f), _TV(1.f), _TV(8), _TV(10));
    Mesh::translate(tmp, _TV(0.f), _TV(3.f), _TV(0.f));
    wheel.generateMesh(tmp, _TV(5.f), _TV(0.25f), _TV(3), _TV(6));
    Mesh::translate(tmp, _TV(0.f), _TV(2.5f), _TV(0.f));
    VBO::setupData(VBO::xylostick, tmp DBGARG("xylostick"));
    tmp.empty();

    generateHihat();
  }
}
