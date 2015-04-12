
#include "city.hh"

#include <stdio.h>
#include <stdlib.h>

#include "sys/msys.h"
#include <GL/gl.h>

#include "animid.hh"
#include "buildings.hh"
#include "cube.hh"
#include "factory.hh"
#include "interpolation.hh"
#include "queue.hh"
#include "shaderid.hh"
#include "spline.h"
#include "textures.hh"
#include "timing.hh"
#include "vbobuild.hh"

#define START_DATE	cityStartDate
#define TRAIN_LEAVE_DATE	(START_DATE - 2000)
#define END_DATE	cityEndDate

static const char dirs[] = {-1,0,  1,0,  0,-1,  0,1};

#define numHRoads 3
#define numVRoads 2
static const int hRoads[numHRoads] = { 20, 30, 40 };
static const int vRoads[numVRoads] = { 24, 34 };

// Spline pour les rails
float rails_data[rails_points*4] = {
// temps,   x,    y,    z (rempli a partir du terrain)
  0.f,   137.f,  0.f, 56.f,
  40.f,	 136.f,	 0.f, 39.f,
  65.f,	 120.f,	 0.f, 37.f,
  90.f,	 110.f,	 0.f, 32.f,
  120.f,  84.f,	 0.f, 30.f,
  170.f,  56.f,	 0.f, 4.f };

// variation de hauteur du terrain
static const float heightVariation = 0.05f;

static vector3f getGrassColor(int i, int j)
{
//static const vector3f grassColor = {0.47f, 0.55f, 0.15f}; // couleur de Zavie
  static const vector3f grassColor = {0.29f, 0.42f, 0.06f};  // FIXME couleur
  static const vector3f grassColor2 = {0.49f, 0.65f, 0.08f}; // FIXME couleur

  vector3f color;
  Texture::Channel *perlin7 = Texture::getPerlin7();
  const float coef = (*perlin7)[i + perlin7->Width() * j];
  color.x = mix(grassColor.x, grassColor2.x, coef);
  color.y = mix(grassColor.y, grassColor2.y, coef);
  color.z = mix(grassColor.z, grassColor2.z, coef);
  return color;
}

City::City(int w, int h):
  width(w), height(h),
  renderLists_(9), // Nombre de textures de bâtiment différentes
  buildingsChunks_(9),
  groundChunk_(w*h*2*24),
  streetHChunk_(w*h*24),
  streetVChunk_(w*h*24),
  streetOChunk_(w*h*24),
  pavementChunk_(w*h*24),
  lightChunk_(w*h*24)
{
  data = (char**) msys_mallocAlloc(sizeof(char*) * w);
  for (int i = 0; i < w; i++)
    data[i] = (char*) msys_mallocAlloc(sizeof(char) * h);

  hmap = (float**) msys_mallocAlloc(sizeof(float*) * w);
  for (int i = 0; i < w; i++)
    hmap[i] = (float*) msys_mallocAlloc(sizeof(float) * h);

  // Force la seed pour avoir toujours la meme ville
  msys_srand(42);
  fill();

  // Cree des routes aleatoirement le long des avenues
  for (int i = 22; i < 50; i += 4)
    for (int k = 0; k < 2; k++) {
      random_walk(20, i);
      random_walk(30, i);
      random_walk(40, i);
    }
  for (int i = 18; i < 52; i += 2) {
    random_walk(i, 24);
    random_walk(i, 34);
  }

  // Nettoie la matrice
  prune();

  // hacks manuels
  data[31][22] = 1; // vire une route
  data[42][23] = 1;
  data[38][19] = 1;
  data[39][20] = 1;

  // Calcule la hauteur du terrain
  computeHeightMap();

  // Met a jour les tableaux rails avec la hauteur du terrain
  for (int i = 0; i < rails_points; i++)
  {
    float *ptr = rails_data + i * 4;
    const int x = msys_ifloorf(ptr[1]);
    ptr[2] = hmap[x >= width ? width-2 : x][msys_ifloorf(ptr[3])];
  }

  for (int i = 0; i < renderLists_.max_size; ++i)
  {
    renderLists_.add(new RenderList(width * height * 5));
  }
  for (int i = 0; i < buildingsChunks_.max_size; ++i)
  {
    buildingsChunks_.add(new Array<VBO::vertex>(width * height * 5 * 24));
  }
}

City::~City()
{
  for (int i = 0; i < width; i++)
    msys_mallocFree(data[i]);
  msys_mallocFree(data);

  for (int i = 0; i < width; i++)
    msys_mallocFree(hmap[i]);
  msys_mallocFree(hmap);

  for (int i = 0; i < renderLists_.size; ++i)
  {
    RenderList * todel = renderLists_[i];
    delete todel;
  }
  renderLists_.size = 0;

  for (int i = 0; i < buildingsChunks_.max_size; ++i)
  {
    Array<VBO::vertex> * todel = buildingsChunks_[i];
    delete todel;
  }
  buildingsChunks_.size = 0;
}

