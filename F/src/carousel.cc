#include "sys/msys.h"

#include "interpolation.hh"
#include "intro.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "shaderid.hh"
#include "shaders.hh"
#include "../snd/sync.hh"
#include "textureid.hh"
#include "timing.hh"
#include "transformation.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"
#include "workshop.hh"

#include "phy/world.hh"
#include "phy/phy_mesh.hh"

namespace Carousel
{
  static Array<Sync::Note> notes[128];
  static int numberOfNotes = 0;
  static phy::Mesh* phyMesh;

  static const float bellYPos = 14.9f;

  // FIXME
  // Il faudrait mettre tous ces tableaux .*Data (plus de ceux de
  // chess.cc) dans un fichier à part
  static float bodyData[] = {
    0, 0, 40,
    0, 5, 40,
    0, 9, 30,
    0, 9, 30,
    0, 13, 38,

    0, 13, 38,
    0, 16, 30,
    0, 21, 30,
    0, 38, 42,
    0, 43, 42,
    0, 47, 30,

    0, 47, 30,
    0, 49, 40,
    0, 51, 40,
    0, 53, 30,

    0, 53, 30,
    0, 57, 42,
    0, 62, 42,
    0, 79, 30,
    0, 84, 30,
    0, 87, 38,

    0, 87, 38,
    0, 91, 30,
    0, 91, 30,
    0, 95, 40,
    0, 100, 40,
  };
  static const int bodyDataLen = sizeof(bodyData) / sizeof(bodyData[0]);

  static float headData[] =
  {
    0, 0, 0,
    0, 1, 90,
    0, 4, 100,
    0, 20, 100,
    0, 23, 90,
    0, 35, 40,
    0, 60, 5,
    0, 61, 5,
    0, 72, 14,
    0, 88, 12,
    0, 100, 0
  };
  static const int headDataLen = sizeof(headData) / sizeof(headData[0]);

  static int idToNote[30];

  static void setUniforms(Shader::id sid, int objid)
  {
    Array<Sync::Note> & arr = notes[idToNote[objid / 2]];
    int time = (int) intro.now.youtube;
    if (time < _TV(165900)) return;

    int i;
    for (i = 0; i < arr.size; i++)
      if (arr[i].time > (int) time)
	break;

    float col = 0.f;
    if (i > 0)
    {
      const Sync::Note & n = arr[i-1];
      const int noteDuration = _TV(500); // en ms
      col = 1.f - smoothStepI(n.time, n.time + noteDuration, time);
    }
    Shader::setUniform1f(sid, Shader::trans, col);
  }

#define HAUTEUR_CORPS_CAROUSEL    14.f
#define DIAMETRE_CORPS_CAROUSEL   1.6f

#define HAUTEUR_CHAPEAU_CAROUSEL  4.f
#define DIAMETRE_CHAPEAU_CAROUSEL 10.f

#define HAUTEUR_SOCLE_CAROUSEL    0.8f
#define HAUTEUR_PIEDS_CAROUSEL    0.4f
#define ECART_PIEDS_CAROUSEL      4.0f

#define HAUTEUR_CLOCHE            8.f
#define DIAMETRE_CLOCHE           0.8f
#define RAPPORT_EPAISSEUR_CLOCHE  0.3f
#define DISTANCE_CLOCHE           4.f

#define LONGUEUR_FIL              1.f
#define EPAISSEUR_FIL             0.05f

