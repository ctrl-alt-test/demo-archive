//
// Intérieur de l'usine
//

#include "factory.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "interpolation.hh"
#include "shaderid.hh"
#include "shaders.hh"
#include "textureid.hh"
#include "timing.hh"
#include "vbobuild.hh"
#include "vboid.hh"

#include "tweakval.h"

#define START_DATE	worldStartDate
#define END_DATE	worldEndDate

#define WTILES 50
#define LTILES 30
#define TILE_SIZE 5
#define TILE_QUADS 5
#define VDISTANCE 20.f

namespace Factory
{
  Array<vertex> floorChunk;
  Array<vertex> buildingsChunk;

  static void createFloorAndCeil(RenderList & renderList);

  // --------------------------------------------------------------------------

  //
  // Description d'un bâtiment
  //
  typedef struct
  {
    float x;
    float y;
    float z;
    float width;
    float length;
    float height;
    float bend;
    float xoffset;
    float yoffset;
  } buildingDesc;

  Array<buildingDesc> buildings;

  static float randomHeight(float seed)
  {
    return 0.1f + 1.9f * seed;
  }

  static float randomSquareness(float seed)
  {
    const float baseRand = msys_frand();
    const float shapedRand = (0.5f + baseRand) * msys_powf(seed, 2.3f);
    return 1.f + 5.f * shapedRand;
  }

  //
  // Génération des formes de bâtiments
  //
  static void createBuildingsDescription()
  {
    msys_srand(13); // 5 pas mal

    buildings.init(WTILES * LTILES);
    for (unsigned int j = 0; j < LTILES; ++j)
      for (unsigned int i = 0; i < WTILES; ++i)
      {
	buildingDesc building;

	const float seed = msys_frand();
	building.height = randomHeight(seed);
	const float squareness = randomSquareness(seed);
	const float volume = 1.f + msys_frand();

	const float surface = volume / building.height;
	// w * l = s
	// w = a * l
	// w = sqr(s * a)
	if (msys_rand() % 2 == 0)
	{
	  building.width = msys_sqrtf(surface * squareness);
	  building.length = surface / building.width;
	}
	else
	{
	  building.length = msys_sqrtf(surface * squareness);
	  building.width = surface / building.length;
	}

	// Position
	building.x = float(TILE_SIZE * i) + 0.5f * float(TILE_SIZE)
	  - 0.5f * float(TILE_SIZE * WTILES) + 1.1f * msys_sfrand();

	building.y = 0.5f * building.height;

	building.z = float(TILE_SIZE * j) + 0.5f * float(TILE_SIZE) -
	  0.5f * float(TILE_SIZE * LTILES) + 1.1f * msys_sfrand();

	building.bend = msys_sfrand();
	building.xoffset = msys_sfrand();
	building.yoffset = msys_sfrand();

	// FIXME : forme autre que parallèlépipédique

	buildings.add(building);
      }
  }

  //
  // Génération des sommets d'un bâtiment à partir de sa description
  //
  static void generateBuildingMesh(const buildingDesc & building,
				   Array<vertex> & vertices)
  {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(building.x, building.y, building.z);
    glScalef(building.width, building.height, building.length);
    Renderable cube(END_DATE, Shader::parallax, START_DATE);
    glPopMatrix();

    // Bricolage d'une vtable à la main
    Array<vertex> initialized;
    msys_memcpy(&vertices, &initialized, sizeof(Array<vertex>));

    vertices.init(cube.numberOfVertices());
    for (unsigned int j = 0; j < cube.numberOfVertices(); ++j)
    {
      vertex v = Cube::vertices[j];
      if (v.y < 0)
      {
	v.r *= 0.15f;
	v.g *= 0.15f;
	v.b *= 0.15f;
      }
      // Coordonnées de texture
      v.u = building.xoffset + 0.25f * (v.x * building.width + v.z * building.length);
      v.v = building.yoffset + 0.25f * v.y * building.height;

      // Cas du plafond
      if (j >= 8 && j < 12)
      {
	v.u = building.xoffset + 0.25f * v.x * building.width;
	v.v = building.yoffset + 0.25f * (v.y * building.height + (float)(v.z > 0) * building.length);
      }

      vertices.add(transformedVertex(cube.viewMatrix(), v));
    }

    IFDBG(initialized.init(2)); // Juste pour éviter l'assert du delete
  }