void City::fill(void)
{
  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      data[i][j] = i % 2 || j % 2;

  for (int j = 0; j < height; j++)
    for (int i = 0; i < numHRoads; i++)
      data[hRoads[i]][j] = 4;
  for (int i = 0; i < hRoads[2]; i++)
      data[i][vRoads[0]] = 4;
  for (int i = hRoads[0]; i < width; i++)
      data[i][vRoads[1]] = 4;
}

// Se promène en créant des rues
void City::random_walk(int x, int y)
{
  assert(x % 2 == 0);
  assert(y % 2 == 0);
  while (true)
    {
      if (x <= 2 || x >= width - 4 || y <= 2 || y >= height - 4) break;
      data[x][y] = 2;
      int j = 2 * (msys_rand() % 4);
      int i;
      for (i = 0; i < 4; i++, j = (j+2) % 8)
          if (data[x + dirs[j]][y + dirs[j+1]] == 1)
            {
              if ((data[x + 4*dirs[j]][y + 4*dirs[j+1]] & 3) == 0)
                break;
              if ((data[x + 4*dirs[j]][y + 4*dirs[j+1]] == 1 && msys_rand() % 2))
                break;
            }
      if (i == 4) break;
      data[x + dirs[j]][y + dirs[j+1]] = 0;
      data[x + 2*dirs[j]][y + 2*dirs[j+1]] = 0;
      data[x + 3*dirs[j]][y + 3*dirs[j+1]] = 0;
      x += dirs[j] * 4;
      y += dirs[j+1] * 4;
      if (data[x][y] != 0) break;
    }
}

// Nettoie la matrice : vire les cases non reliées
void City::prune()
{
  for (int i = 2; i < width - 1; i += 2)
    for (int j = 2; j < height - 1; j += 2)
      if (data[i-1][j] == 1 &&
          data[i+1][j] == 1 &&
          data[i][j-1] == 1 &&
          data[i][j+1] == 1)
        data[i][j] = 1;
}

// La hauteur du terrain dépend de sa distance à la route la plus proche
// (c'est un parcours largeur)
void City::computeHeightMap(void)
{
  Queue<int> mem(100000);
  const float undef = 1000.f;

  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
      {
        if (data[i][j] != 1) {
          mem.push(i * 100 + j);
          hmap[i][j] = 0.f;
        } else
          hmap[i][j] = undef + 1.f;
      }

  while (!mem.empty())
    {
      const int top = mem.pop();
      int x = top / 100;
      int y = top % 100;
      if (x <= 1 || y <= 1 || x >= width-1 || y >= height-1)
        continue;
      if (hmap[x][y] < undef && hmap[x][y] > 0.01f) continue;
      float a = min(min(hmap[x+1][y], hmap[x][y+1]),
                    min(hmap[x-1][y], hmap[x][y-1])) + heightVariation;

      // pas de colline en ville
      if (x <= hRoads[numHRoads - 1])
        a = heightVariation;
      if (hmap[x][y] >= undef)
        hmap[x][y] = a;
      if (hmap[x+1][y] >= undef)
        mem.push((x+1) * 100 + y);
      if (hmap[x-1][y] >= undef)
        mem.push((x-1) * 100 + y);
      if (hmap[x][y+1] >= undef)
        mem.push(x * 100 + (y+1));
      if (hmap[x][y-1] >= undef)
        mem.push(x * 100 + (y-1));
    }
}

void City::addHouse(int coef_x, int coef_y, int x, int y)
{
  // nombre de routes adjacentes
  const int neighbours = data[x-1][y]%2 + data[x+1][y]%2 + data[x][y-1]%2 + data[x-1][y+1]%2;
  const float r = 1.1f + msys_frand() / 10.f;

  glPushMatrix();
  glTranslatef(coef_x * r, hmap[x][y], coef_y * r);
  int dist = (40 - x) * (40 - x) + (30 - y) * (30 - y);
  const int building = 101 - min(dist / 10, 100) + neighbours;

  // Code dégueulasse et débile, mais il faut garder l'ancien comportement
  // buggué, tout en évitant le comportement indéterminé de l'ancien code !
  int b_height;
  int fst = msys_rand() % 3;
  int snd = (msys_rand() % building) / 15;
  if (fst > snd)
    b_height = msys_rand() % 3;
  else
    b_height = (msys_rand() % building) / 15;

  //int b_height = max(fst, snd);
  int id = msys_rand();
  if (b_height > 2 && msys_rand() % 3 == 0)
  {
    glScalef(0.02f, 0.02f, 0.02f);
    Building::addRoundTowerToList(renderLists_, (float)b_height*40, id);
  }
  else
  {
    const float rnd = msys_frand() / 10.f + 0.9f;
    glScalef(rnd, rnd, rnd);

    Anim::id anim = Anim::none;

    if ((x == 31 && y == 23) || (x + coef_x == 27 && y + coef_y == 47))
    {
      glPopMatrix();
      return;
    }

    for (int i = 0; i < b_height; i++)
    {
      Building::stackUp(renderLists_, 1.f, id, false);
    }
    glTranslatef(0, -1.f, 0);
    if (b_height > 0)
    {
      Building::stackUp(renderLists_, 1.f, id, true);
    }
  }
  glPopMatrix();
}

