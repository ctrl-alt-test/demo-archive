//
// Ce qui touche à la lumière
//

#ifndef		LIGHT_HH
# define	LIGHT_HH

#include "algebra/matrix.hh"
#include "basicTypes.hh"

namespace Light
{
  /*
  struct Light
  {
    float position[4];
    float color[4];
    float attenuation;
  };
  */

  extern float light0Position[4];
  extern float light1Position[4];

  void updateLightPositions(date cameraDate);
  void setProjectorMatrix();
  void placeLights(const matrix4 & cameraMatrix);
  void placeProjector(const matrix4 & objectMatrix);

#if DEBUG
  void drawRepresentations();

  void reloadFrames(const char*);
#endif // DEBUG

}

#endif		// LIGHT_HH
