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

namespace Chimney
{
  static float topData[] = {
    0, 1, 0,
    0, 1, 20,
    0, 20, 20,
    0, 20, 40,
    0, 50, 45,
    0, 55, 80,
    0, 60, 75,
    0, 65, 80,
    0, 75, 80,
    0, 80, 100,
    0, 90, 100,
    0, 100, 100,
    0, 100, 0,
  };

  static float bibelotData[] = {
    0, 1, 140,
    0, 10, 120,
    0, 15, 80,
    0, 35, 80,
    0, 45, 60,
    0, 60, 60,
    0, 82, 80,
    0, 85, 95,
    0, 90, 80,
    0, 110, 30,
    0, 120, 70,
    0, 130, 105,
    0, 150, 138,
    0, 170, 155,
    0, 190, 160,
    0, 210, 155,
    0, 230, 138,
    0, 250, 105,
    0, 260, 77,
    0, 270, 0,
  };

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    Renderable chimney(Shader::parallax, VBO::chimney);
    root->attachRenderable(chimney.setTextures(Texture::brick));

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(70.f), _TV(0.f));
    Renderable chimneyTop(Shader::parallax, VBO::chimneyTop);
    root->attachRenderableNode(chimneyTop.setTextures(Texture::none));

    Renderable bibelot(Shader::parallax, VBO::bibelot);
    bibelot.setTextures(Texture::wood_MerisierFonce, Texture::woodBump1);
    bibelot.setShininess(_TV(25));
    glLoadIdentity();
    glTranslatef(_TV(-45.f), _TV(85.f), _TV(-12.f));
    root->attachRenderableNode(bibelot);
    glTranslatef(_TV(90.f), _TV(0.f), _TV(0.f));
    root->attachRenderableNode(bibelot);

    Renderable inside(Shader::parallax, VBO::chimneyInside);
    inside.setTextures((Texture::id)(_TV(1)));
    inside.setShininess(_TV(3));
    glLoadIdentity();
    glTranslatef(_TV(-15.f), _TV(16.f), _TV(50.f));
    root->attachRenderableNode(inside);
    root->attachRenderableNode(inside);

    return root;
  }

  static void generateBibelot()
  {
    // bibelot spérique en bois
    Array<vertex> & bibelot = Mesh::getTemp();
    Mesh::Revolution rev(NULL, NULL, bibelotData, ARRAY_LEN(bibelotData));
    rev.generateMesh(bibelot, _TV(10.f), _TV(5.f), _TV(20), _TV(14));
    VBO::setupData(VBO::bibelot, bibelot DBGARG("bibelot"));
  }

  void generateMeshes()
  {
    Mesh::clearPool();
    Array<vertex> & chimney = Mesh::getTemp();
    Array<vertex> & chimneyEntrance = Mesh::getTemp();
    Mesh::generateCustomCubicTorus(chimneyEntrance, _TV(0.1f), _TV(0.2f), _TV(0.9f), _TV(0.9f));

    Array<vertex> & tmp = Mesh::getTemp();
    glLoadIdentity();
    glTranslatef(_TV(-45.f), _TV(0.f), _TV(0.f));
    glScalef(_TV(90.f), _TV(70.f), _TV(10.f));
    Mesh::addVerticesUsingCurrentProjection(chimney, chimneyEntrance);
    Mesh::reprojectTextureXYPlane(chimney, _TV(0.02f));
    Mesh::generatePave(tmp, _TV(100.f), _TV(18.f), _TV(30.f));
    Mesh::scaleTexture(tmp, _TV(1.f), _TV(0.5f));
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(chimney, tmp);
    Mesh::translate(chimney, _TV(0.f), _TV(0.f), _TV(-10.f));
    VBO::setupData(VBO::chimney, chimney DBGARG("chimney"));

    Array<vertex> & chimneyTop = Mesh::getTemp();
    //Mesh::Revolution rev = Mesh::loadSplineFromFile("data/revolution.txt");
    Mesh::Revolution rev(NULL, NULL, topData, ARRAY_LEN(topData));
    rev.generateMesh(chimneyTop, _TV(15.f), _TV(10.f), _TV(28), _TV(-8));
    glRotatef(_TV(22.5f), 0, 1.f, 0);
    Mesh::applyCurrentProjection(chimneyTop);
    Mesh::expandPave(chimneyTop, _TV(50.f), _TV(0.f), _TV(15.f));
    VBO::setupData(VBO::chimneyTop, chimneyTop DBGARG("chimneyTop"));

    generateBibelot();

    Array<vertex> & inside = Mesh::getTemp();
    Mesh::Revolution cyl(Mesh::wheelHFunc, Mesh::wheelRFunc);
    cyl.generateMesh(inside, _TV(35.f), _TV(3.f), _TV(6), _TV(6));
    glLoadIdentity();
    glTranslatef(_TV(30.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(inside, inside);
    glLoadIdentity();
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    glTranslatef(_TV(0.f), _TV(-25.f), _TV(0.f));
    Mesh::applyCurrentProjection(inside);
    VBO::setupData(VBO::chimneyInside, inside DBGARG("chimneyInside"));
  }
}