void City::addCustomHouses(RenderList & renderList)
{
  const float pavementHeight = 0.01f;
  {
    glPushMatrix();
    glTranslatef(0.5f * 31.f, 0.22f, 0.5f * 23.f);
    glScalef(0.4f, 0.4f, 0.4f);

    Renderable floor1(END_DATE, Shader::bump, cityGrowingBuildingDate);
    floor1.setAnimation(Anim::growingBuilding, cityGrowingBuildingDate);
    floor1.setTextures(Texture::windows1, Texture::windows1Bump, Texture::windows1Specular);
    renderList.add(floor1);

    glTranslatef(0, 1.f, 0);
    Renderable floor2(END_DATE, Shader::bump, cityGrowingBuildingDate);
    floor2.setAnimation(Anim::growingBuilding, cityGrowingBuildingDate);
    floor2.setTextures(Texture::windows1, Texture::windows1Bump, Texture::windows1Specular);
    renderList.add(floor2);

    glPopMatrix();
  }

  {
    glPushMatrix();
    glTranslatef(0.5f * 27.f, 0, 0.5f * 47.f);
    glScalef(0.4f, 0.4f, 0.4f);
    Building::pacmanHouse(renderList, START_DATE, END_DATE);
    glPopMatrix();
  }
}

void City::generateMeshes()
{
  for (int i = 2; i < width - 1; i++)
    for (int j = 2; j < height - 1; j++)
    {
      const vector3f p = {i*1.0f, hmap[i][j], j*1.0f};
      const bool x1face = hmap[i - 1][j] < p.y;
      const bool x2face =  hmap[i + 1][j] < p.y;
      const bool y1face = false;
      const bool y2face = true;
      const bool z1face = hmap[i][j - 1] < p.y;
      const bool z2face = hmap[i][j + 1] < p.y;

      if (data[i][j] % 2 == 1)
      {
        if (data[i-1][j-1] == 1 &&
            data[i+1][j+1] == 1 &&
            data[i+1][j-1] == 1 &&
            data[i-1][j+1] == 1)
        {
          const vector3f color = getGrassColor(i, j);
          VBO::addCube(groundChunk_, p, color,
		       x1face, x2face, y1face, y2face, z1face, z2face);
        }
        else
          VBO::addCube(pavementChunk_, p, 1.f,
		       x1face, x2face, y1face, y2face, z1face, z2face);
      }
      else if (data[i-1][j] % 2 == 0 && data[i+1][j] % 2 == 0 && data[i][j-1] % 2 == 0 && data[i][j+1] % 2 == 0)
	VBO::addCube(streetOChunk_, p, 1.f,
		     x1face, x2face, y1face, y2face, z1face, z2face);
      else if (data[i-1][j] % 2 == 0 && data[i+1][j] % 2 == 0)
	VBO::addCube(streetHChunk_, p, 1.f,
		     x1face, x2face, y1face, y2face, z1face, z2face);
      else if (data[i][j-1] % 2 == 0 && data[i][j+1] % 2 == 0)
	VBO::addCube(streetVChunk_, p, 1.f,
		     x1face, x2face, y1face, y2face, z1face, z2face);
      else
	VBO::addCube(streetOChunk_, p, 1.f,
		     x1face, x2face, y1face, y2face, z1face, z2face);
    }

  roadToFactoryMesh();
  streetLightsMesh();
  buildingElementsMeshes();

  DBG("Terrain de la ville");
  DBG("Herbe : %d sommets", groundChunk_.size);
  DBG("Route : %d sommets", streetOChunk_.size + streetHChunk_.size + streetVChunk_.size);
  DBG("Paves : %d sommets", pavementChunk_.size);
}

typedef struct
{
  VBO::id VBOid;
  Texture::id textureId;
  Texture::id bumpTextureId;
  Texture::id specularTextureId;
} buildingsPartDesc;

