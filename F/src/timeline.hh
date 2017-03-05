//
// Les valeurs qui changent au cours de la demo
//

#ifndef		TIMELINE_HH
# define	TIMELINE_HH

# include "sys/msys.h"
# include <GL/gl.h>

# include "basicTypes.hh"
# include "shaders.hh"
# include "textureid.hh"
# include "timing.hh"
# include "transformation.hh"

namespace Timeline
{
#if DEBUG
  int findYoutubeTime(date storyTime);
#endif

  void doFade(date date);
  bool setSkybox(date renderDate,
		 Texture::id & wallsTextureId, Texture::id & roofTextureId,
		 Texture::id & wallsTextureId2, Texture::id & roofTextureId2);
  Shader::id getPostProcess(date date);
  void printGreetings();
  Clock computeClock(date youtubeTime);
}

#endif
