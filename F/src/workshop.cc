//
// Intérieur de l'atelier
//

#include "workshop.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "animid.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "matrix.hh"
#include "mesh.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "shaderid.hh"
#include "shaders.hh"
// #include "surface.hh"
#include "table.hh"
#include "textureid.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"

#include "armchair.hh"
#include "bookshelf.hh"
#include "carousel.hh"
#include "chess.hh"
#include "chimney.hh"
#include "clock.hh"
#include "desk.hh"
#include "drum.hh"
#include "harp.hh"
#include "lousianne.hh"
#include "orgalame.hh"
#include "pascontent.hh"
#include "puppet.hh"
#include "luxo.hh"
#include "musicbox.hh"
#include "shelf.hh"
#include "stool.hh"
#include "xylotrain.hh"

#define START_DATE	worldStartDate
#define END_DATE	worldEndDate

#define SHOW_TESTS 0

namespace Workshop
{
  phy::World world;

  // --------------------------------------------------------------------------

  static void scaleLight(Light::Light & light,
			 float ambientFactor,
			 float diffuseFactor,
			 float specularFactor)
  {
    light.ambientColor[0] *= ambientFactor;
    light.ambientColor[1] *= ambientFactor;
    light.ambientColor[2] *= ambientFactor;

    light.diffuseColor[0] *= diffuseFactor;
    light.diffuseColor[1] *= diffuseFactor;
    light.diffuseColor[2] *= diffuseFactor;

    light.specularColor[0] *= specularFactor;
    light.specularColor[1] *= specularFactor;
    light.specularColor[2] *= specularFactor;
  }

  void changeLightAndFog(date renderDate,
			 Light::Light & light0,
			 Light::Light & light1,
			 GLfloat * fogColor)
  {
    // Light 0
    light0.attenuation = _TV(0.003f);
    // On change la lum à 26s : on veut plus de lumière de la fenêtre dans
    // l'étagère, mais pas dans le plan où on voit la porte depuis la table
    light1.attenuation = renderDate < 26000 ? _TV(0.0014f) : _TV(0.001f);

    if (renderDate >= 32000 && renderDate < 62000)
    {
      // Cas particulier pour le titre
      scaleLight(light0, _TV(0.f), _TV(1.5f), _TV(1.f));
      scaleLight(light1, _TV(0.3f), _TV(0.75f), _TV(0.45f));
    }
    else if (renderDate > 165900 && renderDate < 217000)
    {
      scaleLight(light0, _TV(0.f), _TV(0.8f), _TV(1.f));
      scaleLight(light1, _TV(0.4f), _TV(1.f), _TV(0.5f));
      light1.attenuation = _TV(0.0007f);
    }
    else
    {
      scaleLight(light0, _TV(0.f), _TV(1.2f), _TV(1.f));
      scaleLight(light1, _TV(0.4f), _TV(1.f), _TV(0.45f));
    }

    // Pour le glow :
    light0.diffuseColor[3] = _TV(0.1f);
    light0.specularColor[3] = _TV(0.5f);

    light1.diffuseColor[3] = _TV(0.f);
    light1.specularColor[3] = _TV(0.5f);

    Shader::state.glowness = _TV(10.f);
  }

  // --------------------------------------------------------------------------

#define HAUTEUR_PLAFOND 250.f
#define LONGUEUR_PIECE  600.f
#define LARGEUR_PIECE   350.f

#if SHOW_TESTS
  static float biduleData[] = {
    0, 0, 80,
    0, 5, 70,
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
    0, 96, 40,
    0, 98, 50,
    0, 105, 70,
    0, 110, 72,
    0, 115, 65,
    0, 118, 55,
    0, 120, 0,
  };
  static const int biduleDataLen = sizeof(biduleData) / sizeof(biduleData[0]);

