#include "pascontent.hh"

#include "intro.hh"
#include "interpolation.hh"
#include "revolution.hh"
#include "../snd/sync.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "vbos.hh"

namespace Orgalame
{
  static Array<Sync::Note> notes[128];
  static int minNote = 128;
  static int maxNote = 0;

  static float baseData[] = {
    0, 0, 30,
    0, 10, 30,
    0, 10, 5,
    0, 40, 30,
    0, 105, 40,
    0, 105, 20,
    0, 100, 20,
    0, 100, 0,
  };

  void keyAnimation(const Node & node, date d)
  {
    int keyId = node.visiblePart()[0].id();

    Array<Sync::Note> & arr = notes[keyId];

    int i;
    for (i = 0; i < arr.size; i++) // FIXME: trop lent ? dichotomie ? sauvegarde du dernier i ?
      if (arr[i].time > (int) d)
	break;

    const int last = i == 0 ? 0 : notes[keyId][i - 1].time;
    const int next = i == arr.size ? worldEndDate : notes[keyId][i].time;
    const int volLast = i == 0 ? 0 : notes[keyId][i - 1].vol;
    const int volNext = i == arr.size ? worldEndDate : notes[keyId][i].vol;

    // Durées
    const int open = _TV(100);
    const int close = volNext == 0 ? _TV(300) : _TV(300);

    const float closing = 1.f - smoothStepI(last, last + close, d);
    const float openning = smoothStepI(next - open, next, d);
    float openRatio = max(closing, openning);

    if (volLast > 0) openRatio = 1.f;
    // Notes proches : la clé ne se referme pas
    if (next - last <= close + open)
    {
	openRatio = 1.f;
    }

    glRotatef(mix(_TV(14.f), _TV(6.f), openRatio), 1.f, 0, 0);
  }

  void bladeAnimation(const Node & node, date d)
  {
    int keyId = node.visiblePart()[0].id();

    // FIXME : si on veut le faire plusieurs fois, passer par un tableau
    int t = _TV(138000);
    int delay = (keyId - minNote) * _TV(20);
    int upDuration = _TV(1000);
    int holdDuration = _TV(500);
    int downDuration = _TV(1500);

    float rotation = 0;
    rotation = smoothStepI(t + delay, t + delay + upDuration, d);
    rotation = min(rotation, 1.f - smoothStepI(t + upDuration + holdDuration, t + upDuration + holdDuration + downDuration, d));

    glRotatef(_TV(20.f) * rotation, 0, 0, 1.f);
  }

  static float LONGUEUR_SOCLE = _TV(30.f);
  static float LARGEUR_SOCLE = _TV(12.f);
  static float HAUTEUR_SOCLE = _TV(4.f);

  static float DIAMETRE_AXE = _TV(0.5f);
  static float DISTANCE_BORD_AXE = _TV(1.f);
  static float DISTANCE_BORD2_AXE = _TV(4.f);
  static float HAUTEUR_SUPPORT = _TV(1.5f);
  static float LONGUEUR_SUPPORT = _TV(32.f);
  static float DIAMETRE_TETE_AXE = _TV(0.7f);
  static float EPAISSEUR_TETE_AXE = _TV(0.2f);

  static float DIAMETRE_TROU = _TV(1.f);
  static float HAUTEUR_TROU = _TV(0.2f);
  static float DISTANCE_BORD_TROU = _TV(3.f);

  static float LONGUEUR_LAME = _TV(8.f);
  static float LARGEUR_LAME = _TV(2.f);

  // Création du mesh de la lame
  static void computePositions(Array<vertex> & vertices,
			       float length, float width,
			       int lFaces, int wFaces)
  {
    const float exponent = _TV(5.f);

    for (int j = 0; j <= lFaces; ++j)
      for (int i = 0; i <= wFaces; ++i)
      {
	float alpha = _TV(0.5f) * float(j) / lFaces;

	float x = msys_cosf(alpha) * float(i) / wFaces;
	float y = float(j) / lFaces;
	float z = msys_sinf(alpha) * float(i) / wFaces;

	float w1 = msys_powf(y, exponent);
	float w2 = 1.f - msys_powf(1.f - y, exponent);
	float w = w2 - w1 + 0.1f;

	vector3f p(width * (w1 + w * x), length * y, width * (w1 + w * z));
	vector3f n(-z, 0, x);
	vector3f t(_TV(1.f) * x, _TV(0.f), _TV(1.f) * z);

	const vertex v = {p, n, t,
//			  1.f, 1.f, 1.f,
			  x, y, 0, 1.f};
	vertices.add(v);
      }
  }

