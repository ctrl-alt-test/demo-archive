// Sous-bois

#include "forest.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "array.hh"
#include "interpolation.hh"
#include "lsystem.hh"
#include "lsystemrule.hh"
#include "textures.hh"
#include "timing.hh"
#include "vbobuild.hh"

#define START_DATE	forestStartDate
#define GROW_START_DATE	(START_DATE + 10000)
#define CUBE_WALK_DATE	(START_DATE + 12000)
#define ALL_TREES_DATE	(START_DATE + 19000)
#define END_DATE	forestEndDate

// Continue a afficher la foret au debut de la rope
#define FULL_FOREST_END_DATE (END_DATE + 4000)

namespace Forest
{
  Array<VBO::vertex> *ground = NULL;
  Array<VBO::vertex> *trees = NULL;

  const date cubeStopDuration = 4000;

  const int size = 100;
  Texture::Channel *map = NULL;

  static RenderList *render = NULL;
  static RenderList *offlineRender = NULL;

  const char* lx[5] = {
    "F-F+F[++>>X]>>[F&&>>X]>>[-->>X]",
    "F+F-F[--<<X]>>[F&>>X]>[++^X]",
    "F-F+F[++>F]>>[F&X>>X]>>[-->>X]",
    "F<F>F[++>>X]F>>[F&&>>X]>>[-->>X]",
    "F-F+F[++>>X]>>[F&&>>X]>>[-->>X]",
  };
  const char* lf = "F";
  const char* lg = "G";
  const char* lh = "H";

  const unsigned char treePositions[] = {
    3, 13,
    4, 21,
    4, 33,
    21, 44,
    8, 14,
    13, 48,
    18, 3,
    26, 29,
    31, 38,
    32, 23,
    35, 10,
    36, 4,
    36, 50,
    41, 30,
    42, 18,
    44, 5,

    63, 28,
    67, 20,
    67, 46,
    71, 3,
    73, 29,
    76, 12,
    76, 44,
    79, 6,
    87, 33
  };

  typedef struct
  {
    int x;
    int y;
    date start;
    date lifetime;
  } treeSeed;

  const treeSeed treeSeeds[] = {
    { 5,  7,    0, 5000},
    {-5, 10, 2500, 5000},
    {-3, 25, 4000, 3500},
    { 8, 38, 6000, 3000},
    {-5, 45, 8000, 4000}
  };

  static void generateHeightMap()
  {
    map = new Texture::Channel(size, size);
    Texture::Channel r(size, size);
    Texture::Channel v(size, size);
    Texture::Channel b(size, size);
    Texture::forestHeightMap(*map, r, v, b, size);
    ground = new Array<VBO::vertex>(size * size * Cube::numberOfVertices);

    Texture::Channel &h = *map;
    for (unsigned j = 0; j < size; ++j)
    {
      const unsigned shift = j * size;
      for (unsigned i = 0; i < size; ++i)
      {
	const vector3f p = {(float)i, 5.f * h[i + shift], (float)j};
	const vector3f c = {r[i + shift], v[i + shift], b[i + shift]};
	const bool x1face = /* i > 0 */ i > size / 3 && h[i - 1 + shift] < p.y;
	const bool x2face = /* i < size - 1 */ i < (2 * size) / 3 && h[i + 1 + shift] < p.y;
	const bool y1face = false;
	const bool y2face = true;
	const bool z1face = j > 0 && h[i + shift - size] < p.y;
	const bool z2face = j < size / 3 /* size - 1 */ && h[i + shift + size] < p.y;
	VBO::addCube(*ground, p, c,
		     x1face, x2face, y1face, y2face, z1face, z2face);
      }
    }
    DBG("Terrain de la forêt généré : %d sommets", ground->size);
  }

  static void addTreeElement(date birth, date death, unsigned int depth)
  {
    Renderable node(0, Shader::none, 0);
    node.setId(depth);
    offlineRender->add(node);
  }

  static void addSingleTree(int kind)
  {
    const char* x = lx[kind];

    int iter = 4;
    float angle = 15.f + 4 * msys_frand();
    float reduction = 0.8f;
    float forward = 1.4f + msys_sfrand() * 0.2f;
    unsigned char nb_cubes = 1;

    glRotatef(-90.f, 1.f, 0, 0);

    if (kind == 1) angle = 18.f + 2 * msys_frand();
    if (kind == 3) iter = 5;

    LSystem::System * l =
      new LSystem::System(*offlineRender, -1, 0,
			  Shader::none, Anim::none, Texture::none,
			  angle, reduction, forward, nb_cubes,
			  0, 0, addTreeElement);
    LSystem::state userSystem = l->iterate(x, lf, lg, lh, iter);
    l->addStateToList(userSystem);
    msys_mallocFree(userSystem);
    delete l;
  }