  void generateMeshes()
  {
    Mesh::Revolution mast(NULL, NULL, bodyData, bodyDataLen);
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution hemisphere(Mesh::hemisphereHFunc, Mesh::hemisphereRFunc);
    Mesh::Revolution head(NULL, NULL, headData, headDataLen);

    Array<vertex> & mastMesh = Mesh::getTemp();
    Array<vertex> & socleMesh = Mesh::getTemp();
    Array<vertex> & baseMesh = Mesh::getTemp();
    Array<vertex> & headMesh = Mesh::getTemp();
    Array<vertex> & bodyMesh = Mesh::getTemp();

    mast.generateMesh(mastMesh, HAUTEUR_CORPS_CAROUSEL, 0.5f * 2.5f * DIAMETRE_CORPS_CAROUSEL, 32, 8);
    wheel.generateMesh(socleMesh, HAUTEUR_SOCLE_CAROUSEL, 0.5f * DIAMETRE_CHAPEAU_CAROUSEL, 7, 16);
    hemisphere.generateMesh(baseMesh, HAUTEUR_PIEDS_CAROUSEL, HAUTEUR_PIEDS_CAROUSEL, 3, 6);
    head.generateMesh(headMesh, HAUTEUR_CHAPEAU_CAROUSEL, 0.5f * DIAMETRE_CHAPEAU_CAROUSEL, 16, 16);

    const float textureScale = _TV(0.05f);
    Mesh::reprojectTextureXYPlane(mastMesh,  4.f * textureScale);
    Mesh::reprojectTextureXZPlane(socleMesh, textureScale);
    Mesh::reprojectTextureXZPlane(baseMesh,  textureScale);
    Mesh::reprojectTextureXZPlane(headMesh,  textureScale);

    for (unsigned int i = 0; i < 4; ++i)
    {
      glLoadIdentity();
      glRotatef(90.f * i, 0, 1.f, 0);
      glRotatef(180.f, 1.f, 0, 0);
      glTranslatef(ECART_PIEDS_CAROUSEL, -HAUTEUR_PIEDS_CAROUSEL, 0);
      Mesh::addVerticesUsingCurrentProjection(bodyMesh, baseMesh);
    }

    glLoadIdentity();
    glTranslatef(0, HAUTEUR_PIEDS_CAROUSEL + 0.5f * HAUTEUR_SOCLE_CAROUSEL, 0);
    Mesh::addVerticesUsingCurrentProjection(bodyMesh, mastMesh);
    Mesh::addVerticesUsingCurrentProjection(bodyMesh, socleMesh);

    VBO::setupData(VBO::carouselBody, bodyMesh DBGARG("carouselBody"));


    Mesh::Revolution outPipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Array<vertex> & bellThread = Mesh::getTemp();
    outPipe.generateMesh(bellThread, LONGUEUR_FIL, 0.5f * EPAISSEUR_FIL, 1, 3);


    Array<vertex> & bellMesh = Mesh::getTemp();
    Mesh::addVerticesUsingCurrentProjection(bellMesh, bellThread);
    glTranslatef(0, LONGUEUR_FIL, 0);
    Mesh::generateTorusCylinder(bellMesh, HAUTEUR_CLOCHE, DIAMETRE_CLOCHE, RAPPORT_EPAISSEUR_CLOCHE, 8);
    VBO::setupData(VBO::carouselBell, bellMesh DBGARG("carouselBell"));



    VBO::setupData(VBO::carouselHead, headMesh DBGARG("carouselHead"));
  }

  void physicsAnimation(const Node & node, date d)
  {
    int pid = node.visiblePart()[0].id();
    phy::Particle & p1 = phyMesh->particles[pid]; // la particule fixe
    phy::Particle & p2 = phyMesh->particles[pid+1]; // la particule mobile

    glTranslatef(p1.position().x, p1.position().y, p1.position().z);
    vector3f up(0, 1, 0);
    vector3f dir = p2.position() - p1.position();
    vector3f axis = up ^ dir;
    glRotatef(RAD_TO_DEG * angle(up, dir), axis.x, axis.y, axis.z);
  }

  // calcule la position de la partie fixe du carousel, en fonction de l'angle de rotation
  static void getPosition(int id, float rotationAngle, float & x, float & z)
  {
    float defaultAngle = id * (2.f * PI) / numberOfNotes;
    float angle = rotationAngle - defaultAngle;
    x = msys_sinf(angle) * DISTANCE_CLOCHE;
    z = msys_cosf(angle) * DISTANCE_CLOCHE;
  }

