
#include "xylotrain.hh"

#include "array.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "node.hh"
#include "revolution.hh"
#include "shaders.hh"
#include "spline.h"
#include "../snd/sync.hh"
#include "textureid.hh"
#include "textures.hh"
#include "timing.hh"
#include "tweakval.h"
#include "vbos.hh"

#if DEBUG
#include <stdio.h>
#define EDIT 1
#endif

#if EDIT
# define SPLINE splineData.elt
# define SPLINE_LEN splineData.size
#else
# define SPLINE railData
# define SPLINE_LEN railDataLen
#endif

namespace Xylotrain
{
#if EDIT
  Array<float> splineData(1000);
  void loadSplineFromFile(const char *file)
  {
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];
    splineData.empty();
    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      int f[4];
      int result = sscanf(buffer, " %d , %d , %d , %d ,", &f[0], &f[1], &f[2], &f[3]);
      if (result == 4) {
        splineData.add((float) f[0]);
        splineData.add((float) f[1]);
        splineData.add((float) f[2]);
        splineData.add((float) f[3]);
      }
    }

    // Calcul de t
    for (int i = 0; i < splineData.size; i += 4)
      splineData[i] = (float)i / (splineData.size - 4);
  }

#else // !EDIT

  // Rails
  float railData[] = {
#include "../data/rail.txt"
  };
  static const int railDataLen = sizeof(railData) / sizeof(railData[0]);