  //
  // Génération des meshs de bâtiments, à partir des descriptions
  //
  static void generateBuildingsMeshes()
  {
    Array<Array<vertex> > meshes(WTILES * LTILES);
    meshes.size = meshes.max_size;

    // Création des différents bâtiments
    glPushMatrix();
    glLoadIdentity();
    for (unsigned int j = 0; j < LTILES; ++j)
      for (unsigned int i = 0; i < WTILES; ++i)
      {
	const buildingDesc & building = buildings[i + WTILES * j];
	Array<vertex> & vertices = meshes[i + WTILES * j];
	generateBuildingMesh(building, vertices);
      }
    glPopMatrix();

    // Comptage du total de sommets
    unsigned int numberOfVertices = 0;
    for (int i = 0; i < meshes.size; ++i)
    {
      numberOfVertices += meshes[i].size;
    }
    buildingsChunk.init(numberOfVertices);

    // Remplissage du tableau de tous les sommets
    for (int i = 0; i < meshes.size; ++i)
    {
      const Array<vertex> & mesh = meshes[i];
      for (int j = 0; j < mesh.size; ++j)
      {
	buildingsChunk.add(mesh[j]);
      }
    }
    assert(buildingsChunk.size > WTILES * LTILES * 10); // Le 10 est au pif
  }

  // --------------------------------------------------------------------------
  Array<int> groundTrails;
  static void generateGroundTrails()
  {
    groundTrails.init(WTILES * LTILES);
    const float p_trail = 1.f / 2.f;
    const float p_continue = 3.f / 4.f;
    const float p_after_trail = p_trail * p_continue;
    const float p_after_empty = p_trail * (1.f - p_continue);

    for (unsigned int j = 0; j < LTILES; ++j)
      for (unsigned int i = 0; i < WTILES; ++i)
      {
	int trails = 0;
	const float seed1 = msys_frand();
	const float seed2 = msys_frand();

	if (0 == i)
	{
	  trails = (seed1 <= p_trail) | ((seed2 <= p_trail)<<2);
	}
	else
	{
	  const int prevTileIndex = (i - 1) + j * WTILES;

	  if ((groundTrails[prevTileIndex] & 2) != 0) // bits: .. ?.
	  {
	    // xx10 si on continue
	    // xx01 si on arrête
	    const int continueTrail = (seed1 <= p_after_trail);
	    trails |= continueTrail ? 2 : 1;
	  }
	  else
	  {
	    // xx11 si on commence
	    // xx00 si on laisse vide
	    const int startTrail = (seed1 <= p_after_empty);
	    trails |= startTrail ? 3 : 0;
	  }


	  if ((groundTrails[prevTileIndex] & 8) != 0) // bits: ?. ..
	  {
	    // 10xx si on continue
	    // 01xx si on arrête
	    const int continueTrail = (seed2 <= p_after_trail);
	    trails |= ((continueTrail ? 2 : 1)<<2);
	  }
	  else
	  {
	    // 11xx si on commence
	    // 00xx si on laisse vide
	    const int startTrail = (seed2 <= p_after_empty);
	    trails |= ((startTrail ? 3 : 0)<<2);
	  }
	}
	groundTrails.add(trails);
      }
  }

  static float groundLuminosity(float x, float z)
  {
    float luminosity = 1.f;

    for (int i = 0; i < buildings.size; ++i)
    {
      const buildingDesc & building = buildings[i];
      const float effect = 2.f * building.height;
      float dx = msys_fabsf(building.x - x) - 0.5f * building.width;
      if (dx < effect)
      {
	dx = max(0.f, dx);
	float dz = msys_fabsf(building.z - z) - 0.5f * building.length;
	if (dz < effect)
	{
	  dz = max(0.f, dz);
	  const float d = msys_sqrtf(dx * dx + dz * dz);
	  luminosity *= min(1.f, d / effect);
	}
      }
    }
    return luminosity;
  }

