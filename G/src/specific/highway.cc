
#include "array.hxx"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/building.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "node.hxx"
#include "randomness.hh"
#include "renderable.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

#if DEBUG
#include <stdio.h>
#define EDIT 1
#endif

namespace HighWay
{
  Array<float> highwaySlice;
  Array<float> highway1Dir;
  Array<float> highway2Dir;
  Array<float> highway3Dir;
  Array<float> highway4Dir;
  Array<float> highway5Dir;
  Array<float> highway6Dir;

#if EDIT
  void load2DSplineFromFile(const char *file, Array<float> & splineData)
  {
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];
    splineData.empty();
    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      float f[2];
      int result = sscanf(buffer, " %ff , %ff ,", &f[0], &f[1]);
      if (result == 2) {
        splineData.add(0.f);
        splineData.add(f[0]);
        splineData.add(f[1]);
      }
    }

    // Calcul de t
    for (int i = 0; i < splineData.size; i += 3)
      splineData[i] = (float)i / (splineData.size - 3);
  }

  void load3DSplineFromFile(const char *file, Array<float> & splineData)
  {
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];
    splineData.empty();
    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      float f[3];
      int result = sscanf(buffer, " %ff , %ff , %ff ,", &f[0], &f[1], &f[2]);
      if (result == 3) {
        splineData.add(0.f);
        splineData.add(f[0]);
        splineData.add(f[1]);
        splineData.add(f[2]);
      }
    }

    // Calcul de t
    for (int i = 0; i < splineData.size; i += 4)
      splineData[i] = (float)i / (splineData.size - 4);
  }

#else // EDIT

  void loadSpline(float * data, int size, int dimension, Array<float> & dest)
  {
    int rows = size / dimension;
    for (int j = 0; j < rows; ++j)
    {
      dest.add(float(j) / (rows - 1));

      for (int i = 0; i < dimension; ++i)
      {
	dest.add(data[j * dimension + i]);
      }
    }
  }

  float highwaySliceData[] = {
#include "../data/highwaySlice.txt"
  };
  float highway1Data[] = {
#include "../data/highway1.txt"
  };
  float highway2Data[] = {
#include "../data/highway2.txt"
  };
  float highway3Data[] = {
#include "../data/highway3.txt"
  };
  float highway4Data[] = {
#include "../data/highway4.txt"
  };
  float highway5Data[] = {
#include "../data/highway5.txt"
  };
  float highway6Data[] = {
#include "../data/highway6.txt"
  };

