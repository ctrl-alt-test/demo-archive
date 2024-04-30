//
// The loading part
//

#include "loading.hh"
#include "loadingsteps.hh"

#include "intro.hh"
#include "interpolation.hh"
#include "shaders.hh"
#include "textureunit.hh"
#include "vbodata.hh"

#include "sys/msys_glext.h"

#if DEBUG
#include <stdio.h>
#endif

// ============================================================================

namespace Loading
{
  static GLuint shaderId = 0;

  void initLoadingShader()
  {
#if STATIC_SHADERS
    const char *src_frag = Shader::shader_src[0];
    const char *src_vert = Shader::shader_src[1];
#else
    char * src_frag = Shader::LoadSource("progress.frag");
    char * src_vert = Shader::LoadSource("progress.vert");

    if (shaderId != 0) oglDeleteProgram(shaderId);
#endif

#if DEBUG
    Shader::compilResult results[2];
#endif

    shaderId = oglCreateProgram();
    GLuint frag = Shader::loadShader(GL_FRAGMENT_SHADER, src_frag DBGARG(&results[0]));
    GLuint vert = Shader::loadShader(GL_VERTEX_SHADER, src_vert DBGARG(&results[1]));
    OGL_ERROR_CHECK("load");
    oglAttachShader(shaderId, frag);
    oglAttachShader(shaderId, vert);
    OGL_ERROR_CHECK("attach");
    oglLinkProgram(shaderId);
    OGL_ERROR_CHECK("link");
  }

  void setupRendering()
  {
    initLoadingShader();
  }

  void drawLoading(float step)
  {
    // Shaders et cie.
    Texture::Unit::unUse();
    Shader::Program::unUse();

    const float invRes[2] = { 1.f / (float)intro.xres, 1.f / (float)intro.yres };

    oglUseProgram(shaderId);
    oglUniform1f(oglGetUniformLocation(shaderId, "percent"), step);
    oglUniform1f(oglGetUniformLocation(shaderId, "ratio"), intro.aspectRatio);
    oglUniform2fv(oglGetUniformLocation(shaderId, "invResolution"), 1, invRes);

    VBO::Element::unUse();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    // Le chargement a pu pourrir la matrice, donc on fait le ménage
    Camera::orthoProj(1.f, 1.f);

    glRects(0,0,1,1);
  }

  static int count = 0;

  int draw()
  {
    glViewport(0, intro.yVP, intro.xres, intro.yresVP);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    drawLoading((float)count / (float)LOADING_STEPS);

    return ++count;
  }

  void drawPostLoaded(date renderDate)
  {
#if DEBUG
    static bool messageDisplayed = false;
    if (!messageDisplayed)
    {
      messageDisplayed = true;
      DBG("Loading complete");

      // remet à jour LOADING_STEPS si besoin
      if (count != LOADING_STEPS)
      {
        FILE* fp = fopen("src/loadingsteps.hh", "w");
        if (fp == NULL)
          DBG("Cannot update LOADING_STEPS (%d)", count);
        else
        {
          fprintf(fp, "#define LOADING_STEPS %d // generated\n", count);
          fclose(fp);
        }
      }
    }
#endif
    const float step = renderDate / float(POST_LOAD_DURATION);
    drawLoading(1.f + step);
  }
}