  static void makeQuadsFromVertices(Array<vertex> & uniqueVertices,
				    Array<vertex> & vertices,
				    int lFaces, int wFaces)
  {
    // Recto
    for (int j = 0; j < lFaces; ++j)
      for (int i = 0; i < wFaces; ++i)
      {
	vertices.add(uniqueVertices[ i      +  j      * (wFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) +  j      * (wFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) + (j + 1) * (wFaces + 1)]);
	vertices.add(uniqueVertices[ i      + (j + 1) * (wFaces + 1)]);
      }

    // Verso
    for (int i = 0; i < uniqueVertices.size; ++i)
    {
      uniqueVertices[i].n = -uniqueVertices[i].n;
      uniqueVertices[i].t = -uniqueVertices[i].t;
    }
    for (int j = 0; j < lFaces; ++j)
      for (int i = 0; i < wFaces; ++i)
      {
	vertices.add(uniqueVertices[ i      +  j      * (wFaces + 1)]);
	vertices.add(uniqueVertices[ i      + (j + 1) * (wFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) + (j + 1) * (wFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) +  j      * (wFaces + 1)]);
      }
  }

  void generateBlade(Array<vertex> & vertices,
		     float length, float width,
		     int lFaces, int wFaces)
  {
//     Mesh::generatePave(vertices, width, length, 0.1f);
    Array<vertex> uniqueVertices((lFaces + 1) * (wFaces + 1));
    computePositions(uniqueVertices, length, width, lFaces, wFaces);
    makeQuadsFromVertices(uniqueVertices, vertices, lFaces, wFaces);
  }