  static void generateTrees()
  {
    const unsigned int numberOfTrees = sizeof(treePositions) / (2 * sizeof(unsigned char));
    const unsigned int maxNumberOfCubePerTree = 256; // Au pif, mais ça fait un majorant
    const unsigned int maxNumberOfCubes = numberOfTrees * maxNumberOfCubePerTree;
    trees = new Array<VBO::vertex>(maxNumberOfCubes * Cube::numberOfVertices);

    offlineRender = new RenderList(maxNumberOfCubes);

    for (unsigned int i = 0; i < numberOfTrees; ++i)
    {
      const unsigned int index = 2 * i;
      const unsigned int x = 99 - treePositions[index];
      const unsigned int y = 99 - treePositions[index + 1];
      glPushMatrix();
      glLoadIdentity();
      glTranslatef((float)x, 5.f * (*map)[x + y * size], (float)y);
      const float rnd = msys_frand() * 0.05f;
      glScalef(0.4f + rnd, 0.4f + rnd, 0.4f + rnd);
      glRotatef(360.f * 20.f * rnd, 0, 1.f, 0);
      addSingleTree(numberOfTrees % 5);
      glPopMatrix();
    }
    for (int i = 0; i < offlineRender->size; i++)
    {
      const Renderable & cube = (*offlineRender)[i];
      const float maxDepth = 14.f; // Etabli visuellement
      const float coeff = cube.id() / maxDepth;

      const float ra = 0.25f;//0.44f;
      const float ga = 0.51f;//90f;
      const float ba = 0.00f;

      const float r1 = 0.00f;
      const float g1 = 0.71f;
      const float b1 = 0.40f;

      const float r2 = 0.85f;
      const float g2 = 0.98f;
      const float b2 = 0.00f;

      const float r3 = 0.89f;
      const float g3 = 0.00f;
      const float b3 = 0.30f;

      const float w1 = UnsecureSpline5_interpolation(msys_frand());
      const float w2 = UnsecureSpline5_interpolation(msys_frand());
      const float w3 = UnsecureSpline5_interpolation(msys_frand());
      const float invW = 1.f / (w1 + w2 + w3);

      const float rb = invW * (w1 * r1 + w2 * r2 + w3 * r3);
      const float gb = invW * (w1 * g1 + w2 * g2 + w3 * g3);
      const float bb = invW * (w1 * b1 + w2 * b2 + w3 * b3);

      const float r = mix(ra, rb, coeff);
      const float g = mix(ga, gb, coeff);
      const float b = mix(ba, bb, coeff);
      const vector3f color = {r, g, b};

      addCube(*trees, cube.viewMatrix(), color);
    }
    delete offlineRender; offlineRender = NULL;

    DBG("Arbres de la forêt générés : %d sommets", trees->size);
  }

  void generateMeshes()
  {
    generateHeightMap();
    generateTrees();
  }

  static void renderGrowingCube(date birth, date death, unsigned depth)
  {
    const short int lo = depth & 0xff;
    const short int hi = msys_rand() & 0xff;
    const unsigned int id = (hi<<8)|lo;

    Renderable cube(death, Shader::tree, birth);
    cube.setId(id);
    cube.setAnimation(Anim::growingTree);
    render->add(cube);
  }

