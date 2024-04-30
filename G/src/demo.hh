#ifndef		DEMO_HH
# define	DEMO_HH

#include "camera.hh"
#include "light.hh"
#include "node.hh"
#include "textures.hh"
#include "timing.hh"

namespace Demo
{
  // Scenes
  Node * create();
  float getTranslatedPos(date d);
  void preloadFFT(date t);
  void update(date t);
  void generateMeshes();

  void setFrameParams(const Clock & now);
  int iPosFromDate(date t);

  // Camera
  extern const char * cameraFile;
  extern const Camera::KeyframeSource cameraFrames[];
  extern const int cameraFramesLength;
}

#endif
