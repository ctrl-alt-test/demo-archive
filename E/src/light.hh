//
// Ce qui touche à la lumière
//

#ifndef		LIGHT_HH
# define	LIGHT_HH

#include "sys/msys.h"

#include "basicTypes.hh"

namespace Light
{
  void setLight(date storyDate,
		date cameraDate,
                GLfloat * position);

  void reloadFrames();
}

#endif		// LIGHT_HH