  //
  // Génération des coordonnées de texture du sommet d'une dalle
  //
  static void computeTexCoord(vertex & p,
			      float uOffset, float vOffset,
			      unsigned int iQuad, unsigned int jQuad)
  {
    // Attention, j est suivant u, et i est suivant v
    p.u = uOffset + (0.25f * jQuad) / TILE_QUADS;
    p.v = vOffset + (0.25f * iQuad) / TILE_QUADS;
  }

  static void computeTexCoord(vertex & a, vertex & b, vertex & c, vertex & d,
			      unsigned int iQuad, unsigned int jQuad,
			      int trails)
  {
    unsigned int uTexElement = trails & 3;
    unsigned int vTexElement = 3 - ((trails & 12)>>2);
    if (0 == vTexElement || 2 == vTexElement)
    {
      if (1 == uTexElement)
	uTexElement = 3;
      else if (3 == uTexElement)
	uTexElement = 1;
    }
    const float uOffset = 0.25f * uTexElement;
    const float vOffset = 0.25f * vTexElement;

    computeTexCoord(a, uOffset, vOffset, iQuad,     jQuad);
    computeTexCoord(b, uOffset, vOffset, iQuad,     jQuad + 1);
    computeTexCoord(c, uOffset, vOffset, iQuad + 1, jQuad + 1);
    computeTexCoord(d, uOffset, vOffset, iQuad + 1, jQuad);
  }

  //
  // Génération des sommets du sol
  //
  static void generateGroundMesh(Array<vertex> & chunk)
  {
    const float width = WTILES * TILE_SIZE;
    const float length = LTILES * TILE_SIZE;
    const float wrepeat = WTILES;
    const float lrepeat = LTILES;
    const unsigned int wquads = WTILES * TILE_QUADS;
    const unsigned int lquads = LTILES * TILE_QUADS;

    Array<vertex> vertices((wquads + 1) * (lquads + 1));

    for (unsigned int j = 0; j <= lquads; ++j)
      for (unsigned int i = 0; i <= wquads; ++i)
      {
	const float dw = i / (float)wquads;
	const float dl = j / (float)lquads;
	const float x = (dw - 0.5f) * width;
	const float z = (dl - 0.5f) * length;
	const float c = groundLuminosity(x, z);
	const float h = 0;//0.5f * msys_frand();

	vertex p = {
	  x, h, z,
	  0.f, 1.f, 0.f, // Normale, vers le haut
	  0.f, 0.f, 1.f, // Tangente, suivant l'axe z (j'ai fait mon boulet en pivotant la texture)
	  c, c, c, // Couleur, modulable pour avoir de l'ambient occlusion
	  dl * lrepeat, dw * wrepeat, 0.f, 1.f
	};
	vertices.add(p);
      }

    assert(chunk.max_size == 0);
    chunk.init(4 * wquads * lquads);

    for (unsigned int j = 0; j < lquads; ++j)
      for (unsigned int i = 0; i < wquads; ++i)
      {
	const unsigned int jTile = j / TILE_QUADS;
	const unsigned int iTile = i / TILE_QUADS;
	const int trails = groundTrails[iTile + WTILES * jTile];

	vertex a = vertices[    i + (wquads + 1) * j];
	vertex b = vertices[    i + (wquads + 1) * (j + 1)];
	vertex c = vertices[i + 1 + (wquads + 1) * (j + 1)];
	vertex d = vertices[i + 1 + (wquads + 1) * j];

	computeTexCoord(a, b, c, d, i % TILE_QUADS, j % TILE_QUADS, trails);

	chunk.add(a);
	chunk.add(b);
	chunk.add(c);
	chunk.add(d);
      }
  }


  void generateMeshes()
  {
    createBuildingsDescription();

    generateGroundTrails();

    generateGroundMesh(floorChunk);

    generateBuildingsMeshes();
  }

  // --------------------------------------------------------------------------