#endif
  int smoothSplinePoints;
  float smoothSpline[10000];


  static float railSection[] = {
    0, 40,

    55, 60,
    65, 100,
    90, 95,
    115, 100,
    125, 60,

    180, 40,

    235, 60,
    245, 100,
    270, 95,
    295, 100,
    305, 60,

    360, 40,
  };

  static vector3f railHFunc(float t, float theta) {
    float ret[3];
    spline(SPLINE, SPLINE_LEN / 4, 3, t, ret);
    return vector3f(ret[0] * _TV(1.f), ret[1] * _TV(1.f), ret[2] * _TV(1.f));
  }
  static float railRFunc(float t, float theta)
  {
    float ret;
    spline(railSection, ARRAY_LEN(railSection) / 2, 1, 180.f/PI * theta, &ret);
    return ret / 100.f;
  }

  float coneRFunc(float t, float theta)
  {
    if (t >= _TV(1.f)) return 0.001f;
    return _TV(1.f) * t + _TV(0.001f);
  }

  static void generateTrain()
  {
    const float textureScale = _TV(0.05f);
    const float largeurWagon = _TV(4.f);
    Array<vertex> & tmp = Mesh::getTemp();
    Mesh::Revolution wheel(Mesh::wheelHFunc, Mesh::wheelRFunc);
    Mesh::Revolution cone(Mesh::pipeHFunc, coneRFunc);

    // roues
    Array<vertex> & wheels = Mesh::getTemp();
    wheel.generateMesh(tmp, _TV(0.5f), _TV(1.5f), _TV(5), _TV(12));
    Mesh::reprojectTextureXZPlane(tmp, textureScale);
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(wheels, tmp);
    glTranslatef(_TV(0.f), largeurWagon, _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(wheels, tmp);

    tmp.empty();
    Mesh::Revolution cyl(Mesh::pipeHFunc, Mesh::pipeRFunc);
    cyl.generateMesh(tmp, largeurWagon, _TV(0.25f), _TV(2), _TV(5));
    glLoadIdentity();
    Mesh::addVerticesUsingCurrentProjection(wheels, tmp);

    glRotatef(_TV(90.f), _TV(0.f), _TV(0.f), _TV(1.f));
    Mesh::applyCurrentProjection(wheels);
    VBO::setupData(VBO::wagonWheels, wheels DBGARG("wagonWheels"));

    Array<vertex> & socle = Mesh::getTemp();
    Array<vertex> & wagon = Mesh::getTemp();
    Array<vertex> & locomotive = Mesh::getTemp();

    // socle
    wheel.generateMesh(socle, _TV(1.f), _TV(0.5f), _TV(5), _TV(10));
    Mesh::expandPave(socle, _TV(1.25f), _TV(0.f), _TV(3.f));
    Mesh::reprojectTextureXZPlane(socle, textureScale);

    // wagon 1
    glLoadIdentity();
    tmp.empty();
    Mesh::addVerticesUsingCurrentProjection(wagon, socle);
    Mesh::generateCubicTorus(tmp, _TV(1.f), _TV(0.9f), _TV(2.f));
    glTranslatef(_TV(0.f), _TV(1.f), _TV(0.f));
    glScalef(_TV(2.5f), _TV(1.f), _TV(5.f));
    Mesh::addVerticesUsingCurrentProjection(wagon, tmp);
    VBO::setupData(VBO::wagon, wagon DBGARG("wagon"));

    tmp.empty();
    Mesh::generatePave(tmp, _TV(4.f), _TV(0.25f), _TV(0.5f));
    Mesh::translate(tmp, _TV(0.f), _TV(2.f), _TV(0.f));
    VBO::setupData(VBO::wagonMetal, tmp DBGARG("wagonMetal"));

    // locomotive
    glLoadIdentity();
    tmp.empty();
    Mesh::addVerticesUsingCurrentProjection(locomotive, socle);
    wheel.generateMesh(tmp, _TV(4.f), _TV(1.5f), _TV(5), _TV(14));
    Mesh::reprojectTextureXZPlane(tmp, textureScale);
    glTranslatef(_TV(0.f), _TV(1.5f), _TV(-1.5f));
    glRotatef(_TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(locomotive, tmp);

    glLoadIdentity();
    tmp.empty();
    cone.generateMesh(tmp, _TV(1.5f), _TV(1.f), _TV(5), _TV(14));
    Mesh::translate(tmp, _TV(0.f), _TV(2.5f), _TV(-0.5f));
    VBO::setupData(VBO::locoMetal, tmp DBGARG("locoMetal"));

    glLoadIdentity();
    tmp.empty();
    glTranslatef(_TV(0.f), _TV(2.f), _TV(2.f));
    Mesh::generatePave(tmp, _TV(3.f), _TV(4.f), _TV(3.f));
    Mesh::addVerticesUsingCurrentProjection(locomotive, tmp);

    VBO::setupData(VBO::locomotive, locomotive DBGARG("locomotive"));
  }

  static vector3f mkVector(float *f)
  {
    return vector3f(f[0], f[1], f[2]);
  }

  // Calcule la distance entre les deux dates, sur une spline de dimension 3
  static float splineLength(float *data, int size, float date1, float date2)
  {
    int steps = _TV(200);
    float length = 0.f;
    for (int i = 1; i <= steps; i++)
    {
      float pos1[3], pos2[3];
      float t1 = mix(date1, date2, (float) (i-1) / steps);
      float t2 = mix(date1, date2, (float) i / steps);
      spline(data, size / 4, 3, t1, pos1);
      spline(data, size / 4, 3, t2, pos2);
      float x = pos1[0] - pos2[0];
      float y = pos1[1] - pos2[1];
      float z = pos1[2] - pos2[2];
      length += msys_sqrtf(x * x + y * y + z * z);
    }
    return length;
  }

  static void generateSmoothSpline()
  {
    float speed = _TV(28.75f);
    smoothSplinePoints = _TV(150);
    int idx = 0;
    smoothSpline[idx++] = 0.f;
    smoothSpline[idx++] = SPLINE[1];
    smoothSpline[idx++] = SPLINE[2];
    smoothSpline[idx++] = SPLINE[3];
    float totalDistance = 0.f;
    for (int i = 1; i < smoothSplinePoints; i++) {
      float oldTime = (float) (i-1) / (smoothSplinePoints - 1);
      float time = (float) i / (smoothSplinePoints - 1);
      float pos[3];
      spline(SPLINE, SPLINE_LEN, 3, time, pos);
      totalDistance += splineLength(SPLINE, SPLINE_LEN, oldTime, time);
      smoothSpline[idx++] = totalDistance * speed;
      smoothSpline[idx++] = pos[0];
      smoothSpline[idx++] = pos[1];
      smoothSpline[idx++] = pos[2];
    }
  }

  void generateMeshes()
  {
    generateTrain();

#if EDIT
    loadSplineFromFile("data/rail.txt");
#else
    for (int i = 0; i < railDataLen; i += 4)
      railData[i] = (float)i / (railDataLen - 4);
#endif
    generateSmoothSpline();

    Mesh::Revolution rail(railHFunc, railRFunc, NULL, 0, true);
    Mesh::clearPool();
    Array<vertex> & railMesh = Mesh::getTemp();
    rail.generateMesh(railMesh, _TV(1.f), _TV(3.f), _TV(128), _TV(20));
    Mesh::scaleTexture(railMesh, _TV(10.f), _TV(3.f));
    Mesh::rotate(railMesh, -90.f, 0, 0, 1.f);

    VBO::setupData(VBO::rail, railMesh DBGARG("rail"));

    // tunnel
    Array<vertex> & tunnel = Mesh::getTemp();
    Mesh::generateTorusCylinder(tunnel, _TV(4.f), _TV(15.f), _TV(0.4f), _TV(20));
    Mesh::rotate(tunnel, _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    Mesh::rotate(tunnel, _TV(0.f), _TV(0.f), _TV(1.f), _TV(0.f));
    glLoadIdentity();
    glTranslatef(_TV(-25.f), _TV(66.f), _TV(0.f));
    Mesh::addVerticesUsingCurrentProjection(tunnel, tunnel);
    VBO::setupData(VBO::tunnel, tunnel DBGARG("tunnel"));

    VBO::generatePave(VBO::wagonAttach, _TV(0.25f), _TV(0.25f), _TV(8.f));
  }

#define INSTRUMENT Instrument::Xylo

  static void setUniforms(Shader::id sid, int objid)
  {
    Array<Sync::Note> & arr = Sync::sheet[INSTRUMENT];
    int time = (int) intro.now.youtube;

    const Sync::Note & n = arr[objid / 1000];

    float col =
      min(smoothStepI(n.time - _TV(50), n.time + _TV(0), time),
          1.f - smoothStepI(n.time + _TV(50), n.time + _TV(100), time));
    Shader::setUniform1f(sid, Shader::trans, col);
  }

  // fonction pour pointer vers un point
  // FIXME : ranger quelque part
  //
  static void doRotation(float* pos, float* old)
  {
    const float difx = old[0] - pos[0];
    const float dify = old[1] - pos[1];
    const float difz = old[2] - pos[2];

    glRotatef(RAD_TO_DEG * msys_atan2f(difx, difz), 0.f, 1.f, 0.f);
    glRotatef(RAD_TO_DEG * -msys_sinf(dify), 1.f, 0.f, 0.f);
  }

  void trainAnimation(const Node &node, date d)
  {
    // Le rail a été écrit vers le haut puis pivoté de 90° autour de Z
    // du coup x = pos[1] et y = -pos[0]

    int diff = 30; // ms d'intervalle pour trouver l'orientation
    if ((int) d < diff) return;
    float old[3];
    spline(smoothSpline, smoothSplinePoints, 3, (float) d - diff, old);
    float tmp = old[0]; old[0] = old[1]; old[1] = -tmp;

    float pos[3];
    spline(smoothSpline, smoothSplinePoints, 3, (float) d, pos);
    tmp = pos[0]; pos[0] = pos[1]; pos[1] = -tmp;

    glTranslatef(pos[0], pos[1], pos[2]);
    doRotation(pos, old);
    glTranslatef(0.f, 2.f, 0.f);
  }

  void wheelsAnimation(const Node & node, date d)
  {
    glRotatef((float)(d * 360 / 200.f), -1.f, 0.f, 0.f);
  }

  static void createWheels(Node* root)
  {
    Renderable wheels(Shader::parallax, VBO::wagonWheels);
    Node* wheelsNode = Node::New(root->birthDate(), root->deathDate());
    wheelsNode->attachRenderable(wheels.setTextures(Texture::wood_Noyer, Texture::woodBump1));
    wheelsNode->setAnimation(Anim::wheelsRotation);
    wheelsNode->attachToNode(root);
  }

  void createWagon(Node* root, int id, VBO::id kind)
  {
    glLoadIdentity();
    glTranslatef(_TV(2.f), _TV(0.5f), _TV(3.f));
    createWheels(root);

    glLoadIdentity();
    glTranslatef(_TV(2.f), _TV(0.5f), _TV(-1.5f));
    createWheels(root);

    glLoadIdentity();
    glTranslatef(_TV(0.f), _TV(0.5f), _TV(0.f));
    Renderable wagon(Shader::parallax, kind);
    root->attachRenderableNode(wagon.setTextures(Texture::wood, Texture::woodBump1));
  }

  Node* create()
  {
    Node * root = Node::New(worldStartDate, worldEndDate);
    Renderable rail(Shader::parallax, VBO::rail);
    rail.setShininess(_TV(60));
    root->attachRenderable(rail.setTextures(Texture::wood_TendreFrais, Texture::railBump));

    Array<Sync::Note> & notes = Sync::sheet[INSTRUMENT];
    int lastTime = 0;
    for (int i = 0; i < notes.size; i++)
    {
      glLoadIdentity();
      int timeMin = _TV(180); // temps minimum entre deux wagons
      if (i > 0 && notes[i].time < lastTime + timeMin) continue;
      int nextWagon = i + 1; // index du wagon suivant
      lastTime = notes[i].time;
      const int begin = max(0, notes[i].time - _TV(15830));
      const int length = _TV(19000);
      Node * train = Node::New(begin, begin + length);
      train->setAnimation(Anim::train);
      train->setAnimationStartDate(begin);
      train->setAnimationStopDate(begin + length);
      // type : locomotive ou wagon
      bool isWagon = i > 0 && notes[i].time - notes[i-1].time < _TV(450);
      createWagon(train, i, isWagon ? VBO::wagon : VBO::locomotive);

      {
        Renderable metal(Shader::xylophone, isWagon ? VBO::wagonMetal : VBO::locoMetal);
	metal.setShininess(_TV(60));
        metal.setTextures(Texture::silver);
        metal.setId(i * 1000 + notes[i].note);
        metal.setCustomSetUniform(setUniforms);
        glTranslatef(_TV(0.f), _TV(0.f), _TV(-1.f));
        // double barre
        if (i + 1 < notes.size && notes[i+1].time <  notes[i].time + timeMin) {
          nextWagon++;
          train->attachRenderableNode(metal);
          glTranslatef(_TV(0.f), _TV(0.f), _TV(3.f));
          metal.setId((i+1) * 1000 + notes[i+1].note);
          train->attachRenderableNode(metal);
        }
        else // simple barre
          train->attachRenderableNode(metal);
      }
      train->attachToNode(root);

      if (nextWagon < notes.size && notes[nextWagon].time < notes[i].time + _TV(300))
      {
        glLoadIdentity();
        glTranslatef(_TV(0.f), _TV(0.25f), _TV(0.f));
        Node * attachNode = Node::New(begin, begin + length);
        attachNode->setAnimation(Anim::train);
        attachNode->setAnimationStartDate(begin + _TV(120));
        attachNode->setAnimationStopDate(begin + length);
        Renderable attach(Shader::parallax, VBO::wagonAttach);
        attach.setShininess(_TV(20));
        attach.setTextures(Texture::black);
        attachNode->attachRenderableNode(attach);
        attachNode->attachToNode(root);
      }
    }

    glLoadIdentity();
    glTranslatef(_TV(410.f), _TV(82.75f), _TV(-4.f));
    Renderable tunnel(Shader::parallax, VBO::tunnel);
    root->attachRenderableNode(tunnel.setTextures(Texture::wood_Lambris, Texture::woodBump1));

    return root;
  }
}
