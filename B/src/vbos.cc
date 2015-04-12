//
// Tous les VBOs utilisés
//

#include "vbos.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "city.hh"
#include "cube.hh"
#include "factory.hh"
#include "forest.hh"
#include "screen.hh"
#include "shaders.hh"
#include "sphere.hh"
#include "treadmill.hh"
#include "vbobuild.hh"
#include "vbodata.hh"
#include "vboid.hh"

namespace VBO
{
  Element * list = NULL;

  void loadVBOs(City* city, Loading::ProgressDelegate * pd, int low, int high)
  {
    const unsigned int numberOfSteps = 6;
    const int ldStep = (high - low) / numberOfSteps;
    int currentLd = low;


    list = new Element[numberOfVBOs];
    int index = oglGetAttribLocation(Shader::list[Shader::bump].id(), "vTan");


    // Cube de base
    // ============
    list[singleCube].setupData(Cube::numberOfVertices * sizeof(vertex), Cube::vertices, index);


    // LE cube
    // ============
    {
      vertex cube[24];
      for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      {
	cube[i] = Cube::vertices[i];
	vertex & dst = cube[i];
	dst.r = 0.5f + dst.x;
	dst.g = 0.5f + dst.y;
	dst.b = 0.5f + dst.z;
      }
      list[theCube].setupData(Cube::numberOfVertices * sizeof(vertex), cube, index);
    }

    // Morceaux de cube
    {
      vertex cubeWallsVertices[20];
      vertex cubeRoofVertices[4];
      vector3f o = {0, 0, 0};
      addCubeToChunk(cubeWallsVertices, o, 1.f, true, true, true, false, true, true);
      addCubeToChunk(cubeRoofVertices, o, 1.f, false, false, false, true, false, false);
      list[cubeWalls].setupData(20 * sizeof(vertex), cubeWallsVertices, index);
      list[cubeRoof].setupData(4 * sizeof(vertex), cubeRoofVertices, index);
    }

    // Tapis roulants
    // ==============
    Treadmill::generateMeshes();
    list[treadmillChunk0].setupData(Treadmill::numberOfVerticesChunk0 * sizeof(vertex), Treadmill::chunk0, index);
    list[treadmillChunk1].setupData(Treadmill::numberOfVerticesChunk1 * sizeof(vertex), Treadmill::chunk1, index);
    list[treadmillChunk2].setupData(Treadmill::numberOfVerticesChunk2 * sizeof(vertex), Treadmill::chunk2, index);
    list[treadmillChunk3].setupData(Treadmill::numberOfVerticesChunk3 * sizeof(vertex), Treadmill::chunk3, index);
    //
    pd->func(pd->obj, currentLd); currentLd += ldStep;


    // Sphère (si si)
    // ==============
    Sphere::generateMesh();
    list[sphere].setupData(Sphere::numberOfVertices * sizeof(vertex), Sphere::vertices, index);
    //
    pd->func(pd->obj, currentLd); currentLd += ldStep;


    // Usine
    // =====
    Factory::generateMeshes();
    list[factoryFrame].setupData(*Factory::frameVertices, index);
    list[disjoinedCube].setupData(Cube::numberOfVertices * sizeof(vertex), Factory::disjoinedCubeVertices, index);
    list[shuffledCube].setupData(Cube::numberOfVertices * sizeof(vertex), Factory::shuffledCubeVertices, index);
    list[resizedCube].setupData(Cube::numberOfVertices * sizeof(vertex), Factory::resizedCubeVertices, index);
    list[shearedCube].setupData(Cube::numberOfVertices * sizeof(vertex), Factory::shearedCubeVertices, index);
    list[zFightCube].setupData(Factory::numberOfZFightCubeVertices * sizeof(vertex), Factory::zFightCubeVertices, index);
    list[rubiksCube].setupData(Cube::numberOfVertices * sizeof(vertex), Factory::rubiksCubeVertices, index);
    //
    pd->func(pd->obj, currentLd); currentLd += ldStep;


    // Ville
    // =====
    if (city)
    {
      city->generateMeshes();

      for (int i = 0; i < city->buildingsChunks().size; ++i)
      {
	list[buildingsWalls0 + i].setupData(*(city->buildingsChunks()[i]), index);
      }

      list[cityGround].setupData(city->groundChunk(), index);
      list[cityPavement].setupData(city->pavementChunk(), index);

      list[cityStreetH].setupData(city->streetHChunk(), index);
      list[cityStreetV].setupData(city->streetVChunk(), index);
      list[cityStreetO].setupData(city->streetOChunk(), index);

      list[cityLights].setupData(city->lightChunk(), index);

      //list[cityBuilding1].setupData(city->buildingChunk1(), index);
    }
    //
    pd->func(pd->obj, currentLd); currentLd += ldStep;


    // Sous bois
    // =========
    Forest::generateMeshes();
    list[forestGround].setupData(*Forest::ground, index);
    list[forestTrees].setupData(*Forest::trees, index);
    //
    pd->func(pd->obj, currentLd); currentLd += ldStep;


    // Écran
    // =====
    Screen::generateMeshes();
    list[screen].setupData(Screen::numberOfScreenVertices * sizeof(vertex), Screen::screen, index);
    list[redPixel].setupData(Cube::numberOfVertices * sizeof(vertex), Screen::redCube, index);
    list[greenPixel].setupData(Cube::numberOfVertices * sizeof(vertex), Screen::greenCube, index);
    list[bluePixel].setupData(Cube::numberOfVertices * sizeof(vertex), Screen::blueCube, index);
    //
    pd->func(pd->obj, high);

    OGL_ERROR_CHECK("Fin du loadVBOs");
  }

  // FIXME : destruction des VBO
}
