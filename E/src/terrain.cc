//
// Engrenage
//

#include "terrain.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "cube.hh"
#include "renderable.hh"

namespace Terrain
{
  static const int width = 80;

  //
  // Fonction pour générer une heightmap de test en attendant
  // d'en avoir une vraie
  //
  float testHeightMap(float x, float y)
  {
    return 3.f * (msys_sinf(x * 0.05f) + msys_sinf(y * 0.1f));
  }

  float * cache = NULL;

  void initCache()
  {
    const unsigned int length = (width + 1) * (width + 1);
    cache = (float *)msys_mallocAlloc(length * sizeof(float));

    for (unsigned int j = 0; j <= width; ++j)
    {
      const float y = j - 40.f;
      const unsigned int index = j * (width + 1);
      for (unsigned int i = 0; i <= width; ++i)
      {
	const float x = i - 40.f;
	cache[index + i] = testHeightMap(x, y);
      }
    }
  }

  float cachedTestHeightMap(float x, float y)
  {
    const unsigned int i = (width / 2) + (int)x;
    const unsigned int j = (width / 2) + (int)y;
    const unsigned int index = j * (width + 1) + i;

    return cache[index];
  }

  bool isFramed(float value, float min, float max)
  {
    return (value >= min && value < max);
  }

  //
  // Est-ce que le terrain coupe le volume
  //
  bool intersects(float side, float Xmin, float Ymin, float Zmin)
  {
    const float Xmax = Xmin + side;
    const float Ymax = Ymin + side;
    const float Zmax = Zmin + side;

    int Xstart = (int)(Xmin + 40.f);
    if (Xmin > Xstart)
    {
      ++Xstart;
    }
    int Xend = (int)(Xmax + 40.f);
    int Zstart = (int)(Zmin + 40.f);
    if (Zmin > Zstart)
    {
      ++Zstart;
    }
    int Zend = (int)(Zmax + 40.f);

    for (int j = Zstart; j <= Zend; ++j)
    {
      const int index = j * (width + 1);
      for (int i = Xstart; i <= Xend; ++i)
      {
	if (isFramed(cache[index + i], Ymin, Ymax))
	{
	  return true;
	}
      }
    }

    return false;
  }

  //
  // Pour avoir une granularité irrégulière
  //
  bool continueToDig(unsigned int depth)
  {
    const unsigned int minDepth = 6;
    const unsigned int maxDepth = minDepth + 1;

    if (depth < minDepth)
    {
      return true;
    }
    if (depth >= maxDepth)
    {
      return false;
    }

    const unsigned int delta = maxDepth - minDepth;
    return (msys_rand() % delta) >= 1;
  }

  void exploreSpace(unsigned int depth, float side,
		    float Xmin, float Ymin, float Zmin)
  {
    if (intersects(side, Xmin, Ymin, Zmin))
    {
      const float halfSide = side * 0.5f;
      const float Xmed = Xmin + halfSide;
      const float Ymed = Ymin + halfSide;
      const float Zmed = Zmin + halfSide;

      if (continueToDig(depth))
      {
	// On subdivise
	const unsigned int newDepth = depth + 1;
	exploreSpace(newDepth, halfSide, Xmin, Ymin, Zmin);
	exploreSpace(newDepth, halfSide, Xmin, Ymin, Zmed);
	exploreSpace(newDepth, halfSide, Xmin, Ymed, Zmin);
	exploreSpace(newDepth, halfSide, Xmin, Ymed, Zmed);
	exploreSpace(newDepth, halfSide, Xmed, Ymin, Zmin);
	exploreSpace(newDepth, halfSide, Xmed, Ymin, Zmed);
	exploreSpace(newDepth, halfSide, Xmed, Ymed, Zmin);
	exploreSpace(newDepth, halfSide, Xmed, Ymed, Zmed);
      }
      else
      {
	// On génère le cube (le cube de base va de -1 à 1)
	glPushMatrix();
	glTranslatef(Xmed, Ymed , Zmed);
	glScalef(halfSide, halfSide, halfSide);
// 	glDrawArrays(GL_QUADS, 0, Cube::number_of_vertex);
// 	renderList.add(Renderable());
	glPopMatrix();
      }
    }
  }

  void addToList(RenderList & renderList, date deathDate)
  {
    msys_srand(0);
//     exploreSpace(1, (float)width, -(width * 0.5f), -(width * 0.5f), -(width * 0.5f));

    for (unsigned int j = 0; j < 40; ++j)
    {
      const float y = ((float)j - 20.f) * 2.f;
      for (unsigned int i = 0; i < 40; ++i)
      {
	const float x = ((float)i - 20.f) * 2.f;
	const float height = testHeightMap(x, y);
	glPushMatrix();
	glTranslatef(x, height, y);
	renderList.add(Renderable(deathDate));
	glPopMatrix();
      }
    }
  }
}
