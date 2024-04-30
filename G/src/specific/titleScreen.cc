
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"
#include "tweakval.hh"

#include "sys/msys_glext.h"

namespace TitleScreen
{
  void renderTitleScreen(bool showCredits)
  {
    if (showCredits == false || intro.now.youtube > 17000) return;

    const float w = 1.f;
    const float h = (_TV(1) ? 1.f : _TV(0) ? intro.aspectRatio : 1.f / intro.aspectRatio);
    const float vertices[] =
      {
	0, 0, 0,   0,
	w, 0, 1.f, 0,
	w, h, 1.f, 1.f,
	0, h, 0,   1.f,
      };

    glEnable(GL_BLEND);

    glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);
    glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);

    // On suppose que les render states sont corrects

    Shader::list[Shader::titleScreenGlow].use();
    Texture::list[Texture::titleScreenGlow].use(Texture::color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDrawArrays(GL_QUADS, 0, 4);

    Shader::list[Shader::titleScreen].use();
    Texture::list[Texture::titleScreen].use(Texture::color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_QUADS, 0, 4);

    glDisable(GL_BLEND);
  }
}
