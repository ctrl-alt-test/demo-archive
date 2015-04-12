//
// Piles de caisses en bois
//

#include "cratestack.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "basicTypes.hh"
#include "cube.hh"
#include "vboid.hh"
#include "shaderid.hh"
#include "textureid.hh"

#define CRATE_SIZE 2.f
#define VERTICAL_SHIFT 1.01f
#define HORIZONTAL_SHIFT 1.1f

namespace CrateStack
{
  void addToList(RenderList & renderList,
		 date birthDate, date deathDate,
		 unsigned int width, unsigned int length)
  {
    unsigned int maxHeight = width;
    if (maxHeight > length)
    {
      maxHeight = length;
    }

    msys_srand(1546327460); // Oui, exactement.

    for (unsigned int k = 0; k < width; ++k)
      for (unsigned int j = 0; j < length; ++j)
      {
	glPushMatrix();
	const float rndx = (HORIZONTAL_SHIFT - 1.f) * msys_sfrand();
	const float rndy = (HORIZONTAL_SHIFT - 1.f) * msys_sfrand();
	glTranslatef(CRATE_SIZE * HORIZONTAL_SHIFT * (float(j) + rndx),
		     CRATE_SIZE * 0.5f,
		     CRATE_SIZE * HORIZONTAL_SHIFT * (float(k) + rndy));
	const unsigned int height = msys_rand() % maxHeight;
	for (unsigned int i = 0; i < height; ++i)
	{
	  glPushMatrix();
	  glTranslatef(0, CRATE_SIZE * VERTICAL_SHIFT * float(i), 0);
	  glRotatef(3.f * msys_sfrand(), 0, 1.f, 0);
	  Renderable crate(deathDate, Shader::bump, birthDate, CRATE_SIZE);
	  if (0 == k && 4 == j)
	  {
	    crate.setTextures(Texture::crateBlackMesa, Texture::crateBump, Texture::crateBlackMesaSpecular);
	  }
	  else
	  {
	    crate.setTextures(Texture::crate, Texture::crateBump, Texture::crateSpecular);
	  }
	  renderList.add(crate);
	  glPopMatrix();
	}
	glPopMatrix();
      }
  }
}