void City::addCity(RenderList & renderList)
{
  const float pavementHeight = 0.01f;
  glPushMatrix();
  glTranslatef(0.f, -0.25f, 0.f);


  Renderable ground(END_DATE, Shader::grass, START_DATE, 0.5f,
		    groundChunk_.size, VBO::cityGround);
  ground.setTextures(Texture::grass, Texture::grassBump, Texture::grassSpecular);
  renderList.add(ground);


  Renderable pavement(END_DATE, Shader::bump, START_DATE, 0.5f,
		      pavementChunk_.size, VBO::cityPavement);
  pavement.setTextures(Texture::pavement,
		       Texture::pavementBump,
		       Texture::pavementSpecular);
  renderList.add(pavement);


  glTranslatef(0.f, pavementHeight, 0.f);
  Renderable hStreets(END_DATE, Shader::bump, START_DATE, 0.5f,
		      streetHChunk_.size, VBO::cityStreetH);
  hStreets.setTextures(Texture::roadH, Texture::roadHBump, Texture::roadHSpecular);
  renderList.add(hStreets);

  Renderable vStreets(END_DATE, Shader::bump, START_DATE, 0.5f,
		      streetVChunk_.size, VBO::cityStreetV);
  vStreets.setTextures(Texture::roadV,Texture::roadVBump,Texture::roadVSpecular);
  renderList.add(vStreets);

  Renderable oStreets(END_DATE, Shader::bump, START_DATE, 0.5f,
		      streetOChunk_.size, VBO::cityStreetO);
  oStreets.setTextures(Texture::roadO,Texture::roadOBump,Texture::roadOSpecular);
  renderList.add(oStreets);

  glPopMatrix();

  Renderable lights(END_DATE, Shader::sodium, START_DATE, 0.5f,
		    lightChunk_.size, VBO::cityLights);
  lights.setTextures(Texture::glowingSodium);
  renderList.add(lights);


  // Bâtiments
  const buildingsPartDesc buildingsPartDescs[] =
  {
    {VBO::buildingsWalls0, Texture::wall1, Texture::wall1Bump, Texture::wall1Specular},
    {VBO::buildingsWalls1, Texture::windows1, Texture::windows1Bump, Texture::windows1Specular},
    {VBO::buildingsWalls2, Texture::windows2, Texture::windows2Bump, Texture::windows2Specular},
    {VBO::buildingsWalls3, Texture::windows3, Texture::windows2Bump, Texture::windows2Specular},
    {VBO::buildingsWalls4, Texture::windows4, Texture::windows4Bump, Texture::windows4Specular},
    {VBO::buildingsRoofs1, Texture::roof1, Texture::roof1Bump, Texture::defaultSpecular},
    {VBO::buildingsRoofs2, Texture::roof2, Texture::roof2Bump, Texture::defaultSpecular},
    {VBO::buildingsRoofs3, Texture::roof3, Texture::roof2Bump, Texture::defaultSpecular},
    {VBO::buildingsRoofs4, Texture::roof4, Texture::roof4Bump, Texture::defaultSpecular}
  };

  const unsigned int numberOfKinds = sizeof(buildingsPartDescs) / sizeof(buildingsPartDesc);
  for (unsigned int i = 0; i < numberOfKinds; ++i)
  {
    const buildingsPartDesc & desc = buildingsPartDescs[i];
    Renderable buildingsPart(END_DATE, Shader::building, START_DATE,
			     1.f, buildingsChunks_[i]->size, desc.VBOid);
    buildingsPart.setTextures(desc.textureId, desc.bumpTextureId, desc.specularTextureId);
    buildingsPart.setId(i == 1 ? 1 : 0); // indique si la texture a 4 fenetres, au lieu des 3 standards.
    renderList.add(buildingsPart);
  }

  addCustomHouses(renderList);
}

void City::addCar(RenderList & renderList, int x, bool dir)
{
  float shift = 0.05f;
  while (shift < 1.f)
  {
    Texture::id carColor;
    switch (msys_rand() % 6)
    {
    case 0: carColor = Texture::whiteCar; break;
    case 1: carColor = Texture::blueCar; break;
    case 2: carColor = Texture::blackCar; break;
    case 3: carColor = Texture::greyCar; break;
    case 4: carColor = Texture::redCar; break;
    case 5: carColor = Texture::taxiCar; break;
    default: assert(false);
    }

    glPushMatrix();
    const float deviation = -msys_frand()/15.f + (dir ? -0.07f : 0.15f);
    glTranslatef((float)x/2.f + deviation, 0.05f, dir ? (float)height/2 : 0.f);

    Renderable car(cityEndCarsDate, Shader::bump, START_DATE, 0.05f);
    car.setTextures(carColor, Texture::carBump);
    car.setAnimation(dir ? Anim::car2 : Anim::car);
    car.setAnimationStepShift(shift);
    renderList.add(car);
    glPopMatrix();

    // espace entre deux voitures de la même rue
    shift += msys_frand() / 10.f + 0.02f;
  }
}

void City::roadToFactoryMesh()
{
  for (int i = width - 2; i < 2 * width; i++)
    for (int j = 3; j < height - 2; j++)
    {
      glPushMatrix();
      glScalef(0.5f, 0.5f, 0.5f);
      int kind = data[width-2][j];
      float h = hmap[width-3][j];
      vector3f p = {(float)i, h, (float)j};
      if ((kind % 2) == 0) // route
        VBO::addCube(streetHChunk_, p);
      else if (i == 140 && j > 34) // route perpendiculaire, vers l'usine
	VBO::addCube(streetVChunk_, p);
      else // herbe
	{
	  p.y += msys_sfrand() / 20.f;
          vector3f color = getGrassColor(i, j);
          VBO::addCube(groundChunk_, p, color,
		       hmap[width-3][j] < p.y,
		       hmap[width-3][j] < p.y,
		       false, true,
		       j > 3 && hmap[width-3][j - 1] < p.y,
		       j < height - 3 && hmap[width-3][j + 1] < p.y);
	}
      glPopMatrix();
    }
}

