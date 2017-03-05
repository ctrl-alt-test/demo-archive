
#include "stool.hh"

#include "sys/msys.h"

#include "mesh.hh"
#include "node.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "timing.hh"
#include "textureId.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Stool
{
  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable stool(Shader::parallax, VBO::stool);
    stool.setTextures(Texture::wood_Pauvre, Texture::woodBump1);
    stool.setShininess(_TV(10));
    root->attachRenderable(stool);

    return root;
  }

#define DIAMETRE_ASSISE 30.f
#define EPAISSEUR_ASSISE 5.f

#define DIAMETRE_SOCLE 24.f
#define EPAISSEUR_SOCLE 5.f

#define LONGUEUR_VIS 30.f
#define DIAMETRE_VIS 5.f

#define LONGUEUR_PIED 40.f
#define DIAMETRE_PIED 3.f

#define LONGUEUR_TRAVERSE 20.f
#define DIAMETRE_TRAVERSE 2.f
#define HAUTEUR_TRAVERSE 20.f

  void generateMesh()
  {
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution pipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution hemisphere(Mesh::hemisphereHFunc, Mesh::hemisphereRFunc);
    wheel.setComputeNormalFunc(Mesh::generalPurposeComputeNormalWithBottoms);

    Mesh::clearPool();
    Array<vertex> & assise   = Mesh::getTemp();
    Array<vertex> & socle    = Mesh::getTemp();
    Array<vertex> & vis      = Mesh::getTemp();
    Array<vertex> & pied     = Mesh::getTemp();
    Array<vertex> & traverse = Mesh::getTemp();
    Array<vertex> & boutVis  = Mesh::getTemp();
    Array<vertex> & boutPied = Mesh::getTemp();

    wheel.generateMesh(assise, EPAISSEUR_ASSISE, 0.5f * DIAMETRE_ASSISE, 7, 18);
    wheel.generateMesh(socle, EPAISSEUR_SOCLE, 0.5f * DIAMETRE_SOCLE, 7, 12);
    pipe.generateMesh(vis, LONGUEUR_VIS, 0.5f * DIAMETRE_VIS, 1, 8);
    pipe.generateMesh(pied, LONGUEUR_PIED, 0.5f * DIAMETRE_PIED, 1, 6);
    pipe.generateMesh(traverse, LONGUEUR_TRAVERSE, 0.5f * DIAMETRE_TRAVERSE, 1, 6);
    hemisphere.generateMesh(boutVis, 0.25f * DIAMETRE_VIS, 0.5f * DIAMETRE_VIS, 2, 8);
    hemisphere.generateMesh(boutPied, 0.25f * DIAMETRE_PIED, 0.5f * DIAMETRE_PIED, 2, 6);

    const float textureScale = _TV(0.025f);
    Mesh::reprojectTextureXZPlane(assise,   textureScale);
    Mesh::reprojectTextureXZPlane(socle,    textureScale);
    Mesh::reprojectTextureXYPlane(vis,      textureScale);
    Mesh::reprojectTextureXYPlane(pied,     textureScale);
    Mesh::reprojectTextureXYPlane(traverse, textureScale);
    Mesh::reprojectTextureXYPlane(boutVis,  textureScale);
    Mesh::reprojectTextureXYPlane(boutPied, textureScale);


    // Assemblage
    Array<vertex> & stool = Mesh::getTemp();

    glLoadIdentity();
    glTranslatef(0, 20.f, 0);
    glRotatef(180.f, 0, 0, 1.f);
    Mesh::addVerticesUsingCurrentProjection(stool, boutVis);
    glRotatef(180.f, 0, 0, 1.f);
    Mesh::addVerticesUsingCurrentProjection(stool, vis);
    glTranslatef(0, 20.f, 0);
    Mesh::addVerticesUsingCurrentProjection(stool, socle);
    glTranslatef(0, 10.f, 0);
    Mesh::addVerticesUsingCurrentProjection(stool, assise);

    for (unsigned int i = 0; i < 4; ++i)
    {
      glLoadIdentity();
      glRotatef(i * 90.f, 0, 1.f, 0);
      glTranslatef(20.f, 0, 0);
      glRotatef(15.f, 0, 0, 1.f);

      glTranslatef(0, 0.25f * DIAMETRE_PIED, 0);
      glRotatef(180.f, 0, 0, 1.f);
      Mesh::addVerticesUsingCurrentProjection(stool, boutPied);
      glRotatef(180.f, 0, 0, 1.f);
      Mesh::addVerticesUsingCurrentProjection(stool, pied);

      glLoadIdentity();
      glRotatef(i * 90.f, 0, 1.f, 0);
      glTranslatef(15.f, HAUTEUR_TRAVERSE, 0);
      glRotatef(90.f, 1.f, 0, 1.f);

      Mesh::addVerticesUsingCurrentProjection(stool, traverse);
    }

    VBO::setupData(VBO::stool, stool DBGARG("stool"));
  }
}
