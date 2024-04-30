
#include "intro.hh"
#include "shaders.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

namespace Sky
{
  void renderBackground()
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glCullFace(GL_BACK);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    const float w = 1.f;
    const float h = (_TV(1) ? 1.f : _TV(0) ? intro.aspectRatio : 1.f / intro.aspectRatio);
    const float vertices[] =
      {
	0, 0, 0,   0,
	w, 0, 1.f, 0,
	w, h, 1.f, 1.f,
	0, h, 0,   1.f,
      };

    glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);
    glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);

    // On suppose que les render states sont corrects

    VBO::Element::unUse();
    Shader::list[Shader::sky].use();
    glDrawArrays(GL_QUADS, 0, 4);

    // On rétablit les états
    glEnable(GL_DEPTH_TEST);
  }
}