static void statue(RenderList & renderList)
{
  Renderable socle(END_DATE, Shader::bump, START_DATE);
  socle.setTextures(Texture::concrete, Texture::concreteBump, Texture::concreteSpecular);
  renderList.add(socle);

  glTranslatef(0.f, 0.7f, 0.f);
  glScalef(0.5f, 0.5f, 0.5f);
  Renderable cube1(END_DATE, Shader::bump, START_DATE);
  cube1.setTextures(Texture::statue, Texture::defaultBump, Texture::statueSpecular);
  renderList.add(cube1);

  glTranslatef(0.f, 2.f, 0.f);
  Renderable cube2(END_DATE, Shader::bump, START_DATE);
  cube2.setTextures(Texture::statue, Texture::defaultBump, Texture::statueSpecular);
  renderList.add(cube2);

  glTranslatef(0.f, -1.f, 0.f);
  glRotatef(45.f, 1.f, 0.f, 1.f);
  glScalef(0.6f, 0.6f, 0.6f);
  Renderable cube3(END_DATE, Shader::bump, START_DATE);
  cube3.setTextures(Texture::statue, Texture::defaultBump, Texture::statueSpecular);
  renderList.add(cube3);
}

void City::addStatues(RenderList & renderList)
{
  glPushMatrix();
  glTranslatef(14.5f, 0.1f, 11.5f);
  glScalef(0.25f, 0.25f, 0.25f);
  statue(renderList);
  glPopMatrix();
}

static void addPharmacy(RenderList & renderList)
{
  glPushMatrix();
  glTranslatef(19.75f, 0.25f, 15.75f);
  glScalef(0.06f, 0.06f, 0.06f);
  Renderable cube(END_DATE, Shader::pharmacy, START_DATE);
  cube.setTextures(Texture::pharmacy);
  renderList.add(cube);
  glPopMatrix();
}

void City::streetLightsMesh()
{
  for (int j = 5; j < height - 5; j+=2)
    for (int i = 5; i < width - 5; i+=2) {
      if (data[i+1][j] % 2 == 0 && msys_rand()%3 == 0)
      {
        const vector3f p = {(float)i + 0.5f, 0.04f, (float)j + 0.5f};
        VBO::addCube(lightChunk_, p, 0.1f);
      }
      if (data[i-1][j] % 2 == 0 && msys_rand()%3 == 0)
      {
        const vector3f p = {(float)i - 0.5f, 0.04f, (float)j - 0.5f};
        VBO::addCube(lightChunk_, p, 0.1f);
      }
    }

  /*
  // Test avec une lampe par case rue
  for (int i = 2; i < width - 1; i++)
    for (int j = 2; j < height - 1; j++)
      if (data[i][j] % 2 != 1)
      {
	const vector3f p = {i*1.0f + 0.5f, hmap[i][j] + 0.5f, j*1.0f + 0.5f};
	VBO::addCube(lightChunk_, p, 0.05f);
      }
  */
}

void City::buildingElementsMeshes()
{
  const float pavementHeight = 0.01f;

  msys_srand(0);
  for (int i = 3; i < width - 2; i += 2)
    for (int j = 3; j < height - 2; j += 2)
    {
      if (data[i-1][j-1] == 1 &&
          data[i+1][j+1] == 1 &&
          data[i+1][j-1] == 1 &&
          data[i-1][j+1] == 1) continue;

        glPushMatrix();
        glTranslatef((float)i/2, pavementHeight, (float) j/2);
        glScalef(0.4f, 0.4f, 0.4f);

        addHouse(0, 0, i, j);
        if (data[i][j+1] == 1)
          addHouse(0, 1, i, j);
        if (data[i+1][j] == 1)
          addHouse(1, 0, i, j);
        if (data[i+1][j+1] == 1)
          addHouse(1, 1, i, j);

        glPopMatrix();
    }

  // On a nos bâtiments qui ont été générés, avec les morceaux dans
  // les différentes renderlists en fonction de la texture ;
  // maintenant on en fait des VBO

  // Le mur sans fenêtre
  VBO::addFromRenderList(*buildingsChunks_[0], *renderLists_[0], true, true, false, false, true, true);

  // Les murs avec fenêtre
  VBO::addFromRenderList(*buildingsChunks_[1], *renderLists_[1], true, true, false, false, true, true);
  VBO::addFromRenderList(*buildingsChunks_[2], *renderLists_[2], true, true, false, false, true, true);
  VBO::addFromRenderList(*buildingsChunks_[3], *renderLists_[3], true, true, false, false, true, true);
  VBO::addFromRenderList(*buildingsChunks_[4], *renderLists_[4], true, true, false, false, true, true);

  // Les toits
  VBO::addFromRenderList(*buildingsChunks_[5], *renderLists_[5], false, false, false, true, false, false);
  VBO::addFromRenderList(*buildingsChunks_[6], *renderLists_[6], false, false, false, true, false, false);
  VBO::addFromRenderList(*buildingsChunks_[7], *renderLists_[7], false, false, false, true, false, false);
  VBO::addFromRenderList(*buildingsChunks_[8], *renderLists_[8], false, false, false, true, false, false);

  DBG("Immeubles de la ville");
  DBG("Mur 1     : %d sommets", buildingsChunks_[0]->size);

  DBG("Fenêtre 1 : %d sommets", buildingsChunks_[1]->size);
  DBG("Fenêtre 2 : %d sommets", buildingsChunks_[2]->size);
  DBG("Fenêtre 3 : %d sommets", buildingsChunks_[3]->size);
  DBG("Fenêtre 4 : %d sommets", buildingsChunks_[4]->size);

  DBG("Toit 1    : %d sommets", buildingsChunks_[5]->size);
  DBG("Toit 2    : %d sommets", buildingsChunks_[6]->size);
  DBG("Toit 3    : %d sommets", buildingsChunks_[7]->size);
  DBG("Toit 4    : %d sommets", buildingsChunks_[8]->size);

  DBG("Total     : %d sommets",
      buildingsChunks_[0]->size +
      buildingsChunks_[1]->size +
      buildingsChunks_[2]->size +
      buildingsChunks_[3]->size +
      buildingsChunks_[4]->size +
      buildingsChunks_[5]->size +
      buildingsChunks_[6]->size +
      buildingsChunks_[7]->size +
      buildingsChunks_[8]->size);
}

