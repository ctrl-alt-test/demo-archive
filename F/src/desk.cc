
#include "desk.hh"

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

namespace Desk
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable desk(Shader::parallax, VBO::desk);
    desk.setTextures(Texture::wood_TendreSec, Texture::woodBump3);
    desk.setShininess(_TV(15));
    root->attachRenderable(desk);


    // Bloc notes et crayon
    glLoadIdentity();
    Node * notepad = Node::New(worldStartDate, worldEndDate);
    Renderable paper(Shader::parallax, VBO::paper);
    paper.setShininess(_TV(10));
    notepad->attachRenderable(paper.setTextures(Texture::paper));

    Renderable pen(Shader::parallax, VBO::pen);
    pen.setShininess(_TV(40));
    notepad->attachRenderable(pen.setTextures(Texture::pen, Texture::penBump));

    notepad->attachToNode(root);


    // Outils
    Renderable toolsWoodPart(Shader::parallax, VBO::toolsWoodPart);
    toolsWoodPart.setTextures(Texture::wood_Pauvre, Texture::woodBump1);
    toolsWoodPart.setShininess(_TV(20));
    root->attachRenderable(toolsWoodPart);

    Renderable toolsMetalPart(Shader::parallax, VBO::toolsMetalPart);
    toolsMetalPart.setTextures(Texture::iron);
    toolsMetalPart.setShininess(_TV(16));
    root->attachRenderable(toolsMetalPart);

    return root;
  }

#define LONGUEUR 150.f
#define LARGEUR 80.f
#define HAUTEUR 75.f
#define EPAISSEUR 4.f

#define ECART_TRAITEAU 50.f
#define HAUTEUR_TRAVERSE 20.f
#define ECART_PIED 60.f

#define LONGUEUR_PIED 75.f
#define LARGEUR_PIED 5.f
#define EPAISSEUR_PIED 2.5f

