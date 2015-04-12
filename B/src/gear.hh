//
// Engrenage
//

#ifndef		GEAR_HH
# define	GEAR_HH

#include "animid.hh"
#include "basicTypes.hh"
#include "renderlist.hh"
#include "shaderid.hh"
#include "textureid.hh"

namespace Gear
{
  void addToList(RenderList & renderList,
		 date birthDate, date deathDate, date endAnimDate,
		 /* date period, */ Anim::id animId,
		 unsigned int length,
		 unsigned int width,
		 unsigned int height = 1,
		 unsigned int spacePerTooth = 1,
		 float vgap = 0,
		 Shader::id shaderId = Shader::default,
		 Texture::id textureId = Texture::none);
}

#endif		// GEAR_HH
