#ifndef FBOS_HH
# define FBOS_HH

#include "intro.h"
#include "loading.hh"
#include "fbodata.hh"

namespace FBO
{
  extern Unit * list;

  void loadFBOs(IntroObject& intro, Loading::ProgressDelegate * pd, int low, int high);

  // Rend le contenu du framebuffer courant sur le framebuffer système
  // en appliquant le shader indiqué.
  // Après cette opération, le framebuffer courant est le framebuffer système.
  void applyPostProcessing(Shader::id shaderId);
}

#endif // FBOS_HH