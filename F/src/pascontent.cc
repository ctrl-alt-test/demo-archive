#include "pascontent.hh"

#include "intro.hh"
#include "interpolation.hh"
#include "revolution.hh"
#include "../snd/sync.hh"
#include "tweakval.h"
#include "vboid.hh"
#include "tweakval.h"
#include "vbos.hh"

#if DEBUG
#include <stdio.h>
#define EDIT 1
#endif

#if EDIT
# define PIPE1     pipe1Data.elt
# define PIPE1_LEN pipe1Data.size
# define PIPE2     pipe2Data.elt
# define PIPE2_LEN pipe2Data.size
# define PIPE3     pipe3Data.elt
# define PIPE3_LEN pipe3Data.size
#else
# define PIPE1     pipe1Data
# define PIPE1_LEN pipe1DataLen
# define PIPE2     pipe2Data
# define PIPE2_LEN pipe2DataLen
# define PIPE3     pipe3Data
# define PIPE3_LEN pipe3DataLen
#endif

namespace Pascontent
{
#if EDIT

  Array<float> pipe1Data(50);
  Array<float> pipe2Data(50);
  Array<float> pipe3Data(50);
  void loadSplineFromFile(Array<float> & target, const char *file)
  {
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];
    target.empty();
    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      int f[4];
      int result = sscanf(buffer, " %d , %d , %d , %d ,", &f[0], &f[1], &f[2], &f[3]);
      if (result == 4) {
        target.add((float) f[0]);
        target.add((float) f[1]);
        target.add((float) f[2]);
        target.add((float) f[3]);
      }
    }

    // Calcul de t
    for (int i = 0; i < target.size; i += 4)
      target[i] = (float)i / (target.size - 4);
  }

#else // !EDIT

  float pipe1Data[] = {
#include "../data/pascontentPipe1.txt"
  };
  static const int pipe1DataLen = ARRAY_LEN(pipe1Data);

  float pipe2Data[] = {
#include "../data/pascontentPipe2.txt"
  };
  static const int pipe2DataLen = ARRAY_LEN(pipe2Data);

  float pipe3Data[] = {
#include "../data/pascontentPipe3.txt"
  };
  static const int pipe3DataLen = ARRAY_LEN(pipe3Data);

#endif

  static float socleData[] = {
    0, 0, 100,
    0, 45, 100,
    0, 50, 97,
    0, 50, 80,
    0, 53, 75,
    0, 87, 8,
    0, 90, 5,
    0, 97, 5,
    0, 100, 0,
  };


#define LARGEUR_SOCLE         20.f
#define HAUTEUR_SOCLE         3.f

#define DIAMETRE_MACHOIRE     16.f
#define HAUTEUR_MACHOIRE      6.f

#define LONGUEUR_TUBE         20.f
#define DIAMETRE_TUBE         3.f

#define HAUTEUR_TUBES	      5.f
#define ECART_V_TUBES         2.f
#define ECART_H_TUBES         2.f

#define ANGLE_VERTICAL_TUBE   25.f
#define ANGLE_V_EVENTAIL_TUBE 15.f
#define ANGLE_H_EVENTAIL_TUBE 50.f

#define NOMBRE_TUBES          9

  static vector3f pipeLHFunc(float * src, int len,
			     float t, float theta)
  {
    float ret[3];
    spline(src, len / 4, 3, t, ret);
    return vector3f(ret[0] / 4.f, ret[1] / 4.f, ret[2] / 4.f);
  }
  static vector3f pipeRHFunc(float * src, int len,
			     float t, float theta)
  {
    float ret[3];
    spline(src, len / 4, 3, t, ret);
    return vector3f(-ret[0] / 4.f, ret[1] / 4.f, ret[2] / 4.f);
  }

  static vector3f pipe1LHFunc(float t, float theta)
  { return pipeLHFunc(PIPE1, PIPE1_LEN, t, theta); }
  static vector3f pipe1RHFunc(float t, float theta)
  { return pipeRHFunc(PIPE1, PIPE1_LEN, t, theta); }

  static vector3f pipe2LHFunc(float t, float theta)
  { return pipeLHFunc(PIPE2, PIPE2_LEN, t, theta); }
  static vector3f pipe2RHFunc(float t, float theta)
  { return pipeRHFunc(PIPE2, PIPE2_LEN, t, theta); }

  static vector3f pipe3LHFunc(float t, float theta)
  { return pipeLHFunc(PIPE3, PIPE3_LEN, t, theta); }
  static vector3f pipe3RHFunc(float t, float theta)
  { return pipeRHFunc(PIPE3, PIPE3_LEN, t, theta); }

  static float pipeRFunc(float t, float theta) {
    return 1.f;
  }

  void generateMeshes()
  {
    Mesh::clearPool();

    // Socle
    Mesh::Revolution base(NULL, NULL, socleData, ARRAY_LEN(socleData));
    Array<vertex> & baseMesh = Mesh::getTemp();
    base.generateMesh(baseMesh, HAUTEUR_SOCLE, _TV(1.5f), _TV(9), _TV(-8));
    Mesh::rotate(baseMesh, 180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::expandPave(baseMesh, 0.5f * LARGEUR_SOCLE, 0, 0.5f * LARGEUR_SOCLE);
    const float woodTextureScale = _TV(0.05f);
//     Mesh::reprojectTextureXZPlane(baseMesh, woodTextureScale);
    VBO::setupData(VBO::pcBase, baseMesh DBGARG("pascontent base"));

    // Éventail de tubes

#if EDIT
    loadSplineFromFile(pipe1Data, "data/pascontentPipe1.txt");
    loadSplineFromFile(pipe2Data, "data/pascontentPipe2.txt");
    loadSplineFromFile(pipe3Data, "data/pascontentPipe3.txt");
#else
    for (int i = 0; i < pipe1DataLen; i += 4)
      pipe1Data[i] = (float)i / (pipe1DataLen - 4);
    for (int i = 0; i < pipe2DataLen; i += 4)
      pipe2Data[i] = (float)i / (pipe2DataLen - 4);
    for (int i = 0; i < pipe3DataLen; i += 4)
      pipe3Data[i] = (float)i / (pipe3DataLen - 4);
#endif

    Mesh::Revolution pipe1L(pipe1LHFunc, pipeRFunc, NULL, 0);
    Mesh::Revolution pipe1R(pipe1RHFunc, pipeRFunc, NULL, 0);
    Mesh::Revolution pipe2L(pipe2LHFunc, pipeRFunc, NULL, 0);
    Mesh::Revolution pipe2R(pipe2RHFunc, pipeRFunc, NULL, 0);
    Mesh::Revolution pipe3L(pipe3LHFunc, pipeRFunc, NULL, 0);
    Mesh::Revolution pipe3R(pipe3RHFunc, pipeRFunc, NULL, 0);

    Array<vertex> & pipesMesh = Mesh::getTemp();
    Array<vertex> & pipesMesh2 = Mesh::getTemp();

    pipe1L.generateMesh(pipesMesh2, _TV(1.f), _TV(1.7f), _TV(24), _TV(8));
    pipe1R.generateMesh(pipesMesh2, _TV(1.f), _TV(1.7f), _TV(24), _TV(8));
    pipe2L.generateMesh(pipesMesh, _TV(1.f), _TV(1.7f), _TV(42), _TV(6));
    pipe2R.generateMesh(pipesMesh, _TV(1.f), _TV(1.7f), _TV(42), _TV(6));
    pipe3L.generateMesh(pipesMesh, _TV(1.f), _TV(1.7f), _TV(16), _TV(8));
    pipe3R.generateMesh(pipesMesh, _TV(1.f), _TV(1.7f), _TV(16), _TV(8));

    Mesh::translate(pipesMesh, _TV(0.f), _TV(4.5f), _TV(-6.f));
    Mesh::rotate(pipesMesh, _TV(-120.f), 0, 1.f, 0);

    Mesh::translate(pipesMesh2, _TV(0.f), _TV(4.5f), _TV(-6.f));
    Mesh::rotate(pipesMesh2, _TV(-120.f), 0, 1.f, 0);

    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Array<vertex> & plaque = Mesh::getTemp();
    wheel.generateMesh(plaque, _TV(1.f), _TV(1.f), _TV(5), _TV(-8));
    Mesh::rotate(plaque, 180.f / _TV(8.f), 0, 1.f, 0);
    Mesh::expandPave(plaque, _TV(8.2f), _TV(0.f), _TV(8.2f));
    Mesh::translate(plaque, 0, _TV(3.5f), 0);

    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(pipesMesh, plaque);

    VBO::setupData(VBO::pcPipes, pipesMesh DBGARG("pascontent pipes"));
    VBO::setupData(VBO::pcPipes2, pipesMesh2 DBGARG("pascontent pipes2"));


	Array<vertex> & slider = Mesh::getTemp();

	wheel.generateMesh(slider, _TV(0.6f), _TV(0.6f), _TV(5), _TV(12));
    Mesh::rotate(slider, 180.f / _TV(12.f), 0, 1.f, 0);
    Mesh::expandPave(slider, _TV(0.6f), _TV(0.f), _TV(1.f));
    Mesh::rotate(slider, 90.f, _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::translate(slider, _TV(-22.f), _TV(2.8f), _TV(10.7f));

	VBO::setupData(VBO::pcSlider, slider DBGARG("pascontent slider"));
  }

  void bodyAnimation(const Node &node, date d)
  {
	  int slamDate = _TV(100225);

	  int liftDuration = _TV(2000);
	  int dropDuration = _TV(20);

	  const int liftDate = slamDate - liftDuration - dropDuration;
	  const int dropDate = slamDate - dropDuration;

	  if ((int)d > liftDate && (int)d < slamDate)
	  {
		  float lift = clamp(((float)d - (float)liftDate) / (float)liftDuration, 0, 1.f);
		  float drop = 1.f - clamp(((float)d - (float)dropDate) / (float)dropDuration, 0, 1.f);
		  float h = min(lift, drop);
		  glTranslatef(0, _TV(1.f) * h, 0);
	  }
  }

  void sliderAnimation(const Node &node, date d)
  {
    const Instrument::Id instr = Instrument::Angry;
    const Array<Sync::Note> & arr = Sync::sheet[instr];
    int idx = Sync::getLastNote(instr, d);
    idx = max(0, idx);
    if (idx == arr.size - 1) return; // fin de la partition

    int next = arr[idx + 1].time; // date de la prochaine note
    float progress = smoothStepI(next - _TV(150), next, d);
    float pos = mix((float)arr[idx].note, (float)arr[idx+1].note, progress);
    glTranslatef(_TV(0.5f) * pos, _TV(0.f), _TV(0.f));
  }

  void pipeAnimation(const Node &node, date d)
  {
    int startDate = _TV(91500);
    int endDate = _TV(92300);
    float progress = smoothStepI(startDate, startDate + _TV(1000), d);
    progress = min(progress, 1.f - smoothStepI(endDate - _TV(100), endDate, d));
    glTranslatef(progress * _TV(0.f), progress * _TV(0.f), _TV(0.f));
    glRotatef(progress * _TV(0.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glTranslatef(progress * _TV(-1.5f), progress * _TV(1.5f), _TV(0.f));
  }

  Node * create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);

    glLoadIdentity();

    Renderable base(Shader::parallax, VBO::pcBase);
    base.setShininess(_TV(100));
    base.setTextures(Texture::wood_NoyerB, Texture::woodBump2);
    root->attachRenderableNode(base);

    Node * bodyNode = Node::New(worldStartDate, worldEndDate);
    Renderable pipes(Shader::anisotropic, VBO::pcPipes);
    pipes.setShininess(_TV(42));
    pipes.setTextures(Texture::aluminium);
    bodyNode->setAnimation(Anim::pcBody);
    bodyNode->attachRenderable(pipes);
    bodyNode->attachToNode(root);

    { // pipes animés
    Node * animPipe = Node::New(worldStartDate, worldEndDate);
    Renderable pipes2(Shader::anisotropic, VBO::pcPipes2);
    pipes2.setShininess(_TV(42));
    pipes2.setTextures(Texture::aluminium);
    animPipe->setAnimation(Anim::pcPipe);
    animPipe->attachRenderable(pipes2);
    animPipe->attachToNode(bodyNode);
    }

    Node * sliderNode = Node::New(worldStartDate, worldEndDate);
    Renderable slider(Shader::anisotropic, VBO::pcSlider);
    slider.setShininess(_TV(100));
    slider.setTextures(Texture::aluminium);
    sliderNode->setAnimation(Anim::pcSlider);
    sliderNode->attachRenderableNode(slider);
    sliderNode->attachToNode(root);

    return root;
  }
}