  static void physicMesh()
  {
#if DEBUG
    bool reinit = false;
    if (phyMesh != 0) {
      delete phyMesh;
      Workshop::world.meshes.empty();
      reinit = true;
    }
#endif

    phy::Mesh* res = new phy::Mesh(2 * numberOfNotes, numberOfNotes, 4);

    for (int i = 0; i < numberOfNotes; i++)
    {
      float x, z;
      getPosition(i, PI / 180.f, x, z);

      phy::Particle p1(vector3f(x, bellYPos, z));
      p1.friction(0.f);
      p1.mobility(0.f);
      res->particles.add(p1);

      phy::Particle p2(vector3f(x, bellYPos - HAUTEUR_CLOCHE - LONGUEUR_FIL, z));
      p2.friction(0.99f);
      res->particles.add(p2);

      res->constraints.add(phy::rigid_constraint(&res->particles[2*i],
						 &res->particles[2*i+1],
						 HAUTEUR_CLOCHE + LONGUEUR_FIL));
    }

    phyMesh = res;
    Workshop::world.meshes.add(res);

#if DEBUG
    if (reinit) Workshop::update(0);
#endif
  }

  // regarde si une note doit être jouée et applique une force sur la particule
  static void playNote(date t)
  {
    static int last = -1;
    Array<Sync::Note> & arr = Sync::sheet[Instrument::Carousel];
    int i;
    for (i = 0; i < arr.size && arr[i].time < (int) t; i++);

    if (i != last && i < arr.size)
    {
      const Sync::Note & n = arr[i];

      int k;
      for (k = 0; k < numberOfNotes; k++)
	if (idToNote[k] == n.note) break;
      if (k == numberOfNotes) return; // FIXME: à virer
      assert(k < numberOfNotes);

      phy::Particle & p = phyMesh->particles[2*k+1];

      vector3f speed = p.speed();
      speed.z = _TV(-8.f);
      speed.x += msys_sfrand() * _TV(0.5f);
      p.speed(speed);
      last = i;
    }
  }

  // appelé toutes les 10ms
  void update(date t)
  {
    playNote(t);
    static float rotationAngle = 0.f;

#if DEBUG // retour dans le temps
    if (t == 0) {
      rotationAngle = 0.f;
      return;
    }
#endif

    // vitesse : nombre de degré pour 10ms
    float speed = _TV(0.022f) + msys_cosf(t * _TV(0.001f)) * _TV(0.003f);
    speed *= smoothStepI(163000, 168000, t);
    speed *= 1.f - smoothStepI(214000, 216000, t);

    rotationAngle += speed;

    for (int i = 0; i < numberOfNotes * 2; i += 2)
    {
      phy::Particle & p = phyMesh->particles[i];
      float x, z;
      getPosition(i / 2, rotationAngle, x, z);
      p.move_to(vector3f(x, bellYPos, z));
    }
  }

  Node* create()
  {
    IFDBG(if (!intro.initDone))
      Sync::getSheetForEachNote(Instrument::Carousel, notes, 1);
    numberOfNotes = 0;
    for (int i = 0; i < 128; i++)
      if (notes[i].size > 0)
        idToNote[numberOfNotes++] = i;

    physicMesh();

    Node * root = Node::New(worldStartDate, worldEndDate);

    int shininess = _TV(100);
    int metalShininess = _TV(60);

    glLoadIdentity();
    Renderable body(Shader::parallax, VBO::carouselBody);
    body.setTextures(Texture::wood);
    body.setShininess(shininess);
    root->attachRenderableNode(body);

    glTranslatef(0.f, HAUTEUR_CORPS_CAROUSEL, 0.f);
    Renderable head(Shader::parallax, VBO::carouselHead);
    head.setTextures(Texture::wood);
    head.setShininess(shininess);
    root->attachRenderableNode(head);

    glLoadIdentity();
    for (int i = 0; i < 2 * numberOfNotes; i+=2)
    {
      Node * node = Node::New(worldStartDate, worldEndDate);
      Renderable rope(Shader::anisotropic, VBO::carouselBell);
      rope.setShininess(metalShininess);
      rope.setId(i);
      rope.setCustomSetUniform(setUniforms);
      node->setAnimation(Anim::carouselPhysics);
      node->attachRenderable(rope);
      node->attachToNode(root);
    }

    return root;
  }
}
