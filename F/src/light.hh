//
// Ce qui touche à la lumière
//

#ifndef		LIGHT_HH
# define	LIGHT_HH

#include "sys/msys.h"

#include "basicTypes.hh"
#include "matrix.hh"

namespace Light
{
  struct Light
  {
    float position[4];

    float ambientColor[4];
    float diffuseColor[4];
    float specularColor[4];

    float attenuation;
  };

  extern Light light0;
  extern Light light1;
  /*
  extern Light light2;
  */

  void setLight(date storyDate,
		date cameraDate,
                Light & light);
  void setSecondLight(Light & light);
  void applyLightsParams();
  void setProjectorMatrix();
  void placeLights(const matrix4 & context);

#if DEBUG
  void drawRepresentations();

  void reloadFrames(const char*);
#endif

}

#endif		// LIGHT_HH