#define LONGUEUR_TRAVERSE 60.f
#define LARGEUR_TRAVERSE 3.f
#define EPAISSEUR_TRAVERSE 2.f

  static vector3f penHFunc(float t, float theta)
  {
    float x = (t < 0.5f ? _TV(0.01f) * t : _TV(0.6f) + _TV(0.4f) * t);
    return vector3f(0, x, 0);
  }
  static float penRFunc(float t, float theta)
  {
    return _TV(1.f) * float(t > 0 && t < 1.f) + _TV(0.0001f);
  }

  static float handleData[] = {
      0,   0,   0,
      0,   1,  28,
      0,  10,  81,
      0,  37, 100,
      0,  68,  57,
      0,  86,  74,
      0,  96,  67,
      0, 100,   0,
  };

  static void generateLimes(Array<vertex> & woodPart,
			    Array<vertex> & metalPart,
			    Array<vertex> & temp,
			    Array<vertex> & temp2)
  {
    const float woodTextureScale = _TV(0.05f);
    const float ironTextureScale = _TV(0.1f);

    // Manche
    temp2.empty();
    Mesh::Revolution handle(NULL, NULL, handleData, ARRAY_LEN(handleData));
    handle.generateMesh(temp2, _TV(9.f), _TV(1.5f), _TV(14), _TV(8));
    Mesh::reprojectTextureZYPlane(temp2, woodTextureScale);

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);

    // Lime presque plate
    temp.empty();
    wheel.generateMesh(temp, _TV(0.8f), _TV(0.25f), _TV(8), _TV(8));
    Mesh::expandPave(temp, _TV(0.4f), _TV(7.f), _TV(0.f));
    Mesh::reprojectTextureZYPlane(temp, ironTextureScale);
    Mesh::translate(temp, 0, _TV(16.f), 0);

    glLoadIdentity();
    glTranslatef(_TV(63.f), _TV(78.5f), _TV(30.f));
    glRotatef(_TV(25.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glRotatef(_TV(-92.5f), _TV(1.f), _TV(0.f), _TV(0.f));

    Mesh::addVerticesUsingCurrentProjection(woodPart, temp2);
    Mesh::addVerticesUsingCurrentProjection(metalPart, temp);

    // Lime quatre pans
    temp.empty();
    wheel.generateMesh(temp, _TV(0.4f), _TV(0.4f), _TV(8), _TV(-4));
    Mesh::expandPave(temp, 0, _TV(7.f), 0);
    Mesh::reprojectTextureZYPlane(temp, ironTextureScale);
    Mesh::translate(temp, 0, _TV(16.f), 0);

    glLoadIdentity();
    glTranslatef(_TV(67.f), _TV(78.5f), _TV(21.f));
    glRotatef(_TV(35.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glRotatef(_TV(-92.5f), _TV(1.f), _TV(0.f), _TV(0.f));

    Mesh::addVerticesUsingCurrentProjection(woodPart, temp2);
    Mesh::addVerticesUsingCurrentProjection(metalPart, temp);
  }

  static void generateMaillet(Array<vertex> & woodPart,
			      Array<vertex> & metalPart,
			      Array<vertex> & temp,
			      Array<vertex> & temp2)
  {
    const float textureScale = _TV(0.1f);

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);

    // Manche
    temp.empty();
    wheel.generateMesh(temp, _TV(2.f), _TV(1.f), _TV(8), _TV(10));
    Mesh::expandPave(temp, _TV(0.f), _TV(10.f), _TV(0.3f));
    Mesh::reprojectTextureZYPlane(temp, textureScale);
    Mesh::translate(temp, 0, _TV(8.6f), 0);

    // Tête
    temp2.empty();
    wheel.generateMesh(temp2, _TV(1.5f), _TV(2.f), _TV(8), _TV(12));
    Mesh::expandPave(temp2, 0, _TV(3.f), 0);
    Mesh::reprojectTextureXYPlane(temp2, textureScale);
    Mesh::rotate(temp2, 90.f, _TV(1.f), _TV(0.f), _TV(0.f));

    glLoadIdentity();
    glTranslatef(_TV(-15.f), _TV(79.f), _TV(-25.f));
    glRotatef(_TV(93.f), _TV(0.f), _TV(0.f), _TV(1.f));
    glRotatef(_TV(25.f), _TV(1.f), _TV(0.f), _TV(0.f));

    Mesh::addVerticesUsingCurrentProjection(woodPart, temp);
    Mesh::addVerticesUsingCurrentProjection(woodPart, temp2);
  }

  float bistable(float t)
  {
    return clamp(4.f * (0.5f - msys_fabsf(t - 0.5f)) - 1.f, 0.f, 1.f);
  }

  vector3f nutHFunc(float t, float theta)
  {
    float h = 3.f * t;
    if (h > 2.f) h = 3.f - h;
    if (h > 1.f) h = 1.f;
    return vector3f(0, h, 0);
  }
  float nutRFunc(float t, float theta)
  {
    float d = clamp(3.f * (1.f - t) - 1.f, 0.f, 1.f);
    return _TV(0.4f) + _TV(0.6f) * d;
  }

  static void generateEcrous(Array<vertex> & woodPart,
			     Array<vertex> & metalPart,
			     Array<vertex> & temp,
			     Array<vertex> & temp2)
  {

    temp.empty();
    Mesh::Revolution nut(nutHFunc, nutRFunc);
    nut.generateMesh(temp, _TV(0.4f), _TV(0.8f), _TV(12), _TV(-6));
    Mesh::translate(temp, 0, _TV(0.f), 0);
    Mesh::expandPave(temp, _TV(0.f), _TV(0.f), _TV(0.f));

    msys_srand(_TV(1));
    for (int i = 0; i < _TV(10); ++i)
    {
      const float x = _TV(8.f) + _TV(16.f) * msys_frand();
      const float y = _TV(77.f);
      const float z = _TV(-25.f) + _TV(12.f) * msys_frand();
      glLoadIdentity();
      glTranslatef(x, y, z);
      glRotatef(_TV(60.f) * msys_frand(), 0, 1.f, 0);
      Mesh::addVerticesUsingCurrentProjection(metalPart, temp);
    }
  }

  void generateMeshes()
  {
    Mesh::clearPool();

    Array<vertex> & toolsWoodPart = Mesh::getTemp();
    Array<vertex> & toolsMetalPart = Mesh::getTemp();
    Array<vertex> & temp = Mesh::getTemp();
    Array<vertex> & temp2 = Mesh::getTemp();

    generateLimes(toolsWoodPart, toolsMetalPart, temp, temp2);
    generateMaillet(toolsWoodPart, toolsMetalPart, temp, temp2);
    generateEcrous(toolsWoodPart, toolsMetalPart, temp, temp2);

    VBO::setupData(VBO::toolsWoodPart, toolsWoodPart DBGARG("toolsWoodPart"));
    VBO::setupData(VBO::toolsMetalPart, toolsMetalPart DBGARG("toolsMetalPart"));


    // Crayon
    Mesh::Revolution pen(penHFunc, penRFunc);
    Array<vertex> & penMesh = Mesh::getTemp();
    pen.generateMesh(penMesh, _TV(16.f), _TV(0.45f), _TV(3), _TV(-6));

    glLoadIdentity();
    glTranslatef(_TV(-13.f), _TV(78.5f), _TV(-2.f));
    glRotatef(_TV(90.f), 1.f, 0, 0);
    glRotatef(_TV(-85.f), 0, 0, 1.f);
    Mesh::applyCurrentProjection(penMesh);
    VBO::setupData(VBO::pen, penMesh DBGARG("pen"));

    Array<vertex> & paper = Mesh::getTemp();
    Mesh::generatePave(paper, _TV(21.f), _TV(1.f), _TV(29.7f));
    glLoadIdentity();
    glTranslatef(_TV(-5.f), _TV(77.6f), _TV(3.f));
    glRotatef(_TV(12.f), 0, 1.f, 0);
    Mesh::applyCurrentProjection(paper);
    VBO::setupData(VBO::paper, paper DBGARG("paper"));

    // Création des pièces de base
    Array<vertex> & plateau = Mesh::getTemp();
    Array<vertex> & pied = Mesh::getTemp();
    Array<vertex> & traverse = Mesh::getTemp();

    Mesh::generatePave(plateau, LARGEUR, EPAISSEUR, LONGUEUR);
    Mesh::generatePave(pied, EPAISSEUR_PIED, LONGUEUR_PIED, LARGEUR_PIED);
    Mesh::generatePave(traverse, EPAISSEUR_TRAVERSE, LARGEUR_TRAVERSE, LONGUEUR_TRAVERSE);
    Mesh::splitAllFaces(plateau, 4); // Hack pour contourner le bug du motion blur

    // Création d'un traiteau
    Array<vertex> & traiteau = Mesh::getTemp();

    for (unsigned int j = 0; j < 2; ++j)
    {
      glLoadIdentity();
      glTranslatef(18.f * (j == 0 ? 1.f : -1.f), 0, 0);
      glRotatef(12.f * (j == 0 ? 1.f : -1.f), 0, 0, 1.f);

      glTranslatef(0, HAUTEUR_TRAVERSE, 0);
      Mesh::addVerticesUsingCurrentProjection(traiteau, traverse);

      glTranslatef(0, 17.f, -(0.5f * ECART_PIED));
      Mesh::addVerticesUsingCurrentProjection(traiteau, pied);
      glTranslatef(0, 0, ECART_PIED);
      Mesh::addVerticesUsingCurrentProjection(traiteau, pied);
    }

    // Assemblage du bureau
    Array<vertex> & desk = Mesh::getTemp();

    glLoadIdentity();
    glTranslatef(-ECART_TRAITEAU, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(desk, traiteau);

    glLoadIdentity();
    glTranslatef(ECART_TRAITEAU, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(desk, traiteau);

    glLoadIdentity();
    glTranslatef(0, HAUTEUR, 0);
    glRotatef(90.f, 0, 1.f, 0); // Pour avoir la texture dans le sens de la longueur
    Mesh::addVerticesUsingCurrentProjection(desk, plateau);

    VBO::setupData(VBO::desk, desk DBGARG("desk"));
  }
}