  void generateMeshes()
  {

    IFDBG(if (!intro.initDone))
      Sync::getSheetForEachNote(Instrument::Orgalame /* fixme */, notes, 0);

    int usedNotes = 0;
    for (int i = 0; i < 128; i++)
      if (notes[i].size != 0)
      {
	++usedNotes;
	minNote = min(minNote, i);
	maxNote = max(maxNote, i);
      }

    Mesh::clearPool();

    // Pied
    Array<vertex> & box = Mesh::getTemp();
    Mesh::Revolution base(NULL, NULL, baseData, ARRAY_LEN(baseData));
    base.generateMesh(box, _TV(4.f), _TV(6.f), _TV(14), _TV(12));
    Mesh::expandPave(box, _TV(15.f), _TV(0.f), _TV(4.f));
    VBO::setupData(VBO::olBase, box DBGARG("orgalame base"));


    // Partie métallique fixe
    Array<vertex> & pipes = Mesh::getTemp();

    // Support
    Array<vertex> & mainPipe = Mesh::getTemp();
    Array<vertex> & supportPipe = Mesh::getTemp();
    Array<vertex> & head = Mesh::getTemp();
    Array<vertex> & hole = Mesh::getTemp();
    Mesh::Revolution pipe(Mesh::pipeHFunc, Mesh::pipeRFunc);
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution torus(Mesh::torusHFunc, Mesh::torusRFunc);

    pipe.generateMesh(mainPipe, LONGUEUR_SUPPORT, 0.5f * DIAMETRE_AXE, 1, 8);
    pipe.generateMesh(supportPipe, HAUTEUR_SUPPORT, 0.5f * DIAMETRE_AXE, 1, 8);
    wheel.generateMesh(head, EPAISSEUR_TETE_AXE, 0.5f * DIAMETRE_TETE_AXE, 5, 8);

    glLoadIdentity();
    wheel.generateMesh(hole, _TV(0.2f), DIAMETRE_TROU * _TV(0.5f), _TV(3), _TV(8));
    Mesh::generateTorusCylinder(hole, HAUTEUR_TROU, DIAMETRE_TROU, 0.2f, 8);

    glTranslatef(-0.5f * LONGUEUR_SOCLE + DISTANCE_BORD_AXE, HAUTEUR_SOCLE, -0.5f * LARGEUR_SOCLE + DISTANCE_BORD2_AXE);
    Mesh::addVerticesUsingCurrentProjection(pipes, supportPipe);
    glTranslatef(LONGUEUR_SOCLE - 2.f * DISTANCE_BORD_AXE, 0, 0);
    Mesh::addVerticesUsingCurrentProjection(pipes, supportPipe);

    glLoadIdentity();
    glTranslatef(0.5f * LONGUEUR_SUPPORT, HAUTEUR_SOCLE + HAUTEUR_SUPPORT, -0.5f * LARGEUR_SOCLE + DISTANCE_BORD2_AXE);
    glRotatef(90.f, 0, 0, 1.f);
    Mesh::addVerticesUsingCurrentProjection(pipes, head);
    Mesh::addVerticesUsingCurrentProjection(pipes, mainPipe);
    glTranslatef(0, LONGUEUR_SUPPORT, 0);
    Mesh::addVerticesUsingCurrentProjection(pipes, head);

    // Trous
    for (int i = 0; i < usedNotes; ++i)
    {
      glLoadIdentity();
      glTranslatef((i+1) * _TV(1.5f) * DIAMETRE_TROU - 0.5f * LONGUEUR_SUPPORT, HAUTEUR_SOCLE, DISTANCE_BORD_TROU);
      Mesh::addVerticesUsingCurrentProjection(pipes, hole);
    }

    VBO::setupData(VBO::olPipes, pipes DBGARG("orgalame pipes"));


    // Clés animées
    Array<vertex> & key = Mesh::getTemp();
    Array<vertex> & tmp = Mesh::getTemp();
    glLoadIdentity();
    Mesh::generatePave(key, _TV(0.125f), _TV(0.125f), _TV(7.f));
    Mesh::translate(key, _TV(0.f), _TV(0.f), _TV(1.5f));

    wheel.generateMesh(tmp, _TV(0.25f), _TV(0.5f), _TV(5), _TV(8));
    Mesh::rotate(tmp, _TV(-17.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::translate(tmp, _TV(0.f), _TV(0.f), _TV(5.1f));
    Mesh::addVerticesUsingCurrentProjection(key, tmp);
    tmp.empty();

    torus.generateMesh(tmp, _TV(1.5f), _TV(0.37f), _TV(16), _TV(-6));
    Mesh::rotate(tmp, _TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::addVerticesUsingCurrentProjection(key, tmp);
    VBO::setupData(VBO::olKey, key DBGARG("olKey"));


    // Lames
    Array<vertex> & blade = Mesh::getTemp();
    generateBlade(blade, LONGUEUR_LAME, LARGEUR_LAME, _TV(10), _TV(4));
    Mesh::rotate(blade, _TV(-90.f), 1.f, 0, 0);
    Mesh::rotate(blade, _TV(10.f), _TV(0.f), _TV(0.f), _TV(1.f));
    VBO::setupData(VBO::olBlade, blade DBGARG("olBlade"));
  }

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    int metalShininess = _TV(100);

    glLoadIdentity();

    Renderable base(Shader::parallax, VBO::olBase);
    base.setShininess(_TV(60));
    root->attachRenderableNode(base.setTextures(Texture::wood_NoyerB, Texture::woodBump2));

    Renderable pipes(Shader::anisotropic, VBO::olPipes);
    pipes.setShininess(metalShininess);
    root->attachRenderableNode(pipes);

    int index = 0; // numéro de la note, entre 0 et usedNotes
    for (int i = minNote; i <= maxNote; ++i)
    {
      if (notes[i].size == 0) continue;
      glLoadIdentity();
      glTranslatef((index+1) * _TV(1.5f) * DIAMETRE_TROU - 0.5f * LONGUEUR_SUPPORT,
		   HAUTEUR_SOCLE + HAUTEUR_SUPPORT,
		   DISTANCE_BORD_TROU + _TV(-5.f));
      Node * keyNode = Node::New(worldStartDate, worldEndDate);
      Renderable key(Shader::anisotropic, VBO::olKey);
      key.setTextures(Texture::gold);
      key.setShininess(metalShininess);
      key.setId(i); // le premier renderable mis dans keyNode doit avoir l'id

      glLoadIdentity();
      Node * bladeNode = Node::New(worldStartDate, worldEndDate);
      Renderable blade(Shader::anisotropic, VBO::olBlade);
      blade.setTextures(Texture::gold, Texture::orgalameBump);
      blade.setShininess(metalShininess);
      blade.setId(i);
      bladeNode->setAnimation(Anim::orgalameBlades);
      bladeNode->attachRenderable(blade);
      bladeNode->attachToNode(keyNode);

      keyNode->setAnimation(Anim::orgalame);
      keyNode->attachRenderable(key);
      keyNode->attachToNode(root);
      index++;
    }

    return root;
  }
};
