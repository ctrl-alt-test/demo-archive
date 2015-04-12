//
// Intérieur de l'usine
//

#include "factory.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "cratestack.hh"
#include "gear.hh"
#include "interpolation.hh"
#include "rotor.hh"
#include "shaderid.hh"
#include "sphere.hh"
#include "textureid.hh"
#include "timing.hh"
#include "treadmill.hh"
#include "vbobuild.hh"
#include "vboid.hh"
#include "zeus.hh"

#include "tweakval.h"

#define START_DATE	factoryStartDate
#define END_DATE	factoryEndDate

#define ARRIVAL_END_DATE		(START_DATE + 36000)

// Incident de la sphère
#define SPHERE_DATE			sphereStartDate
#define STOP_VALIDATORS_DATE		sphereAlertDate
#define STOP_TEST_TREADMILL_DATE	sphereAlertDate
#define STOP_GRABBERS_DATE		sphereAlertDate
#define STOP_ARRIVAL_TREADMILL_DATE	sphereAlertDate
#define START_ZEUS_DATE			(sphereAlertDate + 8000)

#define START_ZEUS_WARM_DATE		(sphereAlertDate + 10000)
#define START_DIE_DATE			(START_ZEUS_WARM_DATE + 10000)
#define END_DIE_DATE			(START_DIE_DATE + 10000)
#define SPHERE_END_DATE			(END_DIE_DATE + 2000)

#define AFTER_TEST_START_DATE		SPHERE_END_DATE
#define AFTER_TEST_END_DATE		(AFTER_TEST_START_DATE + 16000)


#define DANCE_1_START_DATE		AFTER_TEST_END_DATE
#define DANCE_1_END_DATE		(DANCE_1_START_DATE + 3000)
#define DANCE_2_START_DATE		(DANCE_1_END_DATE + 3000)
#define DANCE_2_END_DATE		(DANCE_2_START_DATE + 9500)


#define COLUMN_HEIGHT 19

namespace Factory
{
  const date testTreadmillCycleDuration = 40;
  const date arrivalTreadmillCycleDuration = 200;
  const date onTestTreadmillDuration = 24000;
  const unsigned int pitLength = 2;
  const unsigned int pitWidth = 80;
  const date onPitTreadmillDuration = 31000;
  const date validatorCycleDuration = 4000;
  const float validatorSize = 2.f;
  const date dance1AnimDuration = 4000;
  const date dance2AnimDuration = 2000;

  // --------------------------------------------------------------------------

  static void generateFrameMesh();
  static void generateDisjoinedCubeMesh();
  static void generateShuffledCubeMesh();
  static void generateResizedCubeMesh();
  static void generateShearedCubeMesh();
  static void generateZFightCubeMesh();
  static void generateRubiksCubeMesh();


  static void createFloorAndCeil(RenderList & renderList);
  static void createFrame(RenderList & renderList);
  static void createCrateStack(RenderList & renderList);

  static void createFallingCube(RenderList & renderList);
  static void createArrivalBeam1(RenderList & renderList);
  static void createArrivalBeam2(RenderList & renderList);
  static void createArrivalTreadmill(RenderList & renderList);
  static void createArrivalCubes(RenderList & renderList);

  static void createArrivalGrabbers(RenderList & renderList);
  static void createGrabbedCubes(RenderList & renderList);

  static void createBeforeTestBeam(RenderList & renderList);
  static void createBeforeTestTreadmill(RenderList & renderList);
  static void createBeforeTestCubes(RenderList & renderList);
  static void createValidatedCubes(RenderList & renderList);

  static void createFakeBeam1(RenderList & renderList);
  static void createValidators(RenderList & renderList);

  static void createPitTreadmill(RenderList & renderList);
  static void createPitObjects(RenderList & renderList);

  static void createFinishedCubeBeams(RenderList & renderList);

  static void createDancingCrates(RenderList & renderList);

  // --------------------------------------------------------------------------

  void generateMeshes()
  {
    generateFrameMesh();
    generateDisjoinedCubeMesh();
    generateShuffledCubeMesh();
    generateResizedCubeMesh();
    generateShearedCubeMesh();
    generateZFightCubeMesh();
    generateRubiksCubeMesh();
  }

