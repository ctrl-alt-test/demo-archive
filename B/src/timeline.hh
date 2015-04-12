//
// Les valeurs qui changent au cours de la demo
//

#ifndef		TIMELINE_HH
# define	TIMELINE_HH

# include "sys/msys.h"
# include <GL/gl.h>

# include "basicTypes.hh"
# include "font.hh"
# include "intro.h"
# include "shaders.hh"
# include "textureid.hh"
# include "timing.hh"
# include "transformation.hh"

namespace Timeline
{
  typedef struct
  {
    date story;
    date camera;
  } doubleDate;

  void doFade(date date);
  bool setSkybox(date renderDate,
		 Texture::id & wallsTextureId, Texture::id & roofTextureId,
		 Texture::id & wallsTextureId2, Texture::id & roofTextureId2);
  Shader::id getPostProcess(date date);
  void printText(const IntroObject & intro);
  Transformation getCamera(const IntroObject & intro);
  doubleDate time_control(date youtubeTime);
  void shakeCamera(Transformation &tr, date t);
}

#endif