  inline
  static void _generateTestMeshes()
  {
    // test de heightmap
    Array<vertex> & hm = Mesh::getTemp();
    Texture::Channel t;
    t.Conic();
    t.ClampAndScale(0.f, 1.f);
    Mesh::generateFromHeightMap(hm, t, 12, 12);
    VBO::setupData(VBO::testhm, hm DBGARG("testhm"));


    Mesh::Revolution ball(Mesh::sphereHFunc, Mesh::sphereRFunc);
    Mesh::Revolution torus(Mesh::torusHFunc, Mesh::torusRFunc);

    ball.setComputeNormalFunc(Mesh::sphereComputeNormal);
    torus.setComputeNormalFunc(Mesh::torusComputeNormal);

    Array<vertex> & ballMesh = Mesh::getTemp();
    Array<vertex> & torusMesh = Mesh::getTemp();
    ball.generateMesh(ballMesh, 2.f, 2.f, 8, 16);
    torus.generateMesh(torusMesh, 10.f, 10.f, 16, 32);

    Mesh::rotateTangents(torusMesh);

    Array<vertex> test(40000);

    for (unsigned int k = 0; k < 4; ++k)
      for (unsigned int j = 0; j < 4; ++j)
	for (unsigned int i = 0; i < 4; ++i)
	{
	  glLoadIdentity();
	  glTranslatef(8.f * i, 10.f + 8.f * j, 8.f * k);

	  Mesh::addVerticesUsingCurrentProjection(test, ballMesh);
	}
    glLoadIdentity();
    glTranslatef(-20.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(test, torusMesh);

    VBO::setupData(VBO::test, test DBGARG("test"));


    Mesh::Revolution bidule(NULL, NULL, biduleData, biduleDataLen);
    bidule.setComputeTexCoordFunc(Mesh::axialTexture);
    Array<vertex> & biduleMesh = Mesh::getTemp();
    bidule.generateMesh(biduleMesh, 12.5f, 2.5f, 32, 16);

    VBO::setupData(VBO::test2, biduleMesh DBGARG("test2"));
  }
#endif // SHOW_TESTS

  inline
  static void _generateRoomMesh()
  {
    // Prototype à l'arrache :

    // sol et plafond
    Array<vertex> & floor = Mesh::getTemp();
    Mesh::generatePave(floor, LONGUEUR_PIECE, 1.f, LARGEUR_PIECE);
    VBO::setupData(VBO::ceil, floor DBGARG("ceil"));

    Mesh::splitFace(floor, &floor[12], _TV(8), _TV(1));
    Mesh::removeFace(floor, 12);
    for (int i = 0; i < floor.size; i += 4)
    {
      float startu = msys_sfrand();
      float startv = msys_sfrand();
      float width = _TV(0.2f) + msys_sfrand() * _TV(0.1f);
      float height = _TV(1.f) + msys_sfrand() * _TV(0.5f);
      floor[i].u = startu; floor[i].v = startv;
      floor[i+1].u = startu + width; floor[i+1].v = startv;
      floor[i+2].u = startu + width; floor[i+2].v = startv + height;
      floor[i+3].u = startu; floor[i+3].v = startv + height;
    }

    // murs
    Array<vertex> & wall1 = Mesh::getTemp();
    Array<vertex> & wall2 = Mesh::getTemp();
    Mesh::generatePave(wall1, LONGUEUR_PIECE, HAUTEUR_PLAFOND, 1.f);
    Mesh::generatePave(wall2, 1.f, HAUTEUR_PLAFOND, LARGEUR_PIECE);
    const float wallTextureScale = _TV(0.04f);
    Mesh::reprojectTextureXYPlane(wall1, wallTextureScale);
    Mesh::reprojectTextureZYPlane(wall2, wallTextureScale);

    Array<vertex> & room = Mesh::getTemp();

    glLoadIdentity();
    glTranslatef(0.5f * LONGUEUR_PIECE, -0.5f, 0.5f * LARGEUR_PIECE);
    Mesh::addVerticesUsingCurrentProjection(room, floor);
    VBO::setupData(VBO::floor, room DBGARG("floor"));
    room.empty();

    // mur derrière le bureau
    Array<vertex> & wallDesk = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(wallDesk, _TV(0.675f), _TV(0.305f), _TV(0.691f), _TV(0.345f));
    Mesh::translate(wallDesk, _TV(0.f), _TV(0.f), _TV(-0.5f));
    Mesh::scale(wallDesk, LONGUEUR_PIECE, HAUTEUR_PLAFOND, _TV(1.f));
    Mesh::reprojectTextureXYPlane(wallDesk, wallTextureScale);
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(room, wallDesk);

    // mur avec la cheminée
    Array<vertex> & wallChimney = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(wallChimney, _TV(0.43f), _TV(0.01f), _TV(0.57f), _TV(0.3f));
    Mesh::scale(wallChimney, LONGUEUR_PIECE, HAUTEUR_PLAFOND, _TV(1.f));
    Mesh::reprojectTextureXYPlane(wallChimney, wallTextureScale);
    glLoadIdentity();
    glTranslatef(0.f, _TV(0.f), LARGEUR_PIECE);
    Mesh::addVerticesUsingCurrentProjection(room, wallChimney);

    // mur avec la porte
    glLoadIdentity();
    glTranslatef(LONGUEUR_PIECE, 0.5f * HAUTEUR_PLAFOND, 0.5f * LARGEUR_PIECE);
    Mesh::addVerticesUsingCurrentProjection(room, wall2);

    // mur avec la fenêtre
    Array<vertex> & window = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(window, _TV(0.35f), _TV(0.35f), _TV(0.65f), _TV(0.85f));
    glLoadIdentity();
    glRotatef(_TV(-90.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glScalef(LARGEUR_PIECE, HAUTEUR_PLAFOND, 1.f);
    Mesh::applyCurrentProjection(window);
    Mesh::reprojectTextureZYPlane(window, wallTextureScale);
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(room, window);

    VBO::setupData(VBO::room, room DBGARG("room"));

    Array<vertex> & blackBackground = Mesh::getTemp();
    Mesh::generatePave(blackBackground, _TV(800.f), _TV(200.f), _TV(20.f));
    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(0.f), _TV(470.f));
    Mesh::addVerticesUsingCurrentProjection(blackBackground, blackBackground);
    Mesh::translate(blackBackground, _TV(300.f), _TV(0.f), _TV(-75.f));
    VBO::setupData(VBO::blackBackground, blackBackground DBGARG("blackBackground"));
  }

  static vector3f handleHFunc(float t, float theta) {
    float handleData[] =
    { // t,     x,  z
         0,     _TV(0.f),  _TV(0.f),
         0.25f, _TV(-1.f),  _TV(0.5f),
         0.5f,  _TV(-1.f),  _TV(2.f),
         1.f,   _TV(-0.75f), _TV(1.75f),
    };
    const int handleDataLen = sizeof(handleData) / sizeof(handleData[0]);

    float ret[2];
    spline(handleData, handleDataLen / 3, 2, t, ret);
    return vector3f(ret[0], ret[1], 0);
  }
  static float handleRFunc(float t, float theta) {
    return 1.f;
  }

  static void generateOtherMeshes()
  {
    VBO::generatePave(VBO::doorLock, _TV(5.f), _TV(15.f), _TV(5.f));
    VBO::generatePave(VBO::painting, _TV(80.f), _TV(80.f), _TV(10.f));

    // porte
    Array<vertex> doorMesh(Cube::numberOfVertices);
    Mesh::generatePave(doorMesh, _TV(10.f), _TV(220.f), _TV(90.f));
    Mesh::scaleTexture(doorMesh, 1.f, 2.f);
    VBO::setupData(VBO::door, doorMesh DBGARG("door"));

    // cadre de la porte
    Array<vertex> & doorBorder = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(doorBorder, _TV(0.05f), _TV(0.01f), _TV(0.95f), _TV(0.97f));
    Mesh::translate(doorBorder, _TV(-0.5f), _TV(-0.5f), _TV(-0.5f));
    glLoadIdentity();
    glScalef(_TV(14.f), _TV(230.f), _TV(100.f));
    glRotatef(_TV(90.f), _TV(0.f), _TV(1.f), _TV(0.f));
    Mesh::applyCurrentProjection(doorBorder);
    VBO::setupData(VBO::doorBorder, doorBorder DBGARG("doorBorder"));

    Array<vertex> & doorHinge = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    wheel.generateMesh(doorHinge, _TV(15.f), _TV(0.75f), _TV(5), _TV(6));
    VBO::setupData(VBO::doorHinge, doorHinge DBGARG("doorHinge"));

    // clé de la porte
    Array<vertex> & doorKey = Mesh::getTemp();
    Mesh::Revolution torus(Mesh::torusHFunc, Mesh::torusRFunc);
    torus.generateMesh(doorKey, _TV(3.f), _TV(3.f), _TV(5), _TV(20));
    Mesh::translate(doorKey, _TV(-5.f), _TV(0.f), _TV(0.f));
    Mesh::generatePave(doorKey, _TV(5.f), _TV(0.4f), _TV(0.4f));
    VBO::setupData(VBO::doorKey, doorKey DBGARG("doorKey"));

    // clé de la boite à musique
    Mesh::scale(doorKey, _TV(0.5f), _TV(0.5f), _TV(0.5f));
    VBO::setupData(VBO::mbDoorKey, doorKey DBGARG("doorKey"));

    // poignée
    Mesh::Revolution handle(handleHFunc, handleRFunc);
    Array<vertex> & h = Mesh::getTemp();
    handle.generateMesh(h, _TV(5.f), _TV(1.f), _TV(30), _TV(8));

    VBO::setupData(VBO::doorHandle, h DBGARG("doorHandle"));

    // plinthe
    Array<vertex> & plinthe = Mesh::getTemp();
    Mesh::generateCubicTorus(plinthe, _TV(1.f), _TV(0.995f), _TV(15.f));
    glLoadIdentity();
    glTranslatef(_TV(300.f), _TV(1.f), _TV(175.f));
    glScalef(LONGUEUR_PIECE, 1.f, LARGEUR_PIECE);
    Mesh::applyCurrentProjection(plinthe);
    VBO::setupData(VBO::plinthe, plinthe DBGARG("doorHandle"));

    // poubelle
    Array<vertex> & bin = Mesh::getTemp();
    glLoadIdentity();
    Mesh::generateTorusCylinder(bin, _TV(35.f), _TV(26.f), _TV(0.125f), _TV(16));
    VBO::setupData(VBO::bin, bin DBGARG("bin"));


    // Cadre de la fenêtre
    Array<vertex> & window = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(window, _TV(0.05f), _TV(0.1f), _TV(0.95f), _TV(0.95f));
    glLoadIdentity();
    glTranslatef(_TV(-12.f), _TV(85.f), _TV(232.f));
    glRotatef(_TV(90.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glScalef(_TV(115.f), _TV(130.f), _TV(15.f));
    Mesh::applyCurrentProjection(window);

    // Barre centrale
    //
    // J'ai essayé avec plus de détails (barres horizontales, cadre),
    // mais au final c'est comme ça que ça rend le mieux
    //
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Array<vertex> & bar = Mesh::getTemp();
    cyl.generateMesh(bar, _TV(105.f), _TV(1.f), 1, _TV(-8));
    Mesh::rotate(bar, 180.f / 8.f, 0, 1.f, 0);
    Mesh::expandPave(bar, 0, _TV(5.f), _TV(2.f));

    glLoadIdentity();
    glTranslatef(_TV(-4.f), _TV(98.f), _TV(175.f));
    Mesh::addVerticesUsingCurrentProjection(window, bar);

    VBO::setupData(VBO::window, window DBGARG("window"));
  }

  void generateMeshes()
  {
    Mesh::initPool();

#if SHOW_TESTS
    _generateTestMeshes();
#endif
    _generateRoomMesh();
    Mesh::clearPool();
    generateOtherMeshes();
    Armchair::generateMeshes();
    Chess::generateMeshes();
    Carousel::generateMeshes();
    CatClock::generateMeshes();
    Drum::generateMeshes();
    Harp::generateMeshes();
    MusicBox::generateMeshes();
    Lousianne::generateMeshes();
    Orgalame::generateMeshes();
    Pascontent::generateMeshes();
    Puppet::generateMeshes();
    Xylotrain::generateMeshes();

    Chimney::generateMeshes();
    Stool::generateMesh();
    Desk::generateMeshes();
    Luxo::generateMesh();
    Shelf::generateMeshes();
    BookShelf::generateMeshes();
    Table::generateMeshes();

    Mesh::destroyPool();
  }

  void doorAnimation(const Node & node, date time)
  {
    int doorOpening = _TV(216500);
    float closing = min(smoothStepI(_TV(1500), _TV(2700), time),
                        1.f - smoothStepI(doorOpening, doorOpening + _TV(1000), time));

    float angle = mix(_TV(-25.f), 0, closing);
    glRotatef(angle, 0, 1.f, 0);
  }
  
  void doorHandleAnimation(const Node & node, date time)
  {
    int startTime = _TV(216000);
    float releasing = min(smoothStepI(_TV(2900), _TV(3600), time),
                          1.f - smoothStepI(startTime, startTime + _TV(500), time));
    float angle = mix(_TV(-25.f), 0, releasing);
    glRotatef(angle, 1.f, 0, 0);
  }

  void update(date t)
  {
    static date lastUpdate = 0;
    static int delta = 0;
    const int step = 10; // mise à jour toutes les 10ms

#if DEBUG // retour dans le temps
    if (t == 0) {
      lastUpdate = 0;
      Carousel::update(0);
      delta = 0;
      return;
    }
#endif

    if (lastUpdate == 0)
    {
      lastUpdate = t;
      return;
    }

    delta += t - lastUpdate;
    lastUpdate = t;
    // le moteur physique ne supporte pas les retours dans le passé !
    while (delta > step)
    {
      world.update(step);
      Carousel::update(t);
      delta -= step;
    }
  }

  struct objectDesc
  {
    Node * (*objectCreator)();
    float px;
    float py;
    float pz;
    float ry;
  };

  void createAndPlaceObject(Node * root, const objectDesc & desc)
  {
    glLoadIdentity();
    glTranslatef(desc.px, desc.py, desc.pz);
    glRotatef(desc.ry, 0, 1.f, 0);

    desc.objectCreator()->attachToNode(root);
  }

  Node * create()
  {
    START_TIME_EVAL;

    IFDBG(if (!intro.initDone))
      world.init();

    glPushMatrix();
    glLoadIdentity();

    Node * root = Node::New(START_DATE, END_DATE);
    Renderable room(Shader::parallax, VBO::room);
    room.setTextures(Texture::wall, Texture::wallBump);
    room.setShininess(_TV(8));
    root->attachRenderable(room);

    // arrière-plan noir (pour la cheminée et le tunnel)
    Renderable blackBackground(Shader::parallax, VBO::blackBackground);
    blackBackground.setTextures(Texture::black);
    blackBackground.setShininess(_TV(0));
    root->attachRenderableNode(blackBackground);

    // sol
    Renderable floor(Shader::floor, VBO::floor);
    floor.setTextures(Texture::wood_Noyer, Texture::woodBump1, Texture::felix);
    floor.setShininess(_TV(90));
    root->attachRenderable(floor);

    Renderable plinthe(Shader::parallax, VBO::plinthe);
    root->attachRenderable(plinthe.setTextures(Texture::wood_Lambris, Texture::woodBump1));

    Renderable window(Shader::parallax, VBO::window);
    root->attachRenderable(window.setTextures(Texture::none));

    // plafond
    {
      glLoadIdentity();
      glTranslatef(LONGUEUR_PIECE * 0.5f, HAUTEUR_PLAFOND, LARGEUR_PIECE * 0.5f);
      Renderable ceil(Shader::parallax, VBO::ceil);
      ceil.setTextures(Texture::none, Texture::ceilBump);
      root->attachRenderableNode(ceil);
    }

    // Fenêtre
    {
      glLoadIdentity();
      glTranslatef(_TV(-100.f), _TV(150.f), _TV(175.f));
      glScalef(_TV(1.f), _TV(300.f), _TV(300.f));
      Renderable sky(Shader::lightSource);
      root->attachRenderableNode(sky.setTextures(Texture::sky));
    }

    // Porte
    {
      glLoadIdentity();
      glTranslatef(LONGUEUR_PIECE, _TV(110.f), _TV(150.f));

      Renderable doorBorder(Shader::parallax, VBO::doorBorder);
      root->attachRenderableNode(doorBorder.setTextures(Texture::wood_Lambris, Texture::woodBump1));

      glLoadIdentity();
      glTranslatef(LONGUEUR_PIECE, _TV(110.f), _TV(105.f));
      Node * door = Node::New(worldStartDate, worldEndDate);

      glLoadIdentity();
      glTranslatef(0, 0, _TV(45.f));
      Renderable board(Shader::parallax, VBO::door);
      board.setTextures(Texture::wood_MerisierClair, Texture::doorBump);
      board.setShininess(_TV(40));
      door->attachRenderableNode(board);

      glTranslatef(_TV(-3.5f), _TV(0.f), _TV(40.f));
      Renderable doorLock(Shader::anisotropic, VBO::doorLock);
      doorLock.setTextures(Texture::brass);
      doorLock.setShininess(_TV(40));
      door->attachRenderableNode(doorLock);

      glTranslatef(_TV(-4.f), _TV(-5.f), _TV(0.f));
      Renderable doorKey(Shader::anisotropic, VBO::doorKey);
      doorKey.setTextures(Texture::brass);
      doorKey.setShininess(_TV(40));
      door->attachRenderableNode(doorKey);


      glTranslatef(_TV(3.f), _TV(8.f), _TV(0.f));
      glRotatef(_TV(-90.f), 1.f, 0, 0);
      Node * handle = Node::New(worldStartDate, worldEndDate);

      glLoadIdentity();
      Renderable doorHandle(Shader::anisotropic, VBO::doorHandle);
      doorHandle.setTextures(Texture::brass);
      doorHandle.setShininess(_TV(40));
      handle->attachRenderableNode(doorHandle);
      handle->setAnimation(Anim::doorHandle);
      handle->attachToNode(door);


      glLoadIdentity();
      glTranslatef(_TV(-6.f), _TV(30.f), _TV(0.f));
      Renderable doorHinge(Shader::anisotropic, VBO::doorHinge);
      doorHinge.setTextures(Texture::brass);
      doorHinge.setShininess(_TV(40));
      door->attachRenderableNode(doorHinge);


      door->setAnimation(Anim::door);
      door->attachToNode(root);

      DBG("fin porte");
    }

    // Poubelle
    {
      glLoadIdentity();
      glTranslatef(_TV(342.f), _TV(0.f), _TV(20.f));
      Renderable bin(Shader::parallax, VBO::bin);
      root->attachRenderableNode(bin.setTextures(Texture::none));
    }

    // Tableau
    {
      glLoadIdentity();
      glTranslatef(_TV(260.f), _TV(170.f), _TV(0.f));
      Renderable painting(Shader::parallax, VBO::painting);
      painting.setTextures(Texture::painting, Texture::paintingBump);
      painting.setShininess(_TV(40));
      root->attachRenderableNode(painting);
    }

    // Tableau de Vasarely
    {
      glLoadIdentity();
      glTranslatef(_TV(300.f), _TV(162.f), LARGEUR_PIECE);
      Renderable painting(Shader::parallax, VBO::painting);
      painting.setTextures(Texture::paintingV);
      painting.setShininess(_TV(20));
      root->attachRenderableNode(painting);
    }

#if SHOW_TESTS
    // Objets de test
    {
      glLoadIdentity();
      glTranslatef(420.f, 177.f, 25.f);
      Renderable test(Shader::anisotropic, VBO::test);
      root->attachRenderableNode(test);

      char shininess = 1;
      for (unsigned int i = 0; i < 8; ++i)
      {
	glLoadIdentity();
	glTranslatef(400.f, 143.f, 30.f);

	glTranslatef(6.f * i, 0, 0);

	Renderable biduleMetal(Shader::anisotropic, VBO::test2);
// 	biduleMetal.setTextures();
	biduleMetal.setShininess(shininess);
	root->attachRenderableNode(biduleMetal);

	glTranslatef(0, 0, 12.f);

	Renderable biduleBois(Shader::parallax, VBO::test2);
	biduleBois.setTextures(Texture::wood, Texture::woodBump1);
	biduleBois.setShininess(shininess);
	root->attachRenderableNode(biduleBois);

	shininess *= 2;
      }
    }

    {
      glLoadIdentity();
      glTranslatef(210.f, -120.f, 70.f);
      glScalef(1.f, 1.f, 1.f);
      Renderable hm(Shader::parallax, VBO::testhm);
      hm.setTextures(Texture::wood, Texture::woodBump1);
      root->attachRenderableNode(hm);
    }
#endif // SHOW_TESTS

    const objectDesc objects[] =
      {
	{ Armchair::create,   _TV(30.f),  _TV(35.f) , _TV(280.f), _TV(20.f)  },
	{ Chimney::create,    LONGUEUR_PIECE / 2, 0, LARGEUR_PIECE, 0        },
	{ Table::create,      _TV(180.f), 0,          _TV(275.f), _TV(0.f)   },
	{ Desk::create,       _TV(260.f), 0,          _TV(41.f),  _TV(0.f)   },
	{ Stool::create,      _TV(265.f), 0,          _TV(91.f),  _TV(5.f)   },
	{ BookShelf::create,  _TV(100.f), 0,          _TV(5.f),   _TV(1.f)   },
	{ Shelf::create,      _TV(420.f), 0,          _TV(30.f),  _TV(-2.f)  },
	{ CatClock::create,   _TV(170.f), _TV(170.f), _TV(348.f), _TV(0.f)   },
	{ Xylotrain::create,  _TV(0.f),   _TV(0.f),   _TV(0.f),   _TV(0.f)   },
	{ Luxo::create,       _TV(320.f), _TV(77.f),  _TV(22.f),  _TV(130.f) },
	{ Carousel::create,   _TV(285.f), _TV(85.f),  _TV(335.f),  _TV(0.f)   },
	{ Harp::create,       _TV(202.f), _TV(77.f),  _TV(35.f),  _TV(0.f)   },
	{ Chess::create,      _TV(155.f), _TV(62.f),  _TV(285.f), _TV(285.f)  },
	{ MusicBox::create,   _TV(284.f), _TV(80.f),  _TV(33.f),  _TV(-17.f) },
	{ Lousianne::create,  _TV(150.f), _TV(74.f),  _TV(40.f),  _TV(0.f)   },
	{ Pascontent::create, _TV(50.5f), _TV(70.f),  _TV(35.f),  _TV(0.f)   },
	{ Orgalame::create,   _TV(382.f), _TV(77.f),  _TV(32.f),  _TV(4.f)   },
	{ Puppet::create,     _TV(370.f), _TV(113.f), _TV(35.f),  _TV(0.f)   },
	{ Drum::create,       _TV(397.f), _TV(117.f), _TV(15.f),  _TV(0.f)   },
      };
    const unsigned int numberOfObjects = ARRAY_LEN(objects);

    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      createAndPlaceObject(root, objects[i]);
    }

    glPopMatrix();

    END_TIME_EVAL("Scene setup");
    return root;
  }
}