static void trafficLight(RenderList & renderList, float x, float z, bool greenOn)
{
  glPushMatrix();
  glTranslatef(x, 0.02f, z);
  glScalef(0.05f, 0.05f, 0.05f);

  glTranslatef(0.f, 0.5f, 0.f);
  Renderable green(END_DATE, Shader::bump, START_DATE);
  green.setTextures(greenOn ? Texture::greenLightOn : Texture::greenLightOff);
  renderList.add(green);

  glTranslatef(0.f, 1.f, 0.f);
  Renderable red(END_DATE, Shader::bump, START_DATE);
  red.setTextures(greenOn ? Texture::redLightOff : Texture::redLightOn);
  renderList.add(red);

  glPopMatrix();
}

static void addLights(RenderList & renderList)
{
  trafficLight(renderList, 20.2f, 14.5f, true);
  trafficLight(renderList, 19.6f, 15.2f, false);

  trafficLight(renderList, 16.2f, 11.7f, false);
  trafficLight(renderList, 14.7f, 24.7f, false);

  trafficLight(renderList, 15.3f, 20.6f, true);
  trafficLight(renderList, 22.3f, 8.2f, false);
  trafficLight(renderList, 24.2f, 5.7f, false);
  trafficLight(renderList, 24.2f, 11.7f, false);
}

static void chimney(RenderList & renderList)
{
  glPushMatrix();
  for (unsigned int i = 0; i < 10; ++i)
  {
    glTranslatef(0, 0.5f, 0);
    Renderable block(END_DATE, Shader::bump, START_DATE); // FIXME : id = 3 ?
    block.setTextures(Texture::wall1, Texture::wall1Bump, Texture::wall1Specular);
    renderList.add(block);
    glTranslatef(0, 0.5f, 0);
    glScalef(0.95f, 0.95f, 0.95f);
  }
  glTranslatef(0, 1.f, 0);
  for (unsigned int i = 0; i < 10; ++i)
  {
    Renderable smoke(END_DATE, Shader::default, START_DATE);
    smoke.setTextures(Texture::smoke);
    smoke.setAnimation(Anim::smoke);
    smoke.setAnimationStepShift(0.1f * (i + 1 + msys_sfrand()));
    renderList.add(smoke);
  }
  glPopMatrix();
}

void City::addFactory(RenderList & renderList)
{
  /*
  buildSmokeAnim();
  */

  // Usine exterieure
  glPushMatrix();
  glScalef(0.5f, 0.5f, 0.5f);
  glTranslatef(140.f, hmap[width-2][56], 56.f);
  glScalef(5.f, 5.f, 5.f);
  glTranslatef(0, 0.5f, 0);
  Renderable building(END_DATE, Shader::bump, START_DATE); // FIXME : id = 3 ?
  building.setTextures(Texture::wall1, Texture::wall1Bump, Texture::wall1Specular);
  renderList.add(building);

  // Cheminées
  glTranslatef(-0.3f, 0.5f, -0.3f);
  glScalef(0.1f, 0.1f, 0.1f);
  chimney(renderList);
  glTranslatef(2.f, 0, 0);
  chimney(renderList);

  glPopMatrix();
}

static void doRotation(float* pos, float* old)
{
  const float difx = old[0] - pos[0];
  const float dify = old[1] - pos[1];
  const float difz = old[2] - pos[2];

  glRotatef(RAD_TO_DEG * msys_atan2f(difx, difz), 0.f, 1.f, 0.f);
  glRotatef(RAD_TO_DEG * -msys_sinf(dify), 1.f, 0.f, 0.f);
}