#endif // EDIT

  static vector3f hFunc(float t, float theta, const Array<float> & data) {
    float ret[3];
    spline(data, 3, t, ret);
    return vector3f(ret[0] * _TV(1.f), ret[1] * _TV(1.f), ret[2] * _TV(1.f));
  }

  static vector3f highway1HFunc(float t, float theta) { return hFunc(t, theta, highway1Dir); }
  static vector3f highway2HFunc(float t, float theta) { return hFunc(t, theta, highway2Dir); }
  static vector3f highway3HFunc(float t, float theta) { return hFunc(t, theta, highway3Dir); }
  static vector3f highway4HFunc(float t, float theta) { return hFunc(t, theta, highway4Dir); }
  static vector3f highway5HFunc(float t, float theta) { return hFunc(t, theta, highway5Dir); }
  static vector3f highway6HFunc(float t, float theta) { return hFunc(t, theta, highway6Dir); }

  static vector2f highwayConstSectionFunc(float t, float thetaLevel)
  {
    vector2f ret;
    spline(highwaySlice, 2, thetaLevel, (float*)&ret);
    return ret;
  }

  static vector2f highwayVaryingSectionFunc(float t, float thetaLevel)
  {
    vector2f ret;
    spline(highwaySlice, 2, thetaLevel, (float*)&ret);

    // Elargissement dans la courbe
    const float scaleOpen = smoothStep(0.4f, 0.6f, t);
    const float scaleClose = 1.f - smoothStep(0.6f, 0.8f, t);
    ret.x *= 1.f + 0.8f * scaleOpen * scaleClose;

    return ret;
  }

  void generateLampPost(Mesh::MeshStruct & mesh, Mesh::MeshStruct & temp, bool dualHead)
  {
    const float towerHeight = _TV(6.f);
    const float towerDiameter = _TV(0.3f);
    const float armAngle = _TV(20.f);
    const float armLength = _TV(1.8f);
    const float armDiameter = _TV(0.1f);
    const float lampLength = _TV(1.2f);
    const float lampWidth = _TV(0.3f);

    // Le pied
    temp.clear();
    Mesh::Pave(towerDiameter, towerHeight, towerDiameter).generateMesh(temp);
    for (int i = 0; i < temp.vertices.size; ++i)
      if (temp.vertices[i].p.y > 0.f)
      {
	temp.vertices[i].p.x *= _TV(0.5f);
	temp.vertices[i].p.z *= _TV(0.5f);
      }
    temp.translate(0, 0.5f * towerHeight, 0);
    temp.computeNormals();
    temp.computeBaryCoord();
    mesh.add(temp);


    Mesh::Pave lightBox(lampLength, lampWidth, lampWidth);

    // La face lumineuse
    temp.clear();
    lightBox.generateMesh(temp, Mesh::pave_bottom);
    temp.setColor(Material::cityLightColor * _TV(2.f)); // Soyons fous

    // Puis le capot
    lightBox.generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_all & ~Mesh::pave_bottom));
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      if (temp.vertices[i].p.y > 0.f)
      {
	if (temp.vertices[i].p.x > 0.f)
	  temp.vertices[i].p.x *= _TV(0.5f);
	temp.vertices[i].p.z *= _TV(0.5f);
      }
      else if (temp.vertices[i].p.x < 0.f)
      {
	temp.vertices[i].p.x -= lampWidth * msys_sinf(DEG_TO_RAD * armAngle);
      }
    }


    // Enfin le bras
    temp.translate(0.5f * (lampLength + armLength), 0, 0);
    Mesh::Pave(armLength, armDiameter, armDiameter).generateMesh(temp);

    temp.translate(0.5f * armLength, 0, 0);
    temp.computeNormals();
    temp.computeBaryCoord();

    matrix4 transform = matrix4::translation(vector3f(0, towerHeight, 0))
      .rotate(DEG_TO_RAD * armAngle, vector3f::uz);
    mesh.add(temp, transform);

    if (dualHead)
    {
      transform = matrix4::translation(vector3f(0, towerHeight, 0))
	.rotate(DEG_TO_RAD * 180.f, vector3f::uy)
	.rotate(DEG_TO_RAD * armAngle, vector3f::uz);
      mesh.add(temp, transform);
    }
  }

  void generatePillar(Mesh::MeshStruct & mesh, Mesh::MeshStruct & temp)
  {
    const float height = _TV(5.0f);
    const float minWidth = _TV(2.0f);
    const float maxWidth = _TV(3.0f);
    const float minThickness = _TV(0.7f);
    const float maxThickness = _TV(2.0f);
    const float length = _TV(9.0f);

    // Thales
    const float max = maxWidth - minWidth;
    const float med = max * (length - maxWidth) / length;
    const float zJoinRatio = (med + minWidth) / maxWidth;
    const float zEndRatio = minWidth / maxWidth;

    mesh.clear();

    // Trunk
    temp.clear();

    Mesh::Pave halfTrunk(maxWidth, height, maxWidth);
    halfTrunk.generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_left | Mesh::pave_front | Mesh::pave_back));
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      vector3f & p = temp.vertices[i].p;
      if (p.x > 0) p.x = 0.f;
      else
      {
	if (p.y > 0) p.y -= maxThickness;
	p.z *= zJoinRatio;
      }
    }
    temp.translate(0, 0.5f * height, 0);
    mesh.add(temp);

    temp.clear();
    halfTrunk.generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_right | Mesh::pave_front | Mesh::pave_back));
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      vector3f & p = temp.vertices[i].p;
      if (p.x < 0) p.x = 0.f;
      else
      {
	if (p.y > 0) p.y -= maxThickness;
	p.z *= zJoinRatio;
      }
    }
    temp.translate(0, 0.5f * height, 0);
    mesh.add(temp);

    // Branch
    temp.clear();

    Mesh::Pave branch(length, maxThickness, maxWidth);
    branch.generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_bottom | Mesh::pave_left | Mesh::pave_front | Mesh::pave_back));
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      vector3f & p = temp.vertices[i].p;
      if (p.x > 0)
      {
	if (p.y > 0) p.x = 0.f;
	else
	{
	  p.x = -0.5f * maxWidth;
	  p.z *= zJoinRatio;
	}
      }
      else
      {
	if (p.y < 0) p.y += maxThickness - minThickness;
	p.z *= zEndRatio;
      }
    }
    temp.translate(0.f, height - 0.5f * maxThickness, 0);
    mesh.add(temp);

    temp.clear();
    branch.generateMesh(temp, (Mesh::PaveFaces)(Mesh::pave_bottom | Mesh::pave_right | Mesh::pave_front | Mesh::pave_back));
    for (int i = 0; i < temp.vertices.size; ++i)
    {
      vector3f & p = temp.vertices[i].p;
      if (p.x < 0)
      {
	if (p.y > 0) p.x = 0.f;
	else
	{
	  p.x = 0.5f * maxWidth;
	  p.z *= zJoinRatio;
	}
      }
      else
      {
	if (p.y < 0) p.y += maxThickness - minThickness;
	p.z *= zEndRatio;
      }
    }
    temp.translate(0.f, height - 0.5f * maxThickness, 0);
    mesh.add(temp);

    mesh.computeNormals();
  }

  void init()
  {
    highwaySlice.init(100);
    highway1Dir.init(100);
    highway2Dir.init(100);
    highway3Dir.init(100);
    highway4Dir.init(100);
    highway5Dir.init(100);
    highway6Dir.init(100);
  }

  void setIdForTiming(Mesh::MeshStruct & mesh, int vertices,
		      int firstStartDate, int globalDuration, int localDuration)
  {
    const int numberOfGroups = mesh.vertices.size / vertices;
    for (int i = 0; i < mesh.vertices.size; ++i)
    {
      int group = i / vertices;
      int date = firstStartDate + (globalDuration * group) / numberOfGroups;
      float magicId = float(date) + float(localDuration) / 10000.f;
      mesh.vertices[i].id = magicId;
    }
  }

  void generateMeshes()
  {
    IFDBG(if (!intro.initDone))
      init();

#if EDIT
    load2DSplineFromFile("data/highwaySlice.txt", highwaySlice);
    load3DSplineFromFile("data/highway1.txt",     highway1Dir);
    load3DSplineFromFile("data/highway2.txt",     highway2Dir);
    load3DSplineFromFile("data/highway3.txt",     highway3Dir);
    load3DSplineFromFile("data/highway4.txt",     highway4Dir);
    load3DSplineFromFile("data/highway5.txt",     highway5Dir);
    load3DSplineFromFile("data/highway6.txt",     highway6Dir);
#else
    loadSpline(highwaySliceData, ARRAY_LEN(highwaySliceData), 2, highwaySlice);
    loadSpline(highway1Data,     ARRAY_LEN(highway1Data),     3, highway1Dir);
    loadSpline(highway2Data,     ARRAY_LEN(highway2Data),     3, highway2Dir);
    loadSpline(highway3Data,     ARRAY_LEN(highway3Data),     3, highway3Dir);
    loadSpline(highway4Data,     ARRAY_LEN(highway4Data),     3, highway4Dir);
    loadSpline(highway5Data,     ARRAY_LEN(highway5Data),     3, highway5Dir);
    loadSpline(highway6Data,     ARRAY_LEN(highway6Data),     3, highway6Dir);
#endif

    Mesh::clearPool();

    Mesh::MeshStruct & highwaysMesh = Mesh::getTempMesh();
    Mesh::MeshStruct & temp = Mesh::getTempMesh();

    temp.clear();
    Mesh::Revolution(highway1HFunc, NULL, highwayConstSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(223000), _TV(6000), _TV(2000));
    highwaysMesh.add(temp);

    temp.clear();
    Mesh::Revolution(highway2HFunc, NULL, highwayConstSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(222000), _TV(6000), _TV(2000));
    highwaysMesh.add(temp);

    temp.clear();
    Mesh::Revolution(highway3HFunc, NULL, highwayVaryingSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(237000), _TV(-7000), _TV(10));
    highwaysMesh.add(temp);

    temp.clear();
    Mesh::Revolution(highway4HFunc, NULL, highwayVaryingSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(234500), _TV(-5000), _TV(10));
    highwaysMesh.add(temp);

    temp.clear();
    Mesh::Revolution(highway5HFunc, NULL, highwayVaryingSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(231700), _TV(-5800), _TV(10));
    highwaysMesh.add(temp);

    temp.clear();
    Mesh::Revolution(highway6HFunc, NULL, highwayVaryingSectionFunc)
      .generateMesh(temp, _TV(1.f), _TV(1.f), _TV(100), _TV(15));
    setIdForTiming(temp, 4 * _TV(15), _TV(231200), _TV(-4000), _TV(10));
    highwaysMesh.add(temp);

    highwaysMesh.rotate(DEG_TO_RAD * _TV(180.f), _TV(0.f), _TV(1.f), _TV(0.f));
    highwaysMesh.rotate(DEG_TO_RAD * _TV(90.f), _TV(1.f), _TV(0.f), _TV(0.f));
    highwaysMesh.translate(0.f, 2.f, 0.f);


    //
    // Sol
    //
    temp.clear();
    Mesh::Pave(_TV(15.f), 0.f, _TV(15.f)).generateMesh(temp, Mesh::pave_top);
    for (int j = 0; j < _TV(22); ++j)
      for (int i = 0; i < _TV(22); ++i)
	// On ne pave que la rue
	if ((i >= _TV(7) && i < _TV(11)) ||
	    (j >= _TV(8) && j < _TV(10)))
      {
	matrix4 transform = matrix4::translation(vector3f(_TV(-127.5f) + i * _TV(15.f), 0.f, _TV(-127.5f) + j * _TV(15.f)));
	highwaysMesh.add(temp, transform);
      }

    //
    // Buildings
    //
    Rand rand;
    const vector2f style(_TV(1.f), _TV(1.f));
    for (int j = 0; j < _TV(13); ++j)
      for (int i = 0; i < _TV(9); ++i)
	// On construit en dehors de la rue
	if ((i < _TV(3) || i >= _TV(5)) &&
	    (j < _TV(8) || j >= _TV(10) || (j < _TV(9) && i > _TV(3))))
      {
	const int date = rand.igen(_TV(223000), _TV(236000));
	const float magicId = float(date) + _TV(5000.f) / 10000.f;

	temp.clear();
	Mesh::Building(_TV(40.f), _TV(30.f), _TV(30.f), style)
	  .generateMesh(temp, magicId, 1.f, _TV(0.25f), rand, Mesh::Building::minDetail);
	temp.translate(_TV(-105.f) + i * _TV(30.f), 0.f, _TV(-270.f) + j * _TV(30.f));

	temp.setColor(Material::beautifulWhite);

	highwaysMesh.add(temp);
      }

    //
    // Lampadaires
    //
    Mesh::MeshStruct & lampPost = Mesh::getTempMesh();
    generateLampPost(lampPost, temp, false);
    temp.clear();
    temp.add(lampPost);
    lampPost.translate(-15.f, 0.f, -200.f);

    temp.rotate(DEG_TO_RAD * 180.f, 0.f, 1.f, 0.f);
    temp.translate(15.f, 0.f, -200.f);
    lampPost.add(temp);

    temp.clear();
    for (int i = 0; i < _TV(20); ++i)
    {
      lampPost.translate(0.f, 0.f, 20.f);
      temp.add(lampPost);
    }
    setIdForTiming(temp, lampPost.vertices.size, _TV(226500), _TV(100), _TV(2500));
    highwaysMesh.add(temp);


    lampPost.clear();
    generateLampPost(lampPost, temp, true);

    temp.clear();
    for (int i = 0; i < _TV(17); ++i)
    {
      lampPost.translate(0.f, 0.f, 20.f);
      if (i < _TV(6) || i > _TV(7))
      {
	temp.add(lampPost);
      }
    }
    temp.translate(_TV(0.f), 0.f, _TV(-150.f));
    temp.rotate(DEG_TO_RAD * 90.f, 0, 1.f, 0);
    setIdForTiming(temp, lampPost.vertices.size, _TV(225500), _TV(100), _TV(2500));
    highwaysMesh.add(temp);

    //
    // Pilliers
    //
    Mesh::MeshStruct & pillar = Mesh::getTempMesh();
    generatePillar(pillar, temp);
    pillar.translate(0.f, 0.f, -150.f);

    temp.clear();
    for (int i = 0; i < 10; ++i)
    {
      pillar.translate(0.f, 0.f, 30.f);
      temp.add(pillar);
    }
    setIdForTiming(temp, pillar.vertices.size, _TV(228000), _TV(5000), _TV(1100));
    highwaysMesh.add(temp);

    highwaysMesh.computeNormals();
    highwaysMesh.computeBaryCoord();

    SETUP_VBO(highways, highwaysMesh);
  }

  Node * create(date startDate, date endDate)
  {
    Node * root = Node::New(startDate, endDate);

    Renderable highways(Material::retroGrowing, VBO_(highways));

    root->attachRenderable(highways);

    return root;
  }
}
