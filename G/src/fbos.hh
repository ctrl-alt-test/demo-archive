#ifndef FBOS_HH
# define FBOS_HH

#include "loading.hh"
#include "fbodata.hh"

namespace FBO
{
  extern Unit * list;

  void loadFBOs();

  bool updateParticleTexture(date time);
  void blurPass(FBO::id srcBuffer, FBO::id dstBuffer, FBO::id tmpBuffer, int pass);
  void generateDownScale();
  void generateGlow();
  void generateStreak();
  void generateBokeh();
  void finalPass();
  void antialias();

#if DEBUG

  void noPostProcess();

#endif // DEBUG

}

#endif // FBOS_HH