void City::addRailsCase(RenderList & renderList, float* pos, float* old, int n)
{
  const bool pillar = n % 5 == 0;
  const float x = pos[0];
  const float y = pos[1];
  const float z = pos[2];
  const float fallHeight = 30.f;

  glPushMatrix();
  glScalef(0.5f, 0.5f, 0.5f);
  const float pillar_size = 0.5;
  glTranslatef(x, y + pillar_size * 1.5f + fallHeight, z);

  doRotation(pos, old);

  Renderable block(END_DATE, Shader::bump, START_DATE, pillar_size);
  block.setTextures(Texture::concrete, Texture::concreteBump, Texture::concreteSpecular);
  block.setAnimation(Anim::rails, START_DATE + 60 * n - 3000);
  renderList.add(block);

  if (pillar)
  {
    glTranslatef(0, - pillar_size - fallHeight, 0);
    Renderable pillarBlock(END_DATE, Shader::bump, START_DATE, pillar_size);
    pillarBlock.setTextures(Texture::concrete, Texture::concreteBump, Texture::concreteSpecular);
    renderList.add(pillarBlock);
  }
  glPopMatrix();
}

float railsDist(float p1[3], float p2[3])
{
  float d1 = p1[0] - p2[0];
  float d2 = p1[2] - p2[2];
  return d1 * d1 + d2 * d2;
}

void City::addRails(RenderList & renderList)
{
  float old[3];
  spline(rails_data, rails_points, 3, 0.f, old);
  int count = 0;
  for (float k = 0.f; k < 170.f; k += 0.1f)
  {
    float res[3];
    spline(rails_data, rails_points, 3, k, res);
    float dist = railsDist(res, old);

    if (dist >= 0.3f) {
      addRailsCase(renderList, res, old, count);
      old[0] = res[0];
      old[1] = res[1];
      old[2] = res[2];
      count++;
    }
  }
  count = count + 0;
}

void City::addTrain(RenderList & renderList)
{
  const int skip = 5;
  const int wagons = 6;
  const float wagon_width = 0.25f;
  const float cube_width = 0.1f;

  /*
  Anim::Anim * animWagon = new Anim::Anim(170 / skip, 50000, 0);
  Anim::Anim * animCube1 = new Anim::Anim(170 / skip, 50000, 0);
  Anim::Anim * animCube2 = new Anim::Anim(170 / skip, 50000, 0);
  Anim::Anim * animCube3 = new Anim::Anim(170 / skip, 50000, 0);
  Anim::Anim * animCube4 = new Anim::Anim(170 / skip, 50000, 0);
  glPushMatrix();
  glLoadIdentity();
  for (int i = 0; i < 170; i += skip)
    {
      const float date = i + skip >= 170 ? 1.f : i / 170.f;
      glPushMatrix();

      float pos[3];
      float old[3];

      spline(rails, rails_points, 3, (float)i, pos);
      spline(rails, rails_points, 3, (float)i-2.f, old);

      glTranslatef(pos[0] * 0.5f, pos[1] * 0.5f, pos[2] * 0.5f);
      doRotation(pos, old);
      animWagon->add(Anim::Keyframe(date, 1.f, true));

      glTranslatef(0.f, 0.4f + wagon_width + wagon_width/2.f + cube_width/2.f, 0.f);
      glPushMatrix();
      glTranslatef(-wagon_width/4.f, 0.f, -wagon_width/4.f);
      animCube1->add(Anim::Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(wagon_width/4.f, 0.f, -wagon_width/4.f);
      animCube2->add(Anim::Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(-wagon_width/4.f, 0.f, wagon_width/4.f);
      animCube3->add(Anim::Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(wagon_width/4.f, 0.f, wagon_width/4.f);
      animCube4->add(Anim::Keyframe(date, 1.f, true));
      glPopMatrix();

      glPopMatrix();
    }
  */

  // La locomotive
  {
    glPushMatrix();

    glTranslatef(0.f, 0.4f, 0.f);
    Renderable bogie(END_DATE, Shader::default, START_DATE, 0.35f);
    bogie.setTextures(Texture::container2);
    bogie.setAnimation(Anim::wagon, TRAIN_LEAVE_DATE);
    renderList.add(bogie);

    glTranslatef(0.f, 0.36f, 0.f);
    Renderable locomotive(END_DATE, Shader::default, START_DATE, 0.35f);
    locomotive.setTextures(Texture::container);
    locomotive.setAnimation(Anim::wagon, TRAIN_LEAVE_DATE);
    renderList.add(locomotive);

    glPopMatrix();
  }

  for (int i = 1; i <= wagons; i++)
  {
    const float shift = i / 140.f;

    glPushMatrix();
    glTranslatef(0.f, 0.4f, 0.f);

    // Le socle - wagon
    Renderable bogie(END_DATE, Shader::bump, START_DATE, 0.35f);
    bogie.setTextures(Texture::container2);
    bogie.setAnimation(Anim::wagon, TRAIN_LEAVE_DATE);
    bogie.setAnimationStepShift(shift);
    renderList.add(bogie);

    glTranslatef(0.f, wagon_width, 0.f);

    // les cubes
    {
      Renderable cube(END_DATE, Shader::color, START_DATE,
		      cube_width);
      cube.setId(Factory::notSoRandom(4*i+1));
      cube.setAnimation(Anim::wagonCube1, TRAIN_LEAVE_DATE);
      cube.setAnimationStepShift(shift);
      renderList.add(cube);
    }
    {
      Renderable cube(END_DATE, Shader::color, START_DATE,
		      cube_width);
      cube.setId(Factory::notSoRandom(4*i+2));
      cube.setAnimation(Anim::wagonCube2, TRAIN_LEAVE_DATE);
      cube.setAnimationStepShift(shift);
      renderList.add(cube);
    }
    {
      Renderable cube(END_DATE, Shader::color, START_DATE,
		      cube_width);
      cube.setId(Factory::notSoRandom(4*i+3));
      cube.setAnimation(Anim::wagonCube3, TRAIN_LEAVE_DATE);
      cube.setAnimationStepShift(shift);
      renderList.add(cube);
    }
    if (i == 3) // LE cube
    {
      Renderable cube(END_DATE, Shader::color, START_DATE, cube_width,
		      Cube::numberOfVertices, VBO::theCube);
      cube.setAnimation(Anim::wagonCube4, TRAIN_LEAVE_DATE);
      cube.setAnimationStepShift(shift);
      renderList.add(cube);
    }
    else
    {
      Renderable cube(END_DATE, Shader::color, START_DATE,
		      cube_width);
      cube.setId(Factory::notSoRandom(4*i+4));
      cube.setAnimation(Anim::wagonCube4, TRAIN_LEAVE_DATE);
      cube.setAnimationStepShift(shift);
      renderList.add(cube);
    }
    glPopMatrix();
  }
}