  void changeLightAndFog(date renderDate,
			 GLfloat * ambient,
			 GLfloat * diffuse,
			 GLfloat * specular,
			 GLfloat * fogColor)
  {
    float ambientFactor = 1.75f;
    float diffuseFactor = 0.3f;

    const float dawnAmbient = 0.2f;
    const float noonAmbient = 1.75f;
    const float duskAmbient = 0.02f;

    const float dawnDiffuse = 1.5f;
    const float noonDiffuse = 0.3f;
    const float duskDiffuse = 0.05f;
    const float duskDiffuseGlow = 0.11f;

    const float duskSpecular = 0.1f;
    const float duskSpecularGlow = 0.8f;

    const float duskFog = 0.15f;

    const float noonGlow = 3.f;
    const float duskGlow = 50.f;

    Shader::state.glowness = noonGlow;

    if (renderDate < 7000)
    {
      ambientFactor = dawnAmbient;
      diffuseFactor = dawnDiffuse;
    }
    else if (renderDate < 27000)
    {
      const float x = (float)(renderDate - 7000) / 20000.f;
      ambientFactor = mix(dawnAmbient, noonAmbient, x);
      diffuseFactor = mix(dawnDiffuse, noonDiffuse, x);
    }
    else if (renderDate < 136000)
    {
      ambientFactor = noonAmbient;
      diffuseFactor = noonDiffuse;
    }
    else if (renderDate < 146000)
    {
      const float x = (float)(renderDate - 136000) / 10000.f;
      ambientFactor = mix(noonAmbient, duskAmbient, x);
      diffuseFactor = mix(noonDiffuse, duskDiffuse, x);
      diffuse[3] = mix(diffuse[3], duskDiffuseGlow, x);

      specular[0] = mix(specular[0], duskSpecular, x);
      specular[1] = mix(specular[1], duskSpecular, x);
      specular[2] = mix(specular[2], duskSpecular, x);
      specular[3] = mix(specular[3], duskSpecularGlow, x);

      fogColor[0] = mix(fogColor[0], duskFog, x);
      fogColor[1] = mix(fogColor[1], duskFog, x);
      fogColor[2] = mix(fogColor[2], duskFog, x);

      Shader::state.glowness = mix(noonGlow, duskGlow, x);
    }
    else // t >= 146000
    {
      ambientFactor = duskAmbient;
      diffuseFactor = duskDiffuse;
      diffuse[3] = duskDiffuseGlow;

      specular[0] = duskSpecular;
      specular[1] = duskSpecular;
      specular[2] = duskSpecular;
      specular[3] = duskSpecularGlow;

      fogColor[0] = duskFog;
      fogColor[1] = duskFog;
      fogColor[2] = duskFog;

      Shader::state.glowness = duskGlow;
    }

    ambient[0] *= ambientFactor;
    ambient[1] *= ambientFactor;
    ambient[2] *= ambientFactor;

    diffuse[0] *= diffuseFactor;
    diffuse[1] *= diffuseFactor;
    diffuse[2] *= diffuseFactor;


    // Couleur des rayons

    // De jour : orange vert bleu
    const float trail1color1[] = {1.f, 0.3f, 0};
    const float trail2color1[] = {0.24f, 1.f, 0};
    const float trail3color1[] = {0, 0.7f, 1.f};

    // De nuit, version froide : fuchsia, citron, phosphore
    const float trail1color2[] = {1.f, 0, 0.4f};
    const float trail2color2[] = {0, 1.f, 0.52f};
    const float trail3color2[] = {0.94f, 1.f, 0};

    // De nuit, version pop : fuchsia, or, aqua
    const float trail1color3[] = {1.f, 0, 0.4f};
    const float trail2color3[] = {0, 0.7f, 1.f};
    const float trail3color3[] = {1.f, 0.87f, 0};

    float * color1 = Shader::state.trail1Color;
    float * color2 = Shader::state.trail2Color;
    float * color3 = Shader::state.trail3Color;

    if (renderDate < 136000)
    {
      color1[0] = trail1color1[0];
      color1[1] = trail1color1[1];
      color1[2] = trail1color1[2];

      color2[0] = trail2color1[0];
      color2[1] = trail2color1[1];
      color2[2] = trail2color1[2];

      color3[0] = trail3color1[0];
      color3[1] = trail3color1[1];
      color3[2] = trail3color1[2];
    }
    else if (renderDate < 141000)
    {
      const float x = (float)(renderDate - 136000) / 5000.f;
      color1[0] = mix(trail1color1[0], trail1color2[0], x);
      color1[1] = mix(trail1color1[1], trail1color2[1], x);
      color1[2] = mix(trail1color1[2], trail1color2[2], x);

      color2[0] = mix(trail2color1[0], trail2color2[0], x);
      color2[1] = mix(trail2color1[1], trail2color2[1], x);
      color2[2] = mix(trail2color1[2], trail2color2[2], x);

      color3[0] = mix(trail3color1[0], trail3color2[0], x);
      color3[1] = mix(trail3color1[1], trail3color2[1], x);
      color3[2] = mix(trail3color1[2], trail3color2[2], x);
    }
    else if (renderDate < 149000)
    {
      color1[0] = trail1color2[0];
      color1[1] = trail1color2[1];
      color1[2] = trail1color2[2];

      color2[0] = trail2color2[0];
      color2[1] = trail2color2[1];
      color2[2] = trail2color2[2];

      color3[0] = trail3color2[0];
      color3[1] = trail3color2[1];
      color3[2] = trail3color2[2];
    }
    else if (renderDate < 159000)
    {
      const float x = (float)(renderDate - 149000) / 10000.f;
      color1[0] = mix(trail1color2[0], trail1color3[0], x);
      color1[1] = mix(trail1color2[1], trail1color3[1], x);
      color1[2] = mix(trail1color2[2], trail1color3[2], x);

      color2[0] = mix(trail2color2[0], trail2color3[0], x);
      color2[1] = mix(trail2color2[1], trail2color3[1], x);
      color2[2] = mix(trail2color2[2], trail2color3[2], x);

      color3[0] = mix(trail3color2[0], trail3color3[0], x);
      color3[1] = mix(trail3color2[1], trail3color3[1], x);
      color3[2] = mix(trail3color2[2], trail3color3[2], x);
    }
    else
    {
      color1[0] = trail1color3[0];
      color1[1] = trail1color3[1];
      color1[2] = trail1color3[2];

      color2[0] = trail2color3[0];
      color2[1] = trail2color3[1];
      color2[2] = trail2color3[2];

      color3[0] = trail3color3[0];
      color3[1] = trail3color3[1];
      color3[2] = trail3color3[2];
    }
  }

