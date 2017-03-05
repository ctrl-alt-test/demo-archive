#include "sys/msys.h"

#include "interpolation.hh"
#include "intro.hh"
#include "renderable.hh"
#include "node.hh"
#include "revolution.hh"
#include "shaderid.hh"
#include "shaders.hh"
#include "../snd/sync.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Lousianne
{
  char *fingers[] = {
    "1000000",  // 0, G
    "0110000",  // 1, G#
    "1111111",  // 2, A
    "1011111",
    "1111110",  // 4, B
    "1111101",  // 5, C
    "1111100",
    "1111000",  // 7, D
    "1110100",
    "1110000",  // 9, E
    "1100000",  // 10, F
  };

  float souffletRFunc(float t, float theta) {
    if (t == 1.f)
      return 0.0001f;
    return 0.8f + 0.4f * msys_fabsf(msys_fmodf(10.f * t, 1.f) - 0.5f);
  }

  static void setUniforms(Shader::id sid, int objid)
  {
    int steps = _TV(4);

    int time = (int) intro.now.youtube;
    int idx = Sync::getLastNote(Instrument::Lousianne, time);
    const Array<Sync::Note> & sheet = Sync::sheet[Instrument::Lousianne];
    int last = idx == -1 ? 0 : sheet[idx].time;
    int next = idx == sheet.size - 1 ? time+1 : sheet[idx + 1].time;
    float progress = smoothStepI(last, next, time);
    float lastPos = (steps - idx % steps) / (float) steps;
    float nextPos = (steps - (idx + 1) % steps) / (float) steps;
    float p = mix(lastPos, nextPos, progress);
    Shader::setUniform1f(sid, Shader::trans, p);
  }

  // barres métalliques
  static void generateMetal()
  {
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution sphere(Mesh::sphereHFunc, Mesh::sphereRFunc);

    Array<vertex> & tmp = Mesh::getTemp();
    Array<vertex> & metal = Mesh::getTemp();
    cyl.generateMesh(tmp, _TV(10.f), _TV(0.25f), _TV(2), _TV(6));
    glTranslatef(_TV(0.f), _TV(0.f), _TV(-5.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);
    glTranslatef(_TV(0.f), _TV(0.f), _TV(10.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);

    glLoadIdentity(); tmp.empty();
    cyl.generateMesh(tmp, _TV(10.f), _TV(0.25f), _TV(2), _TV(6));
    glTranslatef(_TV(0.f), _TV(10.f), _TV(-5.f));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);

    glLoadIdentity(); tmp.empty();
    cyl.generateMesh(tmp, _TV(12.f), _TV(0.2f), _TV(1), _TV(6));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);

    glLoadIdentity(); tmp.empty();
    sphere.generateMesh(tmp, _TV(0.4f), _TV(0.4f), _TV(6), _TV(8));
    glTranslatef(_TV(0.f), _TV(12.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(10.f), _TV(5.f));
    glRotatef(90.f, _TV(1.f), 0, _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(10.f), _TV(-5.f));
    glRotatef(90.f, _TV(1.f), 0, _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(metal, tmp);

    VBO::setupData(VBO::lousianneMetal, metal DBGARG("lousianneMetal"));
  }

  void generateMeshes()
  {
    glLoadIdentity();

    Mesh::clearPool();
    Array<vertex> & flute = Mesh::getTemp();
    Array<vertex> & fluteOut = Mesh::getTemp();
    Array<vertex> & fluteIn = Mesh::getTemp();
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution cylIn(Mesh::inPipeHFunc, Mesh::pipeRFunc);
    cyl.generateMesh(fluteOut, _TV(15.f), _TV(0.5f), _TV(2), _TV(12));
    cylIn.generateMesh(fluteIn, _TV(15.f), _TV(0.4f), _TV(2), _TV(12));
    Mesh::scaleTexture(fluteOut, _TV(1.f), _TV(0.32f));
    Mesh::addVerticesUsingCurrentProjection(flute, fluteOut);
    Mesh::addVerticesUsingCurrentProjection(flute, fluteIn);
    VBO::setupData(VBO::flute, flute DBGARG("flute"));

    generateMetal();

    // ornements de la flute
    glLoadIdentity();
    Array<vertex> & fluteO = Mesh::getTemp();
    Mesh::Revolution tore(Mesh::torusHFunc, Mesh::torusRFunc);
    Array<vertex> & ring = Mesh::getTemp();
    tore.generateMesh(ring, _TV(1.75f), _TV(0.65f), _TV(4), _TV(12));

    Mesh::addVerticesUsingCurrentProjection(fluteO, ring);
    glTranslatef(_TV(0.f), _TV(6.5f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(fluteO, ring);
    glTranslatef(_TV(0.f), _TV(8.5f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(fluteO, ring);

    glLoadIdentity();
    Array<vertex> & barre = Mesh::getTemp();
    cyl.generateMesh(barre, _TV(15.f), _TV(0.125f), _TV(2), _TV(5));
    glTranslatef(_TV(1.1f), _TV(0.f), _TV(0.4f));
    Mesh::addVerticesUsingCurrentProjection(fluteO, barre);

    VBO::setupData(VBO::fluteOrnaments, fluteO DBGARG("fluteOrnaments"));

    // les clés
    glLoadIdentity();
    Array<vertex> & key = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::generatePave(key, _TV(0.75f), _TV(0.125f), _TV(0.125f));
    glTranslatef(_TV(-0.5f), _TV(0.f), _TV(0.1f));
    Array<vertex> & tire = Mesh::getTemp();
    wheel.generateMesh(tire, _TV(0.125f), _TV(0.5f), _TV(5), _TV(8));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(key, tire);

    VBO::setupData(VBO::fluteKey, key DBGARG("fluteKey"));

    // soufflet
    Mesh::Revolution souffletGen(Mesh::pipeHFunc, souffletRFunc);
    Array<vertex> & soufflet = Mesh::getTemp();
    souffletGen.generateMesh(soufflet, _TV(3.5f), _TV(3.f), _TV(30), _TV(18));
    Mesh::scaleTexture(soufflet, _TV(1.f), _TV(8.f));
    VBO::setupData(VBO::soufflet, soufflet DBGARG("soufflet"));

    // plateau
    Array<vertex> & plateau = Mesh::getTemp();
    wheel.generateMesh(plateau, _TV(2.f), _TV(6.f), _TV(5), _TV(20));
    Mesh::reprojectTextureXZPlane(plateau, _TV(0.05f));

    VBO::setupData(VBO::plateau, plateau DBGARG("plateau de lousianne"));

    // roues
    Array<vertex> & tmp = Mesh::getTemp();
    Array<vertex> & wheels = Mesh::getTemp();
    wheel.generateMesh(wheels, _TV(1.f), _TV(2.f), _TV(5), _TV(16));
	Mesh::reprojectTextureXZPlane(wheels, _TV(0.1f));
    Mesh::translate(wheels, _TV(0.f), _TV(7.f), _TV(0.f));
    glLoadIdentity();
    cyl.generateMesh(wheels, _TV(7.f), _TV(0.25f), _TV(2), _TV(6));
    Mesh::rotate(wheels, _TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    VBO::setupData(VBO::lousianneWheels, wheels DBGARG("lousianneWheels"));
  }

  void rotateAnimation(const Node & node, date d)
  {
    float angle = smoothStepI(_TV(89100), _TV(90100), d) * _TV(360.f);
    glRotatef(angle, _TV(0.f), _TV(1.f), _TV(0.f));
  }

  void rotateWheelsAnimation(const Node & node, date d)
  {
    float angle = smoothStepI(_TV(89100), _TV(90100), d) * _TV(600.f);
    glRotatef(angle, _TV(1.f), _TV(0.f), _TV(0.f));
  }

  void createFlute(Node *root)
  {
    Renderable flute(Shader::anisotropic, VBO::flute);
    Renderable fluteO(Shader::anisotropic, VBO::fluteOrnaments);
    flute.setTextures(Texture::flute);
    fluteO.setTextures(Texture::fluteOrnaments);

    const int shininess = _TV(40);
    flute.setShininess(shininess);
    fluteO.setShininess(shininess);

    root->attachRenderableNode(flute);
    root->attachRenderableNode(fluteO);

    glTranslatef(_TV(0.75f), _TV(-0.85f), _TV(0.5f));
    for (int i = 0; i < _TV(7); i++)
    {
      glTranslatef(_TV(0.f), _TV(2.1f), _TV(0.f));
      Node * key = Node::New(worldStartDate, worldEndDate);
      Renderable fluteK(Shader::anisotropic, VBO::fluteKey);
      fluteK.setTextures(Texture::fluteOrnaments);
      fluteK.setId(i);
      fluteK.setShininess(shininess);
      key->setAnimation(Anim::flute);
      key->attachRenderable(fluteK);
      key->attachToNode(root);
    }
  }

  static bool keyOn(int note, int key)
  {
    int octave = (note - 7) / 12;
    int octNote = (note - 7) % 12; // numéro de la note dans l'octave

    return fingers[octNote][key] == '1';
  }

  void fluteAnimation(const Node & node, date d)
  {
    const Instrument::Id instr = Instrument::Lousianne;
    const Array<Sync::Note> & arr = Sync::sheet[instr];
    int idx = Sync::getLastNote(instr, d);
    int keyId = node.visiblePart()[0].id();

    if (idx == arr.size - 1) return;

    bool last = idx == -1 ? false : keyOn(arr[idx].note, keyId);
    bool next = keyOn(arr[idx+1].note, keyId);
    int nextTime = arr[idx+1].time;

    float angle;
    if (last && next) // on laisse enfoncé
      angle = 0.f;
    else if (!last && !next)
      angle = 1.f;
    else
    {
      // décalage aléatoire
      nextTime += (int) (msys_sfrand(nextTime + keyId) * _TV(50.f));
      // ouverture
      angle = smoothStepI(nextTime-_TV(150), nextTime, d);
      // fermeture
      if (next) angle = 1.f - angle;
    }
    angle = mix(_TV(0.f), _TV(30.f), angle);

    glTranslatef(_TV(0.5f), _TV(0.f), _TV(0.f));
    glRotatef(angle, _TV(0.f), _TV(1.f), _TV(0.f));
    glTranslatef(_TV(-0.5f), _TV(0.f), _TV(0.f));
  }

  Node* create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    root->setAnimation(Anim::lousianneRotate);

    glLoadIdentity();
    glTranslatef(_TV(4.f), _TV(0.f), _TV(0.f));
    createFlute(root);
    glLoadIdentity();
    glTranslatef(_TV(-4.5f), _TV(0.f), _TV(0.f));
    createFlute(root);

    glLoadIdentity();
    Renderable soufflet(Shader::lousianne, VBO::soufflet);
    soufflet.setShininess(_TV(10));
    soufflet.setCustomSetUniform(setUniforms);
    root->attachRenderable(soufflet.setTextures(Texture::soufflet, Texture::souffletBump));

    Renderable metal(Shader::anisotropic, VBO::lousianneMetal);
    metal.setShininess(_TV(60));
    root->attachRenderable(metal.setTextures(Texture::brass));

    glTranslatef(_TV(0.f), _TV(-1.f), _TV(0.f));
    Renderable plateau(Shader::parallax, VBO::plateau);
    plateau.setShininess(_TV(30));
    root->attachRenderableNode(plateau.setTextures(Texture::wood_MerisierFonce, Texture::woodBump1));

    int numberOfWheels = _TV(8);
    for (int i = 0; i < numberOfWheels; i++)
    {
      glLoadIdentity();
      glTranslatef(_TV(0.f), _TV(-2.f), _TV(0.f));
      glRotatef(i * _TV(360.f) / numberOfWheels, _TV(0.f), _TV(1.f), _TV(0.f));
      glRotatef(msys_frand() * _TV(180.f), _TV(1.f), _TV(0.f), _TV(0.f));
      Node * wheelNode = Node::New(worldStartDate, worldEndDate);
      Renderable wheels(Shader::parallax, VBO::lousianneWheels);
      wheels.setId(i+_TV(0));
      wheels.setShininess(_TV(10));
      wheelNode->setAnimation(Anim::lousianneRotateWheels);
      wheelNode->attachRenderable(wheels.setTextures(Texture::wood_MerisierClair, Texture::woodBump1));
      wheelNode->attachToNode(root);
    }
    return root;
  }
}
