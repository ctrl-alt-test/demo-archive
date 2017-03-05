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

namespace Table
{
  static float tableData[] =
  {
    0, 0, 0,
    0, 1, 19,
    0, 9, 19,
    0, 25, 25,
    0, 82, 15,
    0, 95, 23,
    0, 96, 20,
    0, 102, 20,
    0, 107, 28,
    0, 120, 28,
    0, 122, 165,
    0, 130, 170,
    0, 130, 173,
    0, 145, 174,
    0, 152, 168,
    0, 156, 0,
  };

  // bougeoir
  static float holderData[] = {
    0, 0, 100,
    0, 5, 150,
    0, 20, 200,
    0, 25, 150,
    0, 10, 140,
    0, 2, 50,
    0, 20, 20,
    0, 40, 30,
    0, 50, 20,
    0, 60, 50,
    0, 80, 60,
    0, 100, 30,
    0, 150, 50,
    0, 155, 90,
    0, 165, 0,
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

    // Table
    Renderable table(Shader::parallax, VBO::table);
    table.setTextures(Texture::wood_MerisierClair);
    table.setShininess(_TV(60));
    root->attachRenderable(table);

    glLoadIdentity();
    glTranslatef(_TV(20.f), _TV(62.f), _TV(10.f));
    Renderable candleHolder(Shader::anisotropic, VBO::candleHolder);
    candleHolder.setTextures(Texture::brass);
    candleHolder.setShininess(_TV(40));
    root->attachRenderableNode(candleHolder);

    glTranslatef(_TV(0.f), _TV(14.f), _TV(0.f));
    Renderable candle(Shader::parallax, VBO::candle);
    candle.setTextures(Texture::candle);
    candle.setShininess(_TV(30));
    root->attachRenderableNode(candle);

    // tasse
    glLoadIdentity();
    glTranslatef(_TV(-26.f), _TV(62.f), _TV(-12.f));
    Renderable cup(Shader::parallax, VBO::cup);
    cup.setShininess(_TV(150));
    root->attachRenderableNode(cup.setTextures(Texture::none));

    return root;
  }

  // Anse du mug
  static vector3f handleHFunc(float t, float theta) {
    float handleData[] =
    {     // t,        z,          y
         _TV(0.f),   _TV(0.f),    _TV(0.0f),
         _TV(0.25f), _TV(-0.4f),  _TV(0.27f),
         _TV(0.5f),  _TV(-0.5f),  _TV(0.65f),
         _TV(0.75f), _TV(-0.35f), _TV(0.92f),
         _TV(1.f),   _TV(0.f),    _TV(1.f),
    };

    float ret[2];
    spline(handleData, ARRAY_LEN(handleData) / 3, 2, t, ret);
    return vector3f(0, ret[1], ret[0]);
  }
  static float handleRFunc(float t, float theta) {
    return 1.f; // FIXME : applatir un peu
  }

  // Tore pour le mug
  static vector3f cupTorusHFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    return vector3f(0.f, _TV(1.f) * msys_sinf(lat), 0.f);
  }
  static float cupTorusRFunc(float t, float theta)
  {
    const float lat = 2.f * PI * (t - 0.5f);
    return _TV(0.84f) + _TV(0.08f) * msys_cosf(lat);
  }

  void generateMeshes()
  {
    Mesh::clearPool();

    // table
    Array<vertex> & table = Mesh::getTemp();
    Mesh::Revolution tableR(NULL, NULL, tableData, ARRAY_LEN(tableData));
    tableR.generateMesh(table, _TV(40.f), _TV(30.f), _TV(25), _TV(40));
    // TODO: les pieds
    VBO::setupData(VBO::table, table DBGARG("table"));

    Array<vertex> & candleHolder = Mesh::getTemp();
    Mesh::Revolution holder(NULL, NULL, holderData, ARRAY_LEN(holderData));
    holder.generateMesh(candleHolder, _TV(8.f), _TV(5.f), _TV(30), _TV(18));
    VBO::setupData(VBO::candleHolder, candleHolder DBGARG("candleHolder"));

    Array<vertex> & candle = Mesh::getTemp();
    Mesh::Revolution candleR(Mesh::wheelHFunc, Mesh::wheelRFunc);
    candleR.generateMesh(candle, _TV(6.f), _TV(2.f), _TV(8), _TV(8));
    VBO::setupData(VBO::candle, candle DBGARG("candle"));

    // tasse
    Array<vertex> & cupMesh = Mesh::getTemp();

    Mesh::Revolution cupBody(cupTorusHFunc, cupTorusRFunc);
    cupBody.generateMesh(cupMesh, _TV(0.5f), _TV(3.5f), _TV(16), _TV(20));
    Mesh::expandPave(cupMesh, 0, _TV(4.f), 0);
    Mesh::translate(cupMesh, 0, _TV(4.f), 0);

    Array<vertex> & handleMesh = Mesh::getTemp();
    Mesh::Revolution handle(handleHFunc, handleRFunc);
    handle.generateMesh(handleMesh, _TV(5.f), _TV(0.5f), _TV(30), _TV(8));
    glLoadIdentity();
    glTranslatef(0.f, _TV(1.5f), _TV(-3.f));
    Mesh::addVerticesUsingCurrentProjection(cupMesh, handleMesh);

    VBO::setupData(VBO::cup, cupMesh DBGARG("cup"));
  }
}