  // --------------------------------------------------------------------------

  void create(RenderList & renderList)
  {
    START_TIME_EVAL;

    glPushMatrix();
    glLoadIdentity();

    // Plancher
    Renderable fBuildings(END_DATE, Shader::parallax, START_DATE,
			  1.f, buildingsChunk.size, VBO::buildings);
    fBuildings.setTextures(Texture::buildingLightPatterns,
			   Texture::buildingBumpPatterns,
			   Texture::buildingSpecularPatterns);
    fBuildings.setId(1);
    renderList.add(fBuildings);


    Renderable floor(END_DATE, Shader::ground, START_DATE,
		     1.f, floorChunk.size, VBO::floor);
    floor.setTextures(Texture::lightPatterns,
		      Texture::bumpPatterns,
		      Texture::specularPatterns);
    floor.setTexture2(Texture::groundNoise);
    floor.setId(2);
    renderList.add(floor);


    // Plafond
    glTranslatef(0, VDISTANCE, 0);
    glRotatef(180.f, 1.f, 0, 0);

    Renderable cBuildings(END_DATE, Shader::parallax, START_DATE,
			 1.f, buildingsChunk.size, VBO::buildings);
    cBuildings.setTextures(Texture::buildingLightPatterns,
			   Texture::buildingBumpPatterns,
			   Texture::buildingSpecularPatterns);
    cBuildings.setId(3);
    renderList.add(cBuildings);

    Renderable ceil(END_DATE, Shader::ground, START_DATE,
// 		    1.f, ceilChunk.size, VBO::ceil);
		    1.f, floorChunk.size, VBO::floor);
    ceil.setTextures(Texture::lightPatterns,
		     Texture::bumpPatterns,
		     Texture::specularPatterns);
    ceil.setTexture2(Texture::groundNoise);
    ceil.setId(4);
    renderList.add(ceil);


    glPopMatrix();

    END_TIME_EVAL("Scene setup");
  }
}
