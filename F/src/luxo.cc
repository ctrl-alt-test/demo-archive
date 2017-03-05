
#include "luxo.hh"

#include "sys/msys.h"

#include "mesh.hh"
#include "node.hh"
#include "spline.h"
#include "renderable.hh"
#include "revolution.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Luxo
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable luxo(Shader::anisotropic, VBO::luxo);
//     luxo.setTextures(FIXME);
    luxo.setShininess(_TV(18));
    root->attachRenderable(luxo);
    Renderable bulb(Shader::lightSource, VBO::lightBulb);
    bulb.setTextures(Texture::lightBulb);
    root->attachRenderable(bulb);

    return root;
  }

  // Ampoule
  static float bulbData[] = {
    0, 0, 1,
    0, 1, 30,
    0, 10, 75,
    0, 30, 100,
    0, 50, 85,
    0, 65, 60,
    0, 90, 50,
    0, 100, 40
  };
  static const int bulbDataLen = sizeof(bulbData) / sizeof(bulbData[0]);

  // Pied
  static float baseData[] = {
    0, 0, 100,
    0, 25, 95,
    0, 30, 90,
    0, 40, 75,
    0, 60, 50,
    0, 99, 20,
    0, 100, 1,
  };
  static const int baseDataLen = sizeof(baseData) / sizeof(baseData[0]);

  // Abat-jour
  static float coupeData[] = {
    0, 0, 100,
    0, 25, 95,
    0, 45, 75,
    0, 60, 45,
    0, 61, 45,
    0, 94, 36,
    0, 100, 0,
  };
  static const int coupeDataLen = sizeof(coupeData) / sizeof(coupeData[0]);

  vector3f coupeInHFunc(float t, float theta)
  {
    const float s = 1.f - t;
    const float lat = 0.5f * PI * s;
    return vector3f(0.f, msys_sinf(lat), 0.f);
  }
  float coupeInRFunc(float t, float theta)
  {
    const float s = 1.f - t;
    const float lat = 0.5f * PI * s;
    return (0.9f + 0.1f * float(s == 0)) * msys_cosf(lat);
  }


#define DIAMETRE_PIED   20.f
#define HAUTEUR_PIED    4.f

#define LONGUEUR_BRAS   40.f
#define DIAMETRE_BRAS   1.5f

#define DIAMETRE_COUDE  4.f
#define EPAISSEUR_COUDE 2.5f

#define LONGUEUR_JOINT  2.f
#define DIAMETRE_JOINT  0.8f

#define DIAMETRE_COUPE  16.f
#define HAUTEUR_COUPE   10.f

#define ANGLE_PIED      30.f
#define ANGLE_COUDE    -80.f
#define ANGLE_TETE     -10.f

#define DIAMETRE_AMPOULE 5.f
#define LONGUEUR_AMPOULE 7.f

  void generateMesh()
  {
    Mesh::Revolution base(NULL, NULL, baseData, baseDataLen);
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution pipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution coupeOut(NULL, NULL, coupeData, coupeDataLen);
    Mesh::Revolution coupeIn(coupeInHFunc, coupeInRFunc);

//     wheel.setComputeTexCoordFunc(Mesh::radialTexture);
//     wheel.setComputeNormalFunc(Mesh::generalPurposeComputeNormalWithBottoms);

    Mesh::clearPool();
    Array<vertex> & pied = Mesh::getTemp();
    Array<vertex> & coude = Mesh::getTemp();
    Array<vertex> & bras = Mesh::getTemp();
    Array<vertex> & joint = Mesh::getTemp();
    Array<vertex> & coupeOutMesh = Mesh::getTemp();
    Array<vertex> & coupeInMesh = Mesh::getTemp();
    base.generateMesh(pied, HAUTEUR_PIED, 0.5f * DIAMETRE_PIED, _TV(6), _TV(24));
    wheel.generateMesh(coude, EPAISSEUR_COUDE, 0.5f * DIAMETRE_COUDE, _TV(5), _TV(8));
    pipe.generateMesh(bras, LONGUEUR_BRAS, 0.5f * DIAMETRE_BRAS, _TV(1), _TV(6));
    pipe.generateMesh(joint, LONGUEUR_JOINT, 0.5f * DIAMETRE_JOINT, _TV(1), _TV(6));
    coupeOut.generateMesh(coupeOutMesh, 1.6f * HAUTEUR_COUPE, 0.5f * DIAMETRE_COUPE, _TV(12), _TV(16));
    coupeIn.generateMesh(coupeInMesh, 0.6f * HAUTEUR_COUPE, 0.5f * DIAMETRE_COUPE, _TV(6), _TV(16));

    Mesh::rotateTangents(bras);

    Array<vertex> & coude90 = Mesh::getTemp();
    glLoadIdentity();
    glRotatef(90.f, 0, 0, 1.f);
    Mesh::addVerticesUsingCurrentProjection(coude90, coude);

    // Assemblage
    Array<vertex> & luxo = Mesh::getTemp();

    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(luxo, pied);
    glTranslatef(0, HAUTEUR_PIED + 0.5f * DIAMETRE_COUDE, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, coude90);
    glRotatef(ANGLE_PIED, 1.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, bras);
    glTranslatef(0, LONGUEUR_BRAS, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, coude90);
    glRotatef(ANGLE_COUDE, 1.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, bras);
    glTranslatef(0, LONGUEUR_BRAS, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, coude90);
    glRotatef(ANGLE_TETE, 1.f, 0, 0);
    glTranslatef(0, 0.5f * DIAMETRE_COUDE, 0);
    Mesh::addVerticesUsingCurrentProjection(luxo, joint);
    glTranslatef(0, LONGUEUR_JOINT, 0);
    glRotatef(90.f, 1.f, 0, 0);
    glTranslatef(0, -1.1f * HAUTEUR_COUPE, -0.2f * DIAMETRE_COUPE);
    Mesh::addVerticesUsingCurrentProjection(luxo, coupeOutMesh);
    Mesh::addVerticesUsingCurrentProjection(luxo, coupeInMesh);

    VBO::setupData(VBO::luxo, luxo DBGARG("luxo"));

    Mesh::Revolution bulb(NULL, NULL, bulbData, bulbDataLen);
    bulb.setComputeNormalFunc(Mesh::generalPurposeComputeNormalWithBottoms);
    Array<vertex> & bulbMesh = Mesh::getTemp();
    Array<vertex> & pluggedBulb = Mesh::getTemp();
    bulb.generateMesh(bulbMesh, LONGUEUR_AMPOULE, 0.5f * DIAMETRE_AMPOULE, 10, 8);
    Mesh::addVerticesUsingCurrentProjection(pluggedBulb, bulbMesh);

    VBO::setupData(VBO::lightBulb, pluggedBulb DBGARG("lightBulb"));
  }
}
