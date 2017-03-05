#ifndef FBOS_HH
# define FBOS_HH

#include "loading.hh"
#include "fbodata.hh"

namespace FBO
{
  extern Unit * list;

  void loadFBOs();

  // Rend le contenu du framebuffer courant sur le framebuffer système
  // en appliquant le shader indiqué.
  // Après cette opération, le framebuffer courant est le framebuffer système.
  void applyPostProcessing(Shader::id shaderId, bool isFinal);
  void blurPass(FBO::id buffer1, FBO::id buffer2);
  void generateDownscales();
  void generateBokeh();
  void finalPass();
  void antialias();

#if DEBUG

  void noPostProcess();

#endif // DEBUG

}

#endif // FBOS_HH
