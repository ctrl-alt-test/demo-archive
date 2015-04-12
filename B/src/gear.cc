//
// Engrenage
//

#include "gear.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "basictypes.hh"
#include "cube.hh"

namespace Gear
{
  void addToList(RenderList & renderList,
		 date birthDate, date deathDate, date endAnimDate,
		 /* date period, */ Anim::id animId,
		 unsigned int length, unsigned int width, unsigned int height,
		 unsigned int spacePerTooth,
		 float vgap,
		 Shader::id shaderId,
		 Texture::id textureId)
  {
    const unsigned int spaces = (length + width);

    /*
    const date duration = period * spaces;
    // FIXME : magnifique memory leak
    Anim::Anim * toothAnim = Anim::Anim::newSquareCycle(duration, float(width), float(length));
    */

    glPushMatrix();
    glTranslatef(-0.5f * width, 0, -0.5f * length);
    const float invStep = 1.f / spaces;
    for (unsigned int j = 0; j < spaces; ++j)
    {
      if (j % spacePerTooth == 0)
      {
	const float animationStepShift = j * invStep;
	for (unsigned int i = 0; i < height; ++i)
	{
	  glPushMatrix();
	  glTranslatef(0, i * (1.f + vgap), 0);
	  /*
	  if (period != 0)
	  {
	  */
	    {
	      Renderable tooth(deathDate, shaderId, birthDate);
	      tooth.setTextures(textureId, Texture::defaultBump, Texture::none);
	      tooth.setAnimation(animId);
	      tooth.setAnimationStepShift(animationStepShift);
	      tooth.setAnimationStopDate(endAnimDate);
	      renderList.add(tooth);
	    }
	    /*
	  }
	  else
	  {
	    const Transformation & t = toothAnim->getTransformation(0, animationStepShift);
	    float matrix[16];
	    computeMatrix(matrix, t.q, t.v);
	    glMultMatrixf(matrix);
	    renderList.add(Renderable(deathDate, shaderId, NULL, 0, 0, birthDate));
	  }
	    */
	  glPopMatrix();
	}
      }
    }
    glPopMatrix();
  }
}