static void addTetrisPiece(RenderList & renderList, int kind, int posx, int posy, int time)
{
  const int nbPieces = 3;
  // la position (0,0) est le coin en bas à gauche de la piece
  static const char pieces[nbPieces][4 * 2] = {
    {0,2, 0,1, 0,0, 1,0}, // L
    {0,0, 1,1, 1,0, 2,0}, // _|_
    {0,0, 1,0, 1,1, 2,1}  // S
  };

  const unsigned int offsetAlbedo = kind + 1;
  const unsigned int offsetShape = offsetAlbedo > 1 ? offsetAlbedo - 1 : offsetAlbedo;

  for (int i = 0; i < 4; i++)
    {
      float x = 38.f - (posx + pieces[kind][i * 2])     * 0.4f;
      float y = 30.f + (posy + pieces[kind][i * 2 + 1]) * 0.4f;
      glPushMatrix();
      glTranslatef(x, 0.2f + 0.06f + y, 16.52f);
      Renderable piece(END_DATE, Shader::bump, START_DATE, 0.4f);
      piece.setTextures((Texture::id)(Texture::windows1 + offsetAlbedo),
			  (Texture::id)(Texture::windows1Bump + offsetShape),
			  (Texture::id)(Texture::windows1Specular + offsetShape));
      piece.setAnimation(Anim::tetris, time);
      renderList.add(piece);
      glPopMatrix();
    }
}

static void addTetris(RenderList & renderList)
{
  addTetrisPiece(renderList, 0, 0, 0, START_DATE - 2000);
  addTetrisPiece(renderList, 1, 2, 0, tetrisDate);
  addTetrisPiece(renderList, 2, 1, 1, tetrisDate + 3000);
}

static void addWalkingCube(RenderList & renderList)
{
  const float size = 0.05f;
  const float shift = -0.04f; // decalage pour marcher le long du trottoire

  // Pres de la statue
  {
  glPushMatrix();
  glTranslatef(15.25f + shift, 0.042f, 9.25f);
  glScalef(size, size, size);
  Renderable cube(END_DATE, Shader::color, START_DATE, 1.f,
	      Cube::numberOfVertices, VBO::theCube);
  cube.setAnimation(Anim::cityWalk1, START_DATE + 48000);
  renderList.add(cube);
  glPopMatrix();
  }

  //
  {
  glPushMatrix();
  glTranslatef(19.75f - shift, 0.042f, 13.25f);
  glScalef(size, size, size);
  Renderable cube(END_DATE, Shader::color, START_DATE, 1.f,
	      Cube::numberOfVertices, VBO::theCube);
  cube.setAnimation(Anim::cityWalk1, START_DATE + 40000);
  renderList.add(cube);
  glPopMatrix();
  }
}

void City::generate(RenderList & renderList)
{
  glPushMatrix();
  addCity(renderList);
  addFactory(renderList);
  addRails(renderList);
  addTrain(renderList);
  addStatues(renderList);
  addLights(renderList);
  addPharmacy(renderList);
  addTetris(renderList);
  addWalkingCube(renderList);

  for (int dir = 0; dir < 2; dir++)
  {
    addCar(renderList, 20, dir == 0);
    addCar(renderList, 30, dir == 0);
    addCar(renderList, 40, dir == 0);
  }

  glPopMatrix();
}
