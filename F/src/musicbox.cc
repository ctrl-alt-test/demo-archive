#include "sys/msys.h"

#include "intro.hh"
#include "interpolation.hh"
#include "renderable.hh"
#include "node.hh"
#include "revolution.hh"
#include "shaderid.hh"
#include "../snd/sync.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace MusicBox
{
  static int rotationDuration = 30000; // ms
  static int noteMin = 25;

  static Array<Sync::Note> notes[128];

  static float coverData[] = {
    // Dessous (temporaire)
    0, 0, 0,
    0, 0, 90,
    0, 0, 99,

    // Dessus
    0, 0, 100,
    0, 40, 90,
    0, 70, 60,
    0, 80, 21,
    0, 81, 20,
    0, 95, 10,
    0, 100, 4,
    0, 100, 1,
    0, 100, 0,
  };

  static float boxData[] = {
    0, 0, 40,
    0, 40, 32,
    0, 100, 40,
    0, 100, 35,
    0, 100, 20,
    0, 90, 5,
    0, 0, 1,
  };

  float gearsRFunc(float t, float theta) {
    // pour fermer en haut et en bas
    float close = (t < 0.1f || t > 0.9f) ? 0.0001f : 1.f;

    float tooth = msys_sinf(theta * _TV(16.f) + 0.25f * PI);
    tooth = clamp(tooth * _TV(4.f), -1.f, 1.f);
    return close * (1.f + tooth * _TV(0.1f));
  }

  vector3f gearsHFunc(float t, float theta) {
    return vector3f(0.f, t < 0.5f ? 0.f : 1.f, 0.f);
  }

  // Génère la boite et le contenu statique.
  void generateBox()
  {
    // boite cachant le mécanisme
    Array<vertex> & mechanismMesh = Mesh::getTemp();
    Mesh::generatePave(mechanismMesh, _TV(10.f), _TV(5.f), _TV(5.f));
    VBO::setupData(VBO::mbMechanism, mechanismMesh DBGARG("mbMechanism"));

    // boite
    Array<vertex> & boxMesh = Mesh::getTemp();
    Mesh::Revolution rev(NULL, NULL, boxData, ARRAY_LEN(boxData));
    rev.generateMesh(boxMesh, _TV(7.25f), _TV(6.f), _TV(12), _TV(-4));
    Mesh::rotate(boxMesh, 45.f, 0, 1.f, 0);
    Mesh::expandPave(boxMesh, _TV(8.25f), _TV(0.f), _TV(8.25f));
    Array<vertex> & bottom = Mesh::getTemp();
    Mesh::generatePave(bottom, _TV(18.f), _TV(1.f), _TV(18.f));
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(boxMesh, bottom);
    VBO::setupData(VBO::mbBox, boxMesh DBGARG("mbBox"));

    // pour la clé
    Array<vertex> & keyHole = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(keyHole, _TV(0.25f), _TV(1.f), _TV(5), _TV(-8));
    Mesh::rotate(keyHole, 180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::expandPave(keyHole, _TV(0.75f), 0, _TV(0.25f));
    Mesh::rotate(keyHole, 90.f, 1.f, 0, 0);
    Mesh::translate(keyHole, _TV(0.f), _TV(3.f), _TV(9.75f));
    VBO::setupData(VBO::mbKeyHole, keyHole DBGARG("mbKeyHole"));
  }

  void generateMeshes()
  {
    glLoadIdentity();
    // Test d'orientation : ça joue sur l'éclairage
    if (_TV(1))
    {
      Array<vertex> & lameMesh = Mesh::getTemp();
      Mesh::generatePave(lameMesh, _TV(.125f), _TV(0.17f), _TV(3.5f));
      Mesh::rotate(lameMesh, _TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
      VBO::setupData(VBO::mbKey, lameMesh DBGARG("mbKey"));
    }
    else
    {
      VBO::generatePave(VBO::mbKey, _TV(0.17f), _TV(0.125f), _TV(3.5f));
    }

    Mesh::clearPool();

    // les pointes
    Array<vertex> & picMesh = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(picMesh, _TV(0.7f), _TV(0.05f), _TV(8), _TV(6));
    Mesh::translate(picMesh, _TV(0.f), _TV(2.4f), _TV(0.f));
    VBO::setupData(VBO::mbPic, picMesh DBGARG("mbPic"));

    // cylindre qui tourne avec l'engrenage
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Array<vertex> & bodyMesh = Mesh::getTemp();
    Mesh::generateTorusCylinder(bodyMesh, _TV(9.5f), _TV(5.f), _TV(0.25f), _TV(28));
    Mesh::translate(bodyMesh, _TV(0.f), _TV(1.5f), _TV(0.f));
    cyl.generateMesh(bodyMesh, _TV(17.f), _TV(0.75f), _TV(1), _TV(12));
    Mesh::scaleTexture(bodyMesh, _TV(1.f), _TV(2.f));

    Array<vertex> & gearMesh = Mesh::getTemp();
    Mesh::Revolution gear(gearsHFunc, gearsRFunc);
    gear.generateMesh(gearMesh, _TV(0.5f), _TV(3.f), _TV(5), _TV(-64));

    // Gros hack pour corriger un artefact:
    // On définit à la main la normale des deux faces des roues dentées
    {
      // 4 sommets x 64 faces x 2 (sommets en double pour les arrêtes)
      const vector3f n1(0, -1.f, 0);
      const vector3f n2(0, 1.f, 0);
      for (int i = 0; i < 512; ++i)
      {
	float theta = PI * float((i + _TV(0))/_TV(4))/_TV(64.f);
	gearMesh[i].n = n1;
	gearMesh[i].t = vector3f(msys_cosf(theta), 0, msys_sinf(theta));
	gearMesh[gearMesh.size - i - 1].n = n2;
	gearMesh[gearMesh.size - i - 1].t = vector3f(msys_cosf(-theta), 0, msys_sinf(-theta));
      }
    }

    const float metalTextureScale = _TV(0.1f);
    Mesh::reprojectTextureXZPlane(gearMesh, metalTextureScale);

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(15.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(bodyMesh, gearMesh);
    VBO::setupData(VBO::mbBody, bodyMesh DBGARG("mbBody"));

    cyl.generateMesh(gearMesh, _TV(17.f), _TV(0.5f), _TV(1), _TV(12));
    VBO::setupData(VBO::mbGear, gearMesh DBGARG("mbGear"));

    // petit cylindre pour poser les touches
    Array<vertex> & cylMesh = Mesh::getTemp();
    cyl.generateMesh(cylMesh, _TV(17.f), _TV(0.125f), _TV(1), _TV(8));
    VBO::setupData(VBO::mbSmallCylinder, cylMesh DBGARG("mbSmallCylinder"));


    Mesh::Revolution cover(NULL, NULL, coverData, ARRAY_LEN(coverData));
    Array<vertex> & coverMesh = Mesh::getTemp();
    cover.generateMesh(coverMesh, _TV(1.5f), _TV(1.44f), _TV(32), _TV(-4));
    Mesh::rotate(coverMesh, 45.f, 0, 1.f, 0);
    Mesh::expandPave(coverMesh, _TV(9.f), _TV(0.f), _TV(9.f));
    const float woodTextureScale = _TV(0.05f);
    Mesh::reprojectTextureXZPlane(coverMesh, woodTextureScale);
    VBO::setupData(VBO::mbCover, coverMesh DBGARG("mbCover"));

    // triangle
    Array<vertex> & triangle = Mesh::getTemp();
    wheel.generateMesh(triangle, _TV(1.25f), _TV(1.5f), _TV(32), _TV(-3));
    Mesh::expandPave(triangle, _TV(4.f), _TV(1.5f), _TV(0.f));
    Mesh::translate(triangle, _TV(2.f), _TV(2.f), _TV(-6.f));
    Mesh::reprojectTextureXZPlane(triangle, _TV(0.5f));
    VBO::setupData(VBO::mbTriangle, triangle DBGARG("mbTriangle"));

    generateBox();
  }

  void vibrationAnimation(const Node & node, date time)
  {
    const int upTime = _TV(600); // temps pour lever la lame
    const int downTime = upTime + _TV(80); // fin de l'animation

    int id = node.visiblePart()[0].id();
    Array<Sync::Note> & arr = notes[id];

    int i;
    for (i = 0; i < arr.size; i++) // FIXME: trop lent ? dichotomie ? sauvegarde du dernier i ?
      if (arr[i].time > (int) time)
	break;
    if (i == arr.size) return;

    int diff = arr[i].time - (int) time;
    if (diff > downTime) return; // temps de vibration
    diff = downTime - diff;

    float ampl;
    if (diff < upTime)
      ampl = (float) diff / upTime;
    else
      ampl = 1.f - smoothStepI(upTime, downTime, diff);

    glTranslatef(_TV(0.f), _TV(0.f), _TV(-2.f));
    glRotatef(_TV(-2.f) * ampl, 1.f, 0.f, 0.f);
    glTranslatef(_TV(0.f), _TV(0.f), _TV(2.f));
  }

  static inline void createLames(Node *root)
  {
    IFDBG(if (!intro.initDone))
      Sync::getSheetForEachNote(Instrument::MusicBox, notes, 1);

    int firstNote = 0;
    int lastNote = 0;
    for (int i = 1; i < 128; i++)
    {
      if (notes[i].size == 0) continue;
      if (firstNote == 0) firstNote = i;
      lastNote = i;
    }

    const int deviceShininess = _TV(35);

    for (int i = firstNote; i < lastNote; i++)
    {
      glLoadIdentity();
      glTranslatef((float) (i-noteMin) * _TV(0.25f), _TV(1.f), _TV(-4.25f));
      Node * node = Node::New(worldStartDate, worldEndDate);
      Renderable lame(Shader::anisotropic, VBO::mbKey);
      lame.setId(i);
      lame.setShininess(deviceShininess);
      lame.setTextures(Texture::mbCylinder);
      node->setAnimation(Anim::vibration);
      node->attachRenderable(lame);
      node->attachToNode(root);
    }
  }

  // Rotation des pics sur le cylindre
  void rotationAnimation(const Node & node, date d)
  {
    glRotatef((float)(180 + d * 360 / (float) rotationDuration), 1.f, 0.f, 0.f);
  }

  // Rotation du cylindre principal
  void cylinderAnimation(const Node & node, date d)
  {
    glRotatef((float)(_TV(180) + d * 360 / (float) rotationDuration), _TV(0.f), _TV(-1.f), _TV(0.f));
  }

  // ouverture de la boite
  void openingAnimation(const Node & node, date time)
  {
    float opening = smoothStepI(_TV(34800), _TV(35500), time);
    float angle = mix(_TV(0.f), _TV(-73.f), opening);
    glTranslatef(_TV(10.f), _TV(0.f), _TV(-10.f));
    glRotatef(angle, _TV(1.f), _TV(0.f), _TV(0.f));
    glTranslatef(_TV(0.f), _TV(0.f), _TV(10.f));
  }

  Node* create()
  {
    const int animationEndDate = _TV(69000);
    Node * root = Node::New(worldStartDate, worldEndDate);

    const int woodShininess = _TV(80);
    const int deviceShininess = _TV(32);

    glLoadIdentity();

    const Array<Sync::Note> & arr = Sync::sheet[Instrument::MusicBox];
    for (int i = 0; i < arr.size; i++)
    {
      const Sync::Note n = arr[i];

      if (n.vol == 0 || n.note == 0) continue;

      const int start = max(0, n.time - _TV(9600)); // temps en ms avant le son
      glPushMatrix();
      glTranslatef((float) (n.note-noteMin) * _TV(0.25f), _TV(0.f), _TV(0.f));

      Node * node = Node::New(start, start + rotationDuration);
      Renderable pic(Shader::anisotropic, VBO::mbPic);
      node->setAnimation(Anim::musicBoxRotation);
      node->setAnimationStartDate(start);
      pic.setTextures(Texture::mbCylinder);
      pic.setShininess(deviceShininess);
      node->attachRenderable(pic);
      node->attachToNode(root);
      glPopMatrix();
    }

    createLames(root);

    // gros cylindre avec engrenage
    {
      glLoadIdentity();
      glTranslatef(_TV(21.25f), _TV(0.f), _TV(0.f));
      glRotatef(90.f, 0.f, 0.f, 1.f);
      Node * node = Node::New(worldStartDate, worldEndDate);
      node->setAnimation(Anim::musicBoxCylRotation);
      node->setAnimationStopDate(animationEndDate);
      Renderable body(Shader::anisotropic, VBO::mbBody);
      body.setTextures(Texture::mbCylinder);
      body.setShininess(deviceShininess);
      node->attachRenderable(body);
      node->attachToNode(root);
    }

    // petit engrenage
    {
      glLoadIdentity();
      glTranslatef(_TV(5.8f), _TV(0.53f), _TV(5.5f));
      glRotatef(_TV(-90.f), 0.f, 0.f, 1.f); // -90 pour que l'animation soit inversée
      glScalef(_TV(0.75f), _TV(0.75f), _TV(0.75f));
      Node * node = Node::New(worldStartDate, worldEndDate);
      node->setAnimation(Anim::musicBoxCylRotation);
      node->setAnimationStopDate(animationEndDate);
      Renderable body(Shader::anisotropic, VBO::mbGear);
      body.setShininess(deviceShininess);
      node->attachRenderable(body.setTextures(Texture::mbCylinder));
      node->attachToNode(root);
    }

    // boite interne et cylindre de support
    {
      glLoadIdentity();
      glTranslatef(_TV(15.f), _TV(-1.f), _TV(5.25f));
      Renderable mechanism(Shader::parallax, VBO::mbMechanism);
      mechanism.setTextures(Texture::black, Texture::mbCylinderBump);
      mechanism.setShininess(_TV(40));
      root->attachRenderableNode(mechanism);

      glLoadIdentity();
      glTranslatef(_TV(21.5f), _TV(0.8f), _TV(-3.75f));
      Renderable smallCyl(Shader::anisotropic, VBO::mbSmallCylinder);
      smallCyl.setShininess(deviceShininess);
      glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
      smallCyl.setTextures(Texture::mbCylinder);
      root->attachRenderableNode(smallCyl);
      glTranslatef(_TV(0.f), _TV(0.f), _TV(-1.f));
      root->attachRenderableNode(smallCyl);
    }

    // la boite
    {
      glLoadIdentity();
      glTranslatef(_TV(13.f), _TV(-3.f), _TV(0.f));
      Renderable body(Shader::parallax, VBO::mbBox);
      body.setShininess(woodShininess);
      root->attachRenderableNode(body.setTextures(Texture::wood_MerisierClair, Texture::woodBump1));

      Renderable keyHole(Shader::parallax, VBO::mbKeyHole);
      root->attachRenderableNode(keyHole.setTextures(Texture::gold));

      Renderable triangle(Shader::parallax, VBO::mbTriangle);
      root->attachRenderableNode(triangle.setTextures(Texture::black, Texture::mbCylinderBump));
    }

    // couvercle
    {
      glLoadIdentity();
      glTranslatef(_TV(3.f), _TV(4.f), _TV(0.f));
      Renderable cover(Shader::parallax, VBO::mbCover);
      cover.setShininess(woodShininess);
      Node * node = Node::New(worldStartDate, worldEndDate);
      node->setAnimation(Anim::musicBoxOpening);
      node->attachRenderable(cover.setTextures(Texture::wood_MerisierClairAliceJuliette, Texture::woodBump1));
      node->attachToNode(root);
    }

    {
      glLoadIdentity();
      glTranslatef(_TV(13.f), _TV(0.f), _TV(10.f));
      glRotatef(_TV(90.f), _TV(0.f), _TV(1.f), _TV(0.f));
      Renderable key(Shader::anisotropic, VBO::mbDoorKey);
      key.setShininess(_TV(40));
      Node * node = Node::New(worldStartDate, worldEndDate);
      node->setAnimation(Anim::musicBoxRotation);
      node->setAnimationStartDate(_TV(36000));
      node->setAnimationStopDate(animationEndDate);
      node->attachRenderable(key.setTextures(Texture::gold));
      node->attachToNode(root);
    }

    return root;
  }
}
