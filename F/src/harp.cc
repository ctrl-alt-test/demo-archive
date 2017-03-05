#include "harp.hh"

#include "sys/msys.h"

#include "interpolation.hh"
#include "intro.hh"
#include "renderable.hh"
#include "revolution.hh"
#include "shaderid.hh"
#include "shaders.hh"
#include "../snd/sync.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Harp
{
  static Array<Sync::Note> notes[128];
  static int minNote = 128;
  static int maxNote = 0;

  static float bodyData[] = {
    0, 0, 0,
    0, 5, 45,
    0, 25, 85,
    0, 55, 100,
    0, 85, 90,
    0, 100, 80,
  };
  static const int bodyDataLen = sizeof(bodyData) / sizeof(bodyData[0]);

  static float neckData[] = {
    0, 0, 100,
    0, 30, 60,
    0, 70, 45,
    0, 100, 60,
  };
  static const int neckDataLen = sizeof(neckData) / sizeof(neckData[0]);

  static float plotData[] = {
    0, 0, 100,
    0, 80, 70,
    0, 80, 30,
    0, 0, 0,
  };
  static const int plotDataLen = sizeof(plotData) / sizeof(plotData[0]);

  static vector3f couronneHFunc(float t, float theta) { return vector3f(0, 0, 0); }
  static float couronneRFunc(float t, float theta) { return 0.1f + 0.9f * t; }

#define HAUTEUR_CORPS_CYCLOGRATTE  12.f
#define DIAMETRE_CORPS_CYCLOGRATTE 20.f

#define BASE_CORDE                 6.f
#define LONGUEUR_CORDE             26.f
#define EPAISSEUR_CORDE            0.05f

#define DIAMETRE_PLOT              0.3f
#define HAUTEUR_PLOT               0.4f

#define LONGUEUR_MANCHE            25.f
#define DIAMETRE_MANCHE            4.f
#define DIAMETRE_POMME             4.f
#define EPAISSEUR_POMME            1.5f

#define LONGUEUR_POIGNEE           6.f
#define DIAMETRE_POIGNEE           2.f
#define DIAMETRE_TETE              2.f
#define EPAISSEUR_TETE             1.f

#define LONGUEUR_AXE               3.f
#define DIAMETRE_AXE               0.5f

#define DIAMETRE_CLEF              1.5f
#define EPAISSEUR_CLEF             0.6f

  void generateMeshes()
  {
    IFDBG(if (!intro.initDone))
    Sync::getSheetForEachNote(Instrument::Harp, notes, 65);

    for (int i = 0; i < 128; i++)
      if (notes[i].size != 0)
      {
	minNote = min(minNote, i);
	maxNote = max(maxNote, i);
      }


    Mesh::clearPool();

    // Corps
    Mesh::Revolution body(NULL, NULL, bodyData, bodyDataLen);
    Mesh::Revolution flat(couronneHFunc, couronneRFunc);
    Mesh::Revolution neck(NULL, NULL, neckData, neckDataLen);
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution pipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution plot(NULL, NULL, plotData, plotDataLen);

    Array<vertex> & cycloMesh = Mesh::getTemp();
    Array<vertex> & bodyMesh = Mesh::getTemp();
    Array<vertex> & flatMesh = Mesh::getTemp();
    Array<vertex> & neckMesh = Mesh::getTemp();
    Array<vertex> & baseMesh = Mesh::getTemp();
    Array<vertex> & handMesh = Mesh::getTemp();
    Array<vertex> & endMesh = Mesh::getTemp();
    Array<vertex> & pipeMesh = Mesh::getTemp();
    Array<vertex> & keyMesh = Mesh::getTemp();
    Array<vertex> & plotMesh = Mesh::getTemp();

    body.generateMesh(bodyMesh, HAUTEUR_CORPS_CYCLOGRATTE, 0.5f * DIAMETRE_CORPS_CYCLOGRATTE, 20, 32);
    flat.generateMesh(flatMesh, 1.f, 0.5f * 0.8f * DIAMETRE_CORPS_CYCLOGRATTE, 1, 32);
    neck.generateMesh(neckMesh, LONGUEUR_MANCHE, 0.5f * DIAMETRE_MANCHE, 10, 12);
    wheel.generateMesh(baseMesh, EPAISSEUR_POMME, 0.5f * DIAMETRE_POMME, 7, 12);
    neck.generateMesh(handMesh, LONGUEUR_POIGNEE, 0.5f * DIAMETRE_POIGNEE, 4, 8);
    wheel.generateMesh(endMesh, EPAISSEUR_TETE, 0.5f * DIAMETRE_TETE, 7, 12);
    pipe.generateMesh(pipeMesh, LONGUEUR_AXE, 0.5f * DIAMETRE_AXE, 1, 6);
    wheel.generateMesh(keyMesh, EPAISSEUR_CLEF, 0.5f * DIAMETRE_CLEF, 5, 8);
    plot.generateMesh(plotMesh, HAUTEUR_PLOT, 0.5f * DIAMETRE_PLOT, 3, 6);

    const float textureScale = _TV(0.05f);
    Mesh::reprojectTextureXYPlane(bodyMesh, textureScale);
    Mesh::reprojectTextureXZPlane(flatMesh, textureScale);
    Mesh::reprojectTextureXZPlane(baseMesh, textureScale);
    Mesh::reprojectTextureXZPlane(endMesh,  textureScale);
    Mesh::reprojectTextureXZPlane(keyMesh,  textureScale);
    Mesh::reprojectTextureXYPlane(neckMesh, textureScale);
    Mesh::reprojectTextureXYPlane(handMesh, textureScale);

    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, bodyMesh);
    glTranslatef(0, HAUTEUR_CORPS_CYCLOGRATTE, 0);
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, neckMesh);
    glRotatef(180.f, 1.f, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, flatMesh);
    glRotatef(180.f, 1.f, 0, 0);
    glTranslatef(0, LONGUEUR_MANCHE + 0.5f * EPAISSEUR_POMME, 0);
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, baseMesh);
    glTranslatef(0, 0.5f * EPAISSEUR_POMME, 0);
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, handMesh);

    for (unsigned int i = 0; i < 3; ++i)
    {
      glPushMatrix();
      glRotatef(float(i) * 15.f, 0, 1.f, 0);
      glTranslatef(0.5 * LONGUEUR_AXE, LONGUEUR_POIGNEE * (float(i) + 1.f) / 4.f, 0);
      glRotatef(90.f, 0, 0, 1.f);
      Mesh::addVerticesUsingCurrentProjection(cycloMesh, pipeMesh);
      glRotatef(90.f, 1.f, 0, 0);
      glRotatef(40.f * msys_sfrand(), 0, 0, 1.f);
      Mesh::addVerticesUsingCurrentProjection(cycloMesh, keyMesh);
      glTranslatef(0, 0, -LONGUEUR_AXE);
      glRotatef(40.f * msys_sfrand(), 0, 0, 1.f);
      Mesh::addVerticesUsingCurrentProjection(cycloMesh, keyMesh);
      glPopMatrix();
    }

    glTranslatef(0, LONGUEUR_POIGNEE + 0.5f * EPAISSEUR_TETE, 0);
    Mesh::addVerticesUsingCurrentProjection(cycloMesh, endMesh);

    for (int i = minNote; i <= maxNote; ++i)
    {
      if (_TV(1))
        if (notes[i].size == 0) continue;
      glLoadIdentity();
      glRotatef(360.f * float(i - minNote) / float(maxNote - minNote), 0, 1.f, 0);
      glTranslatef(BASE_CORDE, HAUTEUR_CORPS_CYCLOGRATTE, 0);

      Mesh::addVerticesUsingCurrentProjection(cycloMesh, plotMesh);
    }

    VBO::setupData(VBO::cyclogratte, cycloMesh DBGARG("cyclogratte"));


    // Cordes
    Mesh::Revolution rope(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::clearPool();
    Array<vertex> & ropeMesh = Mesh::getTemp();
    rope.generateMesh(ropeMesh, LONGUEUR_CORDE, EPAISSEUR_CORDE, 2, 3);

    VBO::setupData(VBO::rope, ropeMesh DBGARG("harp"));
  }

  static void setUniforms(Shader::id sid, int objid)
  {
    Array<Sync::Note> & arr = notes[objid];
    int time = (int) intro.now.youtube;

    int i;
    for (i = 0; i < arr.size; i++) // FIXME: trop lent ? dichotomie ? sauvegarde du dernier i ?
      if (arr[i].time > (int) time)
	break;

    float col = 0.f;
    if (i > 0)
    {
      const Sync::Note & n = arr[i-1];
      const int noteDuration = 5000; // en ms
      col = 1.f - smoothStepI(n.time, n.time + noteDuration, time);
    }
    Shader::setUniform1f(sid, Shader::trans, col);
  }

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    unsigned char shininess = _TV(20);

    glLoadIdentity();

    Renderable body(Shader::parallax, VBO::cyclogratte);
    body.setTextures(Texture::wood_Lambris, Texture::woodBump1);
    body.setShininess(shininess);
    root->attachRenderableNode(body);

    for (int i = minNote; i <= maxNote; ++i)
    {
      if (_TV(1))
        if (notes[i].size == 0) continue;
      glLoadIdentity();
      glRotatef(_TV(360.f) * float(i - minNote) / float(maxNote - minNote), 0, 1.f, 0);
      glTranslatef(BASE_CORDE, HAUTEUR_CORPS_CYCLOGRATTE, 0);
      glRotatef(_TV(10.5f), 0, 0, 1.f);

      Renderable rope(Shader::string, VBO::rope);
      rope.setId(i);
      rope.setCustomSetUniform(setUniforms);
      root->attachRenderableNode(rope);
    }

    return root;
  }
}