  // --------------------------------------------------------------------------

  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position)
  {
    ambient[0] = 0.1f;
    ambient[1] = 0.125f;
    ambient[2] = 0.15f;
    if (renderDate < factoryStartDate + 10000)
    {
      ambient[0] = 0;
      ambient[1] = 0;
      ambient[2] = 0;
    }
    else if (renderDate < factoryStartDate + 20000)
    {
      const float intensity = (renderDate - factoryStartDate - 10000) / 10000.f;
      ambient[0] = 0.1f * intensity;
      ambient[1] = 0.125f * intensity;
      ambient[2] = 0.15f * intensity;
    }
    if (renderDate >= sphereAlertDate)
    {
      if (renderDate < START_ZEUS_DATE)
      {
	// Gyrophare rouge
	ambient[0] = 0.2f;
	ambient[1] = 0.1f;
	ambient[2] = 0.1f;
	diffuse[0] = 1.f;
	diffuse[1] = 0.3f;
	diffuse[2] = 0.2f;
	position[0] = 20.f * msys_sinf(0.01f * renderDate);
	position[1] = 20.f;
	position[2] = 20.f * msys_cosf(0.01f * renderDate);
      }
      else if (renderDate < END_DIE_DATE)
      {
	const float level = (renderDate >= START_DIE_DATE ?
			     (renderDate > END_DIE_DATE - 2000 ?
			      (END_DIE_DATE - renderDate) / 2000.f : 1.f) :
			      (renderDate - START_ZEUS_DATE) / (float)(START_DIE_DATE - START_ZEUS_DATE));
	const float intensity = mix(1.f, 0.5f + msys_frand(), level);

	ambient[0] = 0.05f;
	ambient[1] = 0.075f;
	ambient[2] = 0.1f;

	diffuse[0] = intensity * 0.7f;
	diffuse[1] = intensity * 0.9f;
	diffuse[2] = intensity * 1.f;

	position[0] = 25.f + 2.f * level * msys_sfrand();
	position[1] = 30.f+ 2.f * level * msys_sfrand();
	position[2] = 4.f + 2.f * level * msys_sfrand();
      }
      else
      {
	ambient[0] = 0.1f;
	ambient[1] = 0.125f;
	ambient[2] = 0.15f;
      }
    }
  }

  // --------------------------------------------------------------------------

  static Sphere::Cube * sphereCube;

  void create(RenderList & renderList)
  {
    // Intérieur de l'usine
    createFloorAndCeil(renderList);
    createFrame(renderList);
    createCrateStack(renderList);

    // Sphère
    {
      sphereCube = new Sphere::Cube(START_DIE_DATE, END_DIE_DATE, SPHERE_END_DATE, renderList);
      glPushMatrix();
      glTranslatef(25.f, 12.f, 4.f);
      Sphere::addToList(renderList, SPHERE_DATE, START_DIE_DATE, START_DIE_DATE + 5000);
      glPopMatrix();
    }

    // Canon Zeus
    {
      glPushMatrix();
      glTranslatef(25.f, 15.f, 4.f);
      Zeus::addToList(renderList, START_ZEUS_DATE, SPHERE_END_DATE);
      glPopMatrix();
    }


    // Rotor
    {
      glPushMatrix();
      glTranslatef(-10.f, 15.f, 20.f);
      Rotor::addToList(renderList, START_DATE, END_DATE, 800, 10, Shader::default);
      glPopMatrix();
    }

    // Tapis roulants
    {
      createArrivalBeam1(renderList);
      createArrivalBeam2(renderList);
      createArrivalTreadmill(renderList);

      glPushMatrix();
      glTranslatef(5.f, 0, -55.f);
      glRotatef(-90.f, 0, 1.f, 0);
      for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
      {
	if (3 != treadmill % 4)
	{
	  createBeforeTestBeam(renderList);
	  createBeforeTestTreadmill(renderList);
	}
 	glTranslatef(0, 0, -5.f);
      }
      glPopMatrix();

      // Tapis des cubes non conformes
      createPitTreadmill(renderList);
      createPitObjects(renderList);

      // Faux tapis
      createFakeBeam1(renderList);

      // Tapis des cubes finis
      createFinishedCubeBeams(renderList);

      // Chorégraphie
      createDancingCrates(renderList);
    }

    //
    // FIXME : meublage de l'usine
    //

    // Cube qui tombe
    createFallingCube(renderList);
    createArrivalCubes(renderList);
    createBeforeTestCubes(renderList);
    createValidatedCubes(renderList);

    // Manipulateurs
    createArrivalGrabbers(renderList);
    createGrabbedCubes(renderList);

    // Validateurs
    createValidators(renderList);
  }

  void update(RenderList & renderList, date renderDate)
  {
    if (renderDate >= sphereCube->start && renderDate < sphereCube->morphEnd)
    {
      const float progression = clamp(interpolate((float)renderDate,
						  (float)sphereCube->start,
						  (float)sphereCube->morphEnd));
      const float x = mix(24.70f, 24.6f, progression);
      const float y = mix(11.72f, 11.6f, progression);
      const float z = mix( 3.85f,  3.6f, progression);
      glPushMatrix();
      glLoadIdentity();
      glTranslatef(x, y, z);
      sphereCube->queueRendering(renderDate);
      glPopMatrix();
    }
  }

  // --------------------------------------------------------------------------

  Array<VBO::vertex> * frameVertices = NULL;

  static void generateFrameMesh()
  {
    frameVertices = new Array<VBO::vertex>((6 * 60 * 2 + 7 * 7 * (COLUMN_HEIGHT - 2)) * Cube::numberOfVertices);

    for (unsigned int k = 0; k < 61; ++k)
      for (unsigned int j = 0; j < 61; ++j)
	if (j % 10 == 0 || k % 10 == 0)
	{
	  if (j % 10 == 0 && k % 10 == 0)
	    if ((j != 30 || k != 30) && (j != 10 || k != 0))
	    {
	      // On ne fait pas les deux premiers, qui auront une
	      // texture différente
	      for (unsigned int i = 2; i < COLUMN_HEIGHT; ++i)
	      {
		const vector3f p = {float(j), float(i), float(k)};
		VBO::addCube(*frameVertices, p, 1.f, true, true, false, false, true, true);
	      }
	    }
	  {
	    const vector3f p = {float(j), 19.f, float(k)};
	    VBO::addCube(*frameVertices, p, 1.f, true, true, true, false, true, true);
	  }
	}
  }

  //
  // Cube avec les sommets mal calés
  //
  VBO::vertex * disjoinedCubeVertices = NULL;

  static void generateDisjoinedCubeMesh()
  {
    disjoinedCubeVertices = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    for (unsigned int vertex = 0; vertex < Cube::numberOfVertices; ++vertex)
    {
      disjoinedCubeVertices[vertex] = Cube::vertices[vertex];
      VBO::vertex & dst = disjoinedCubeVertices[vertex];
      dst.x += 0.05f * msys_sfrand();
      dst.y += 0.05f * msys_sfrand();
      dst.z += 0.05f * msys_sfrand();
    }
  }

  //
  // Cube avec les indices mélangés
  //
  VBO::vertex * shuffledCubeVertices = NULL;

  static void generateShuffledCubeMesh()
  {
    shuffledCubeVertices = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    for (int vertex = 0; vertex < (int)Cube::numberOfVertices; ++vertex)
    {
      int modulo = vertex % 3;
      int index = vertex - modulo + (2 - modulo);
      if (index < 0)
	index += Cube::numberOfVertices;
      if (index >= (int)Cube::numberOfVertices)
	index -= Cube::numberOfVertices;
      shuffledCubeVertices[vertex] = Cube::vertices[index];
    }
  }

  //
  // Cube avec des longueurs différentes
  //
  VBO::vertex * resizedCubeVertices = NULL;

  static void generateResizedCubeMesh()
  {
    resizedCubeVertices = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    for (unsigned int vertex = 0; vertex < Cube::numberOfVertices; ++vertex)
    {
      resizedCubeVertices[vertex] = Cube::vertices[vertex];
      VBO::vertex & dst = resizedCubeVertices[vertex];
      if (dst.y > 0 && dst.z > 0)
      {
	dst.x *= 0.8f;
	dst.y *= 0.8f;
	dst.z *= 0.9f;
      }
    }
  }

  //
  // Cube cisaillé
  //
  VBO::vertex * shearedCubeVertices = NULL;

  static void generateShearedCubeMesh()
  {
    shearedCubeVertices = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    const float xshift = 0.5f * msys_sfrand();
    const float zshift = 0.5f * msys_sfrand();
    for (unsigned int vertex = 0; vertex < Cube::numberOfVertices; ++vertex)
    {
      shearedCubeVertices[vertex] = Cube::vertices[vertex];
      VBO::vertex & dst = shearedCubeVertices[vertex];
      dst.x += xshift * dst.y;
      dst.z += zshift * dst.y;
    }
  }

  //
  // Cube avec problème de Z-fight
  //
  unsigned int numberOfZFightCubeVertices = 0;
  VBO::vertex * zFightCubeVertices = NULL;

  static void generateZFightCubeMesh()
  {
    numberOfZFightCubeVertices = 2 * Cube::numberOfVertices;
    zFightCubeVertices = (VBO::vertex *)msys_mallocAlloc(numberOfZFightCubeVertices * sizeof(VBO::vertex));
    for (unsigned int vertex = 0; vertex < numberOfZFightCubeVertices; ++vertex)
    {
      const unsigned int index = (vertex < Cube::numberOfVertices ? vertex :
				  (vertex + 4) % Cube::numberOfVertices);
      zFightCubeVertices[vertex] = Cube::vertices[index];
      if (vertex != index)
      {
	VBO::vertex & dst = zFightCubeVertices[vertex];
	dst.x += 0.01f * msys_sfrand();
	dst.y += 0.01f * msys_sfrand();
	dst.z += 0.01f * msys_sfrand();
// 	dst.x += 2.f;
      }
    }
  }

  //
  // Rubik's cube
  //
  VBO::vertex * rubiksCubeVertices = NULL;

  static void generateRubiksCubeMesh()
  {
    rubiksCubeVertices = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    const vector3f red = {0.85f, 0.12f, 0};
    const vector3f purple = {0.72f, 0.42f, 0.55f};

    const vector3f white = {1.f, 1.f, 1.f};
    const vector3f yellow = {1.f, 0.92f, 0};

    const vector3f green = {0.26f, 0.58f, 0.26f};
    const vector3f blue = {0.19f, 0.49f, 0.73f};

    const vector3f p = {0, 0, 0};

    VBO::addCubeToChunk(rubiksCubeVertices, p, red, purple, white, yellow, green, blue);
  }

  // --------------------------------------------------------------------------

  static void createFloorAndCeil(RenderList & renderList)
  {
    glPushMatrix();
    const float floorSquareSize = 10.f;
    glScalef(floorSquareSize, floorSquareSize, floorSquareSize);
    glTranslatef(0.5f, -0.5f, 0.5f);
    glTranslatef(-6.f, 0, -6.f);
    for (unsigned int j = 0; j < 12; ++j)
      for (unsigned int i = 0; i < 12; ++i)
      {
	glPushMatrix();
	glTranslatef(float(i), 0, float(j));
	if (6 != j || 5 != i) // Fosse
	{
	  Renderable floor(END_DATE, Shader::bump, START_DATE);
	  floor.setTextures(Texture::antiSlip, Texture::antiSlipBump, Texture::antiSlipSpecular);
	  renderList.add(floor);
	}
	glTranslatef(0, 5.f, 0);
	renderList.add(Renderable(END_DATE, Shader::default));
	glPopMatrix();
      }
    glPopMatrix();

    glPushMatrix();
    glScalef(2.f * floorSquareSize, 2.f * floorSquareSize, 2.f * floorSquareSize);
    for (unsigned int j = 0; j < 2; ++j)
      for (unsigned int i = 0; i < 6; ++i)
      {
	if (i > 1)
	{
	  glPushMatrix();
	  glTranslatef(float(i) - 2.5f, float(j) + 0.5f, -3.5f);
	  Renderable structure(END_DATE, Shader::default, START_DATE);
	  structure.setTextures(Texture::concrete,
				Texture::none,
				Texture::concreteSpecular);
	  renderList.add(structure);
	  glPopMatrix();
	}
	glPushMatrix();
	glTranslatef(float(i) - 2.5f, float(j) + 0.5f, 3.5f);
	{
	  Renderable structure(END_DATE, Shader::default, START_DATE);
	  structure.setTextures(Texture::concrete,
				Texture::none,
				Texture::concreteSpecular);
	  renderList.add(structure);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.5f, float(j) + 0.5f, float(i) - 2.5f);
	{
	  Renderable structure(END_DATE, Shader::default, START_DATE);
	  structure.setTextures(Texture::concrete,
				Texture::none,
				Texture::concreteSpecular);
	  renderList.add(structure);
	}
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.5f, float(j) + 0.5f, float(i) - 2.5f);
	{
	  Renderable structure(END_DATE, Shader::default, START_DATE);
	  structure.setTextures(Texture::concrete,
				Texture::none,
				Texture::concreteSpecular);
	  renderList.add(structure);
	}
	glPopMatrix();
      }
    glPopMatrix();
  }

  static void createFrame(RenderList & renderList)
  {
    const Shader::id frameShader = Shader::bump;
    const float columnWidth = 2.f;

    glPushMatrix();
    glScalef(columnWidth, columnWidth, columnWidth);

    glTranslatef(0.5f, 0.5f, 0.5f);
    glTranslatef(-30.5f, 0, -30.5f);

    Renderable frame(END_DATE, frameShader, START_DATE, 1.f,
		     frameVertices->size, VBO::factoryFrame);
    frame.setTextures(Texture::concrete, Texture::defaultBump, Texture::concreteSpecular);
    renderList.add(frame);

    for (unsigned int k = 0; k < 61; ++k)
      for (unsigned int j = 0; j < 61; ++j)
	if (j % 10 == 0 && k % 10 == 0 &&
	    (j != 30 || k != 30) && (j != 10 || k != 0))
	{
	  glPushMatrix();
	  glTranslatef(float(j), 0, float(k));
	  {
	    Renderable block1(END_DATE, frameShader, START_DATE);
	    block1.setTextures(Texture::dangerStripes, Texture::defaultBump, Texture::dangerStripesSpecular);
	    renderList.add(block1);
	  }

	  glTranslatef(0, 1.f, 0);
	  {
	    Renderable block2(END_DATE, frameShader, START_DATE);
	    block2.setTextures(Texture::dangerStripes, Texture::defaultBump, Texture::dangerStripesSpecular);
	    renderList.add(block2);
	  }
	  glPopMatrix();
	}
    glPopMatrix();
  }

  static void createCrateStack(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(-38.f, 0, 22.f);
    CrateStack::addToList(renderList, START_DATE, END_DATE, 14, 7);
    glPopMatrix();
  }

  // ------------------------------------------------------------------
  // Chute du cube et arrivée sur le tapis

  static void createFallingCube(RenderList & renderList)
  {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-40.f,  14.8f, -70.f);

    const date switchDate = START_DATE + 2000;

    // Cube qui tombe
    Renderable fallingCube(switchDate, Shader::color, START_DATE,
			   1.f, Cube::numberOfVertices, VBO::theCube);
    fallingCube.setAnimation(Anim::fall);
    renderList.add(fallingCube);

    // Cube qui se déplace après la chute
    Renderable afterFallCube(START_DATE + 8000, Shader::color, switchDate,
			     1.f, Cube::numberOfVertices, VBO::theCube);
    afterFallCube.setAnimation(Anim::moveToTreadmill);
    renderList.add(afterFallCube);
    glPopMatrix();
  }

  // ------------------------------------------------------------------
  // Tapis d'arrivée

  static void addBeamBlock(RenderList & renderList)
  {
    Renderable block(END_DATE, Shader::bump, START_DATE);
    block.setTextures(Texture::concrete,
		      Texture::defaultBump,
		      Texture::concreteSpecular);
    renderList.add(block);
  }

  static void createArrivalBeam1(RenderList & renderList)
  {
    const unsigned int beamLength = 55;

    glPushMatrix();
    glTranslatef(-55.f, 0, -50.f);
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < 9; ++j)
    {
      glPushMatrix();
      glTranslatef(0, 0.5f + float(j), 0);
      addBeamBlock(renderList);

//       glTranslatef((float)(beamLength - 1), 0, 0);
//       addBeamBlock(renderList);
      glPopMatrix();
    }
    glPopMatrix();

    // Poutre du tapis d'arrivée du bas
    glPushMatrix();
    glTranslatef(-55.f, 13.3f, -50.f);
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < beamLength; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();
  }

  static void createArrivalBeam2(RenderList & renderList)
  {
    const unsigned int beamLength = 20;

    for (unsigned int j = 0; j < 28; ++j)
    {
      glPushMatrix();
      glTranslatef(22.f, 1.4f * (0.5f + float(j)), -50.f);
      glScalef(1.4f, 1.4f, 1.4f);
      addBeamBlock(renderList);
      glPopMatrix();
    }

    // Poutre du faux tapis d'arrivée du haut
    glPushMatrix();
    glTranslatef(23.4f, 20.3f, -50.f);
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < beamLength; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();
  }

  static void createArrivalTreadmill(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(-55.5f, 14.3f, -50.f);
    Treadmill::addToList(renderList,
			 START_DATE, ARRIVAL_END_DATE, ARRIVAL_END_DATE,
			 256, 7, Shader::default);
    glPopMatrix();
  }

  //
  // On veut que les sommets aient une couleur rgb aléatoire ; mais il
  // faut que deux sommets adjacents partagent une composante sinon
  // c'est moche.
  //
  static void affectOneComponent(char & r, char & g, char & b, char sr, char sg, char sb)
  {
    switch (msys_rand() % 3)
    {
    case 0:
      r = sr;
      break;
    case 1:
      g = sg;
      break;
    default:
      b = sb;
      break;
    }
  }

  static int cubeColorIndex(unsigned char x, unsigned char y, unsigned char z)
  {
    return 3 * (x + 2 * (y + 2 * z));
  }

  int notSoRandom(int seed) // FIXME : faire une version plus courte
  {
    msys_srand(seed);

    char colors[24];
    for (unsigned int i = 0; i < 24; ++i)
    {
      colors[i] = (msys_rand() % 2 == 0 ? 0 : 1);
    }
    for (char z = 0; z < 2; ++z)
      for (char y = 0; y < 2; ++y)
	for (char x = 0; x < 2; ++x)
	{
	  char & r = colors[cubeColorIndex(x, y, z)];
	  char & g = colors[cubeColorIndex(x, y, z) + 1];
	  char & b = colors[cubeColorIndex(x, y, z) + 2];

	  if (r == 0 && g == 0 && b == 0)
	  {
	    affectOneComponent(r, g, b, 1, 1, 1);
	  }

	  if (0 == x)
	  {
	    char & r2 = colors[cubeColorIndex(1, y, z)];
	    char & g2 = colors[cubeColorIndex(1, y, z) + 1];
	    char & b2 = colors[cubeColorIndex(1, y, z) + 2];
	    if (r != r2 && b != b2 && g != g2)
	    {
	      affectOneComponent(r2, g2, b2, r, g, b);
	    }
	    if (0 == y)
	    {
	      char & r2 = colors[cubeColorIndex(1, 1, z)];
	      char & g2 = colors[cubeColorIndex(1, 1, z) + 1];
	      char & b2 = colors[cubeColorIndex(1, 1, z) + 2];
	      if (r == r2 && b == b2 && g == g2)
	      {
		affectOneComponent(r2, g2, b2, !r, !g, !b);
	      }
	    }
	    if (0 == z)
	    {
	      char & r2 = colors[cubeColorIndex(1, y, 1)];
	      char & g2 = colors[cubeColorIndex(1, y, 1) + 1];
	      char & b2 = colors[cubeColorIndex(1, y, 1) + 2];
	      if (r == r2 && b == b2 && g == g2)
	      {
		affectOneComponent(r2, g2, b2, !r, !g, !b);
	      }
	    }
	  }
	  if (0 == y)
	  {
	    char & r2 = colors[cubeColorIndex(x, 1, z)];
	    char & g2 = colors[cubeColorIndex(x, 1, z) + 1];
	    char & b2 = colors[cubeColorIndex(x, 1, z) + 2];
	    if (r != r2 && b != b2 && g != g2)
	    {
	      affectOneComponent(r2, g2, b2, r, g, b);
	    }
	    if (0 == z)
	    {
	      char & r2 = colors[cubeColorIndex(x, 1, 1)];
	      char & g2 = colors[cubeColorIndex(x, 1, 1) + 1];
	      char & b2 = colors[cubeColorIndex(x, 1, 1) + 2];
	      if (r == r2 && b == b2 && g == g2)
	      {
		affectOneComponent(r2, g2, b2, !r, !g, !b);
	      }
	    }
	  }
	  if (0 == z)
	  {
	    char & r2 = colors[cubeColorIndex(x, y, 1)];
	    char & g2 = colors[cubeColorIndex(x, y, 1) + 1];
	    char & b2 = colors[cubeColorIndex(x, y, 1) + 2];
	    if (r != r2 && b != b2 && g != g2)
	    {
	      affectOneComponent(r2, g2, b2, r, g, b);
	    }
	  }
	}
    int final = 0;
    int c = 1;
    for (unsigned int i = 0; i < 24; ++i)
    {
      if (colors[i] != 0)
      {
	final += c;
      }
      c <<= 1;
    }
    return final << 4;
  }


  static void createArrivalCube(RenderList & renderList,
				unsigned int i,
				int id,
				date startDate,
				date endDate)
  {
    glPushMatrix();
    glTranslatef(-50.f + 5.f * (i % 3), 14.8f, -50.f);
    Renderable cube(endDate, Shader::color, startDate,
		    1.f, Cube::numberOfVertices,
		    (id == 0 ? VBO::theCube : VBO::singleCube));
    cube.setId(id);
    cube.setAnimation(Anim::arrivalCubes, START_DATE);
    cube.setAnimationStepShift((i / 3) / 4.f);
    cube.setAnimationStopDate(STOP_ARRIVAL_TREADMILL_DATE);
    renderList.add(cube);
    glPopMatrix();
  }

  static void createArrivalCubes(RenderList & renderList)
  {
    for (unsigned int i = 0; i < 12; ++i)
    {
      // id random sauf pour notre cube
      const int id = (i == 2 ? 0 : notSoRandom(i + 42));
      const date endDate = (i == 2 ? START_DATE + 18000 : ARRIVAL_END_DATE);
      createArrivalCube(renderList, i, id, START_DATE, endDate);
    }
    createArrivalCube(renderList, 2, notSoRandom(2 + 42), START_DATE + 18000, END_DATE);
  }

  static void createArrivalGrabbers(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(5.f, 16.8f, -50.f);
    for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
    {
      if (3 != treadmill % 4)
      {
	Renderable grabber(END_DATE, Shader::bump, START_DATE);
	grabber.setTextures(Texture::manipulator,
			    Texture::manipulatorBump,
			    Texture::manipulatorSpecular);
	grabber.setAnimation(treadmill < 4 ? Anim::grab1 : Anim::grab2);
	grabber.setAnimationStepShift(0.15f);
	grabber.setAnimationStopDate(STOP_GRABBERS_DATE);
	renderList.add(grabber);
      }
      else
      {
	glTranslatef(0, 7.f, 0);
      }
      glTranslatef(5.f, 0, 0);
    }
    glPopMatrix();
  }

  static void createGrabbedCubes(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(5.f, 15.8f, -50.f);
    for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
    {
      if (3 != treadmill % 4)
      {
	for (unsigned int i = 0; i < 12; ++i)
	{
	  // id random sauf pour notre cube
	  const int id = (i == 4 && treadmill == 2 ? 0 : notSoRandom((3 * i + treadmill) % 12 + 42));
	  Renderable cube(END_DATE, Shader::color, START_DATE,
		    1.f, Cube::numberOfVertices,
		    (id == 0 ? VBO::theCube : VBO::singleCube));
	  cube.setId(id);
	  cube.setAnimation(treadmill < 4 ? Anim::grabbed1 : Anim::grabbed2);
	  cube.setAnimationStepShift(i / 12.f);
	  cube.setAnimationStopDate(STOP_GRABBERS_DATE);
	  renderList.add(cube);
	}
      }
      else
      {
	glTranslatef(0, 7.f, 0);
      }
      glTranslatef(5.f, 0, 0);
    }
    glPopMatrix();
  }

  // ------------------------------------------------------------------
  // Tapis avant le test

  static void createBeforeTestBeam(RenderList & renderList)
  {
    const unsigned int beamLength = 41;

    const Shader::id beamShader = Shader::default;

    glPushMatrix();
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < 7; ++j)
    {
      glPushMatrix();
      glTranslatef(0, 0.5f + float(j), 0);
      addBeamBlock(renderList);

      glTranslatef((float)(beamLength - 1), 0, 0);
      addBeamBlock(renderList);
      glPopMatrix();
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 10.5f, 0);
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < beamLength; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();
  }

  static void createBeforeTestTreadmill(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(-0.5f, 11.5f, 0);
    Treadmill::addToList(renderList, START_DATE, END_DATE,
			 STOP_TEST_TREADMILL_DATE, 192, 7, Shader::default);
    glPopMatrix();
  }

  static void createBeforeTestCubes(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(5.f, 12.f, -48.f);
    for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
    {
      if (3 != treadmill % 4)
      {
	for (unsigned int i = 0; i < 12; ++i)
	{
	  // id random sauf pour notre cube
	  const int id = (i == 3 && treadmill == 2 ? 0 : notSoRandom((3 * (i + 1) + treadmill) % 12 + 42));
	  const date start = START_DATE;
	  const date end = START_ZEUS_WARM_DATE + 7000;
	  if (treadmill != 5 || i != 7)
	  {
	    Renderable cube(end, Shader::color, start,
			    1.f, Cube::numberOfVertices,
			    (id == 0 ? VBO::theCube : VBO::singleCube));
	    cube.setId(id);
	    cube.setAnimation(Anim::beforeTest);
	    cube.setAnimationStepShift((0.5f + i) / 12.f);
	    cube.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
	    renderList.add(cube);
	  }
	  else
	  {
	    Renderable beforeTest(end, Shader::color, start,
				  1.f, Cube::numberOfVertices, VBO::resizedCube);
	    beforeTest.setId(id);
	    beforeTest.setAnimation(Anim::beforeTest);
	    beforeTest.setAnimationStepShift((0.5f + i) / 12.f);
 	    beforeTest.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
	    renderList.add(beforeTest);

	    Renderable afterTest(35000, Shader::color, 31000,
				 1.f, Cube::numberOfVertices, VBO::resizedCube);
	    afterTest.setId(id);
	    afterTest.setAnimation(Anim::goToPit, 33000);
	    afterTest.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
	    renderList.add(afterTest);
	  }
	}
      }
      glTranslatef(5.f, 0, 0);
    }
    glPopMatrix();

    // Notre cube resté au test
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(15.f, 12.f, 4.f);
    Renderable grabbed(SPHERE_END_DATE + 5000, Shader::color,
		       STOP_TEST_TREADMILL_DATE, 1.f,
		       Cube::numberOfVertices, VBO::theCube);
    grabbed.setAnimation(Anim::grabbedValidated, SPHERE_END_DATE + 3500);
    renderList.add(grabbed);

    // Le manipulateur
    glTranslatef(0, 1.f, 0);
    Renderable grabber(SPHERE_END_DATE + 5000, Shader::bump,
		       SPHERE_END_DATE + 2500);
    grabber.setTextures(Texture::manipulator,
			Texture::manipulatorBump,
			Texture::manipulatorSpecular);
    grabber.setAnimation(Anim::grabValidated);
    grabber.setAnimationStepShift(0.55f);
    renderList.add(grabber);

    glPopMatrix();
  }

  static void createValidatedCubes(RenderList & renderList)
  {
    for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
      if (3 != treadmill % 4)
      {
	const int i = (treadmill < 3 ? 0 : 3); // Grosse truande
	const int id = (notSoRandom((3 * (i + 1) + treadmill) % 12 + 42));

	glPushMatrix();
	glTranslatef((treadmill + 1.f) * 5.f, 12.f, 4.f);
	date start = START_DATE + 28000;
	date end = SPHERE_DATE - 500;
	if (5 == treadmill)
	{
	  end = START_DATE + 32000;
	}

	// Cube manipulé
	Renderable grabbed(end, Shader::color, start);
	grabbed.setId(id);
	grabbed.setAnimation(Anim::grabbedValidated);
	grabbed.setAnimationStepShift(0.5f);
	grabbed.setAnimationStopDate(STOP_VALIDATORS_DATE);
	renderList.add(grabbed);

	// Manipulateur
	glTranslatef(0, 1.f, 0);
	Renderable grabber(end, Shader::bump, start);
	grabber.setTextures(Texture::manipulator,
			    Texture::manipulatorBump,
			    Texture::manipulatorSpecular);
	grabber.setAnimation(Anim::grabValidated);
	grabber.setAnimationStepShift(0.55f);
	grabber.setAnimationStopDate(STOP_VALIDATORS_DATE);
	renderList.add(grabber);

	glPopMatrix();
      }
  }

  // ------------------------------------------------------------------
  // Tapis qui gènent la vue

  static void createFakeBeam1(RenderList & renderList)
  {
    for (unsigned int j = 0; j < 10; ++j)
    {
      glPushMatrix();
      glTranslatef(-10.f, 1.4f * (0.5f + float(j)), -30.f);
      glScalef(1.4f, 1.4f, 1.4f);
      addBeamBlock(renderList);
      glPopMatrix();

      glPushMatrix();
      glTranslatef(44.6f, 1.4f * (0.5f + float(j)), -30.f);
      glScalef(1.4f, 1.4f, 1.4f);
      addBeamBlock(renderList);
      glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(-10.f, 14.7f, -30.f);
    glScalef(1.4f, 1.4f, 1.4f);
    for (unsigned int j = 0; j < 40; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10.f, 19.f, -30.f);
    glScalef(2.8f, 2.8f, 2.8f);
    for (unsigned int j = 0; j < 40; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10.f, 25.f, -30.f);
    glScalef(2.8f, 2.8f, 2.8f);
    for (unsigned int j = 0; j < 40; ++j)
    {
      addBeamBlock(renderList);
      glTranslatef(1.f, 0, 0);
    }
    glPopMatrix();
  }

  // ------------------------------------------------------------------

  static void createValidator(RenderList & renderList,
			      date start, date end, float animDelay,
			      Texture::id texture2Id,
			      int role)
  {
    Renderable validator(end, Shader::bump, start);
    validator.setTextures(Texture::validator,
			  Texture::validatorBump,
			  Texture::validatorSpecular);
    validator.setTexture2(texture2Id);
    validator.setAnimationStepShift(animDelay);

    switch (role)
    {
      case 0:
	validator.setAnimation(Anim::validatorCycle);
	validator.setAnimationStopDate(STOP_VALIDATORS_DATE);
	break;

      case 1:
	validator.setAnimation(Anim::validatorCycle);
	validator.setAnimationStopDate(STOP_VALIDATORS_DATE + 1000);
	break;

      case 2:
	validator.setAnimation(Anim::validatorCycle);
	validator.setAnimationStopDate(START_ZEUS_DATE + 1000);
	break;
    }

    renderList.add(validator);
  }

  static void createValidators(RenderList & renderList)
  {
    for (unsigned int treadmill = 0; treadmill < 8; ++treadmill)
      if (3 != treadmill % 4)
      {
	glPushMatrix();
	glTranslatef((treadmill + 1.f) * 5.f, 12.f, 4.f);
	date start = START_DATE;
	date rend = (4 == treadmill ? SPHERE_DATE : END_DATE);
	date end = rend;
	int role = (treadmill == 4 ? 0 : 1);

	if (5 == treadmill)
	{
	  date start2 = start + 32000;
	  date end2 = start2 + Factory::validatorCycleDuration / 2;

	  // KO left
	  createValidator(renderList, start2, end2, 0.25f, Texture::validatorKO, role);
	  // OK again left
	  createValidator(renderList, end2, end, 0.75f, Texture::validatorOK, role);

	  // KO right
	  createValidator(renderList, start2, end2, 0.75f, Texture::validatorKO, role);
	  // OK again right
	  createValidator(renderList, end2, end, 0.25f, Texture::validatorOK, role);

	  end = start2;
	}

	// Normal left
	createValidator(renderList, start, end, 0.25f, Texture::validatorOK, role);
	// Normal right
	createValidator(renderList, start, end, 0.75f, Texture::validatorOK, role);

	if (4 == treadmill) // sphere validators
	{
	  createValidator(renderList, rend, START_ZEUS_DATE, 0,    Texture::validatorKO, 0);
	  createValidator(renderList, rend, START_ZEUS_DATE, 0.5f, Texture::validatorKO, 0);
	  createValidator(renderList, START_ZEUS_DATE, END_DATE, 0,    Texture::validatorKO, 2);
	  createValidator(renderList, START_ZEUS_DATE, END_DATE, 0.5f, Texture::validatorKO, 2);
	}

	glPopMatrix();
      }
  }

  static void createPitTreadmill(RenderList & renderList)
  {
    glPushMatrix();
    glTranslatef(0, -18.f, 2.f);
    glScalef(0.5f, 0.5f, 0.5f);
    glRotatef(90.f, 1.f, 0, 0);
    Gear::addToList(renderList, START_DATE, SPHERE_END_DATE, STOP_TEST_TREADMILL_DATE,
		    Anim::pitTrack, 80, 2, 12, 1, 0.1f, Shader::bump);
    glPopMatrix();
  }

  static void createPitObjects(RenderList & renderList)
  {
    glPushMatrix();

    glTranslatef(-0.5f, 2.75f, 5.f);
    int id = 0;

    // Erreur de sommets
    glPushMatrix();
    glTranslatef(0, 0, -1.f);
    id = notSoRandom(100);
    {
      Renderable failure(END_DATE, Shader::color, START_DATE,
			 1.f, Cube::numberOfVertices, VBO::disjoinedCube);
      failure.setId(id);
      failure.setAnimation(Anim::pitObject);
      failure.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      renderList.add(failure);
    }
    glPopMatrix();

    // Erreur d'indices
    glPushMatrix();
    glTranslatef(0, 0, 1.f);
    id = notSoRandom(101);
    {
      Renderable failure(END_DATE, Shader::color, START_DATE,
			 1.f, Cube::numberOfVertices, VBO::shuffledCube);
      failure.setId(id);
      failure.setAnimation(Anim::pitObject);
      failure.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      failure.setAnimationStepShift(0.2f);
      renderList.add(failure);
    }
    glPopMatrix();

    // Erreur de dimensions
    glPushMatrix();
    glTranslatef(0, 0, 0.5f);
    id = notSoRandom(102);
    {
      Renderable failure(END_DATE, Shader::color, START_DATE,
			 1.f, Cube::numberOfVertices, VBO::resizedCube);
      failure.setId(id);
      failure.setAnimation(Anim::pitObject);
      failure.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      failure.setAnimationStepShift(0.25f);
      renderList.add(failure);
    }
    glPopMatrix();

    // Erreur de cisaillement
    glPushMatrix();
    glTranslatef(0, 0, 1.3f);
    id = notSoRandom(103);
    {
      Renderable failure(END_DATE, Shader::color, START_DATE,
			 1.f, Cube::numberOfVertices, VBO::shearedCube);
      failure.setId(id);
      failure.setAnimation(Anim::pitObject);
      failure.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      failure.setAnimationStepShift(0.7f);
      renderList.add(failure);
    }
    glPopMatrix();

    // Erreur de Z fight
    glPushMatrix();
    glTranslatef(0, 0, -1.5f);
    id = notSoRandom(105);
    {
      Renderable failure1(END_DATE, Shader::color, START_DATE,
			  1.f, numberOfZFightCubeVertices, VBO::zFightCube);
      failure1.setId(id);
      failure1.setAnimation(Anim::pitObject);
      failure1.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      failure1.setAnimationStepShift(0.8f);
      renderList.add(failure1);
    }
    glRotatef(0.01f, 0, 1.f, 0);
    glRotatef(0.01f, 1.f, 0, 0);
    {
      Renderable failure2(END_DATE, Shader::default, START_DATE,
			  1.f, numberOfZFightCubeVertices, VBO::zFightCube);
      failure2.setAnimation(Anim::pitObject);
      failure2.setAnimationStopDate(STOP_TEST_TREADMILL_DATE);
      failure2.setAnimationStepShift(0.8f);
      renderList.add(failure2);
    }
    glPopMatrix();

    glPopMatrix();
  }

  static void createFinishedCubeBeams(RenderList & renderList)
  {
    for (unsigned int j = 0; j < 5; ++j)
    {
      glPushMatrix();
      glTranslatef(30.f, 10.f + 4.f * j, 40.f);
      glRotatef(90.f, 0, 1.f, 0);
      glScalef(1.4f, 1.4f, 1.4f);
      for (unsigned int i = 0; i < 15; ++i)
      {
	addBeamBlock(renderList);
	glTranslatef(1.f, 0, 0);
      }
      glPopMatrix();
    }

    for (unsigned int j = 0; j < 5; ++j)
      for (unsigned int i = 0; i < 10; ++i)
      {
	glPushMatrix();
	glTranslatef(30.f, 11.2f + 4.f * j, 20.f + 2.f * i);

	Renderable cube(END_DATE, Shader::bump, START_DATE,
			1.f, Cube::numberOfVertices,
			(j == 2 ? VBO::rubiksCube : VBO::singleCube));
	if (0 == j)
	  cube.setTextures(Texture::pavement, Texture::pavementBump, Texture::pavementSpecular);
	else if (1 == j)
	  cube.setTextures(Texture::mario);
	else if (2 == j)
	  cube.setTextures(Texture::rubiks, Texture::rubiksBump, Texture::rubiksSpecular);
	else if (3 == j)
	  cube.setTextures(3 == i ? Texture::companionCube : Texture::storageCube,
			   Texture::companionCubeBump, Texture::companionCubeSpecular);
	else
	  cube.setTextures(Texture::crate, Texture::crateBump, Texture::crateSpecular);

	renderList.add(cube);
	glPopMatrix();
      }
  }

// -------------------------------------------------------------------

  static void addCrateDirection(RenderList & renderList, unsigned int shift)
  {
    const unsigned int maxRows = 2;
    const unsigned int dt = 2000 * shift;
    glTranslatef(0, 0, -2.f);
    for (unsigned int j = 0; j < maxRows; ++j)
    {
      glTranslatef(0, 0, 4.f);
      for (unsigned int i = 0; i < 5; ++i)
      {
	const date start = DANCE_2_START_DATE + dt + 4000 * j;
	const date end = DANCE_2_END_DATE;
	Renderable crate(end, Shader::bump, start, 2.f);
	crate.setTextures(Texture::crate, Texture::crateBump, Texture::crateSpecular);
	crate.setAnimation(Anim::dance1);
	crate.setAnimationStepShift(0.2f * i + 0.1f * (j + shift));
	renderList.add(crate);
      }
    }
  }

  static void createDancingCrates(RenderList & renderList)
  {
    // Dance en rond
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(10.f, 1.f, 30.f);
    for (unsigned int i = 0; i < 12; ++i)
    {
      glPushMatrix();
      glRotatef(i * 30.f, 0, 1.f, 0);
      glTranslatef(0, 0, 6.f);
      {
	const date start = DANCE_1_START_DATE + 150 * i;
	const date end = min(DANCE_1_END_DATE, start + dance2AnimDuration);
	Renderable crate(end, Shader::bump, start, 2.f);
	crate.setTextures(Texture::crate,
			  Texture::crateBump,
			  Texture::crateSpecular);
	crate.setAnimation(Anim::dance2);
	renderList.add(crate);
      }
      glPopMatrix();
    }
    glPopMatrix();

    // Dance en croisements
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(10.f, 1.f, 30.f);

    // Cubes dans un sens
    glPushMatrix();
    addCrateDirection(renderList, 0);
    glPopMatrix();

    // Cubes dans l'autre sens
    glPushMatrix();
    glRotatef(180.f, 0, 1.f, 0);
    addCrateDirection(renderList, 0);
    glPopMatrix();

    // Cubes orthogonaux
    glPushMatrix();
    glRotatef(90.f, 0, 1.f, 0);
    addCrateDirection(renderList, 1);
    glPopMatrix();

    // Cubes orthogonaux dans l'autre sens
    glPushMatrix();
    glRotatef(-90.f, 0, 1.f, 0);
    addCrateDirection(renderList, 1);
    glPopMatrix();

    glPopMatrix();
  }
}