  static void addGrowingTree(RenderList & renderList, date start, date end, int kind)
  {
    LSystem::System * l = NULL;
    LSystem::state userSystem = NULL;

    int iter = 6;
    float angle = 15.f + 4 * msys_frand();
    float reduction = 0.8f;
    float forward = 1.4f + msys_sfrand() * 0.2f;
    unsigned char nb_cubes = 1;

    glRotatef(-90.f, 1.f, 0, 0);
    l = new LSystem::System(renderList, end, start,
			    Shader::default, Anim::none, Texture::none,
			    angle, reduction, forward, nb_cubes,
			    100, 0, renderGrowingCube);
    userSystem = l->iterate(lx[kind], lf, lg, lh, iter);
    l->addStateToList(userSystem);
    msys_mallocFree(userSystem);
    delete l;
  }

  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position)
  {
    ambient[0] = 0.7f * 1.f;
    ambient[1] = 0.7f * 0.87f;
    ambient[2] = 0.7f * 0.83f;
    ambient[3] = 0.9f;

    diffuse[0] = 0.3f * 1.f;
    diffuse[1] = 0.3f * 1.f;
    diffuse[2] = 0.3f * 1.f;
    diffuse[3] = 0.7f;
  }

  static void displayTheCube(RenderList & renderList, int x, int y, date birth, date death)
  {
    glPushMatrix();
    glTranslatef((float)x, 0.5f + 5.f * (*map)[x + y*size], (float)y);

    Renderable arrivingCube(CUBE_WALK_DATE, Shader::color, START_DATE,
			    0.9f, Cube::numberOfVertices, VBO::theCube);
    arrivingCube.setAnimation(Anim::arrivingForestCube);
    renderList.add(arrivingCube);

    glTranslatef(0, 0, -1.f); // Contournement du bug de marche
    Renderable walkingCube(END_DATE, Shader::color, CUBE_WALK_DATE,
			   0.9f, Cube::numberOfVertices, VBO::theCube);
    walkingCube.setAnimation(Anim::forestWalk);
    renderList.add(walkingCube);

    glPopMatrix();
  }

  // FIXME : utiliser la même couleur que pour les arbres ?
  static void addJumpingCube(RenderList & renderList, int x, int z)
  {
    glPushMatrix();
    const float cube_size = 0.4f;
    x += size / 2;
    glTranslatef((float)x, 5.f*(*map)[x + z*size] + 0.5f*cube_size, (float)z);
    Renderable cube(FULL_FOREST_END_DATE, Shader::tree, CUBE_WALK_DATE - 1000 + 180 * z, cube_size);
    cube.setId(msys_rand());
//     cube.setTextures(Texture::none);
    cube.setAnimation(Anim::forestJumpingCube);
    renderList.add(cube);
    glPopMatrix();
  }

  static void addJumpingCubes(RenderList & renderList)
  {
    for (int i = 0; i < 72; i += 4)
    {
      addJumpingCube(renderList, -2, i);
      addJumpingCube(renderList,  2, i);
    }
  }

  void create(RenderList & renderList)
  {
    render = &renderList;

    // Le terrain
    glPushMatrix();
    glTranslatef(0.f, -0.5f, 0.f);
    Renderable ground(FULL_FOREST_END_DATE, Shader::grass, START_DATE,
		      1.f, ground->size, VBO::forestGround);
    ground.setTextures(Texture::moss, Texture::mossBump, Texture::mossSpecular);
    ground.setTexture2(Texture::rosace);
    renderList.add(ground);
    glPopMatrix();

    // Notre cube
    displayTheCube(renderList, size / 2, 10, START_DATE, END_DATE);

    addJumpingCubes(renderList);

    // Arbres qui poussent
    const unsigned int numberOfSeeds = sizeof(treeSeeds) / sizeof(treeSeed);
    for (unsigned int i = 0; i < numberOfSeeds; ++i)
    {
      const treeSeed & seed = treeSeeds[i];

      glPushMatrix();
      const float rnd = msys_sfrand();
      glTranslatef((float)(size / 2 + seed.x),
		   5.f * (*map)[size / 2 + seed.x + seed.y * size],
		   (float)seed.y);
      glScalef(0.4f, 0.4f, 0.4f);
      glRotatef(360.f * rnd, 0, 1.f, 0);
      addGrowingTree(renderList, GROW_START_DATE + seed.start,
		     GROW_START_DATE + seed.start + seed.lifetime, 0);
      glPopMatrix();
    }

    // Arbres statiques
    renderList.add(Renderable(FULL_FOREST_END_DATE, Shader::grass,
			      ALL_TREES_DATE,
			      1.f, trees->size, VBO::forestTrees));

    // Les fleurs
    for (unsigned int i = 0; i < 200; ++i)
    {
      const int x = msys_rand() % size;
      const int y = msys_rand() % size;
      const float h = 0.2f + 5.f * (*map)[x + y * size];
      glPushMatrix();
      glTranslatef(x + msys_frand(), h, y + msys_frand());
      Renderable flower(FULL_FOREST_END_DATE, Shader::default,
			GROW_START_DATE + msys_rand() % 15000, 0.2f);
      flower.setTextures(Texture::fuchsia);
      flower.setAnimation(Anim::growingTree);
      renderList.add(flower);
      glPopMatrix();
    }

    delete map; map = NULL;
  }
}
