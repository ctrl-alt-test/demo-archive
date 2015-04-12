#include "fbos.hh"
#include <GL/GLU.h>
#include "fboid.hh"
#include "textures.hh"
#include "shaders.hh"
#include "vbodata.hh"

namespace FBO
{
  extern id _used;

  Unit * list = NULL;

  typedef void (*fboBuilder)(IntroObject&);

  void buildPostProcess(IntroObject&);
//   void buildPostProcessPong(IntroObject&);
  void buildPostProcessDownscale2(IntroObject&);
  void buildPostProcessDownscale4(IntroObject&);
  void buildPostProcessDownscale8(IntroObject&);
  void buildPostProcessDownscale8Pong(IntroObject&);
  void buildTrails(IntroObject&);

  const fboBuilder builders[] =
  {
    buildPostProcess,
//     buildPostProcessPong,
    buildPostProcessDownscale2,
    buildPostProcessDownscale4,
    buildPostProcessDownscale8,
    buildPostProcessDownscale8Pong,
    buildTrails,
  };

  void loadFBOs(IntroObject& intro, Loading::ProgressDelegate * pd, int low, int high)
  {
    START_TIME_EVAL;

    const unsigned int numberOfBuilders = sizeof(builders) / sizeof(fboBuilder);

    const int ldStep = (high - low) / (numberOfBuilders + 1);
    int currentLd = low;

    list = new Unit[numberOfFBOs];

    for (unsigned int i = 0; i < numberOfBuilders; ++i)
    {
      DBG("loading FBO %u...", i);
      builders[i](intro);
      pd->func(pd->obj, currentLd); currentLd += ldStep;
      OGL_ERROR_CHECK("loadFBOs i = %u", i);
    }
    pd->func(pd->obj, high);

    END_TIME_EVAL("FBO setup");
  }

  void buildPostProcess(IntroObject& intro)
  {
    list[postProcess].setRenderSize(intro.xres, intro.yres);
    list[postProcess].attachTexture(Texture::postProcessRender, FBO::color);
    list[postProcess].attachTexture(Texture::postProcessVelocity, FBO::velocity);
    list[postProcess].attachTexture(Texture::postProcessDepth, FBO::depth);
  }

  /*
  void buildPostProcessPong(IntroObject& intro)
  {
    list[postProcessPong].setRenderSize(intro.xres, intro.yres);
    list[postProcessPong].attachTexture(Texture::postProcessPong, FBO::color);
  }
  */

  void buildPostProcessDownscale2(IntroObject& intro)
  {
    list[postProcessDownscale2].setRenderSize(intro.xres / 2, intro.yres / 2);
    list[postProcessDownscale2].attachTexture(Texture::postProcessDownscale2, FBO::color);
  }

  void buildPostProcessDownscale4(IntroObject& intro)
  {
    list[postProcessDownscale4].setRenderSize(intro.xres / 4, intro.yres / 4);
    list[postProcessDownscale4].attachTexture(Texture::postProcessDownscale4, FBO::color);
  }

  void buildPostProcessDownscale8(IntroObject& intro)
  {
    list[postProcessDownscale8].setRenderSize(intro.xres / 8, intro.yres / 8);
    list[postProcessDownscale8].attachTexture(Texture::postProcessDownscale8, FBO::color);
  }

  void buildPostProcessDownscale8Pong(IntroObject& intro)
  {
    list[postProcessDownscale8Pong].setRenderSize(intro.xres / 8, intro.yres / 8);
    list[postProcessDownscale8Pong].attachTexture(Texture::postProcessDownscale8Pong, FBO::color);
  }

  void buildTrails(IntroObject& intro)
  {
    list[trails].setRenderSize(256, 256);
    list[trails].attachTexture(Texture::trails, FBO::color);
  }

  static void _renderFullScreenImage(Texture::Unit & src)
  {
    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    VBO::Element::unUse();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 1., 0., 1.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    src.use(0);

    float x = src.renderWidth() * 1.f / src.width();
    float y = src.renderHeight() * 1.f / src.height();
    float vertices[] =
      {
	0, 0, 0,   0,
	0, y, 0,   1.f,
	x, y, 1.f, 1.f,
	x, 0, 1.f, 0
      };

    glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);
    glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
  }

  /*
  static bool ping = true;

  void applyPostProcessing(IntroObject& intro, Shader::id shaderId, bool isFinal)
  {
    id src = ping ? postProcess : postProcessPong;
    id dst = ping ? postProcessPong : postProcess;
    ping = !ping;

    if (isFinal)
    {
      ping = true;
      unUse();
      glViewport(0, 0, intro.xres, intro.yres);
    }
    else
      list[dst].use();

    Texture::Unit& tex = Texture::list[list[src].renderTexture()];
    Texture::Unit& velocityTex = Texture::list[list[postProcess].velocityTexture()];
    Texture::Unit& depthTex = Texture::list[list[postProcess].depthTexture()];

    depthTex.use(1);
    velocityTex.use(2);

    Shader::list[shaderId].use();
    _renderFullScreenImage(tex);
  }
  */

  void generateDownscales(IntroObject& intro)
  {
    Shader::list[Shader::noPost].use();

    Texture::Unit& src1 = Texture::list[list[postProcess].renderTexture()];
    list[postProcessDownscale2].use();
    _renderFullScreenImage(src1);

    Texture::Unit& src2 = Texture::list[list[postProcessDownscale2].renderTexture()];
    list[postProcessDownscale4].use();
    _renderFullScreenImage(src2);

    Texture::Unit& src3 = Texture::list[list[postProcessDownscale4].renderTexture()];
    list[postProcessDownscale8].use();
    _renderFullScreenImage(src3);


    // Passe de flou
    Shader::list[Shader::hBlurPass].use();
    Texture::Unit& src4 = Texture::list[list[postProcessDownscale8].renderTexture()];
    list[postProcessDownscale8Pong].use();
    _renderFullScreenImage(src4);

    Shader::list[Shader::vBlurPass].use();
    Texture::Unit& src5 = Texture::list[list[postProcessDownscale8Pong].renderTexture()];
    list[postProcessDownscale8].use();
    _renderFullScreenImage(src5);

//     Shader::list[Shader::hBlurPass].use();
//     list[postProcessDownscale8Pong].use();
//     _renderFullScreenImage(src4);

//     Shader::list[Shader::vBlurPass].use();
//     list[postProcessDownscale8].use();
//     _renderFullScreenImage(src5);

//     Shader::list[Shader::hBlurPass].use();
//     list[postProcessDownscale8Pong].use();
//     _renderFullScreenImage(src4);

//     Shader::list[Shader::vBlurPass].use();
//     list[postProcessDownscale8].use();
//     _renderFullScreenImage(src5);


    // Test de rendu à l'écran
//     unUse();
//     glViewport(0, 0, intro.xres, intro.yres);
//     Texture::Unit& final = Texture::list[list[postProcessDownscale8].renderTexture()];
//     _renderFullScreenImage(final);
  }

  //
  // Dernière passe de post processing, où on combine le glow, filtre,
  // ajoute le bruit et le vignetting...
  //
  void finalPass(IntroObject& intro)
  {
    unUse();
    glViewport(0, 0, intro.xres, intro.yres);

    Texture::Unit& tex = Texture::list[list[postProcess].renderTexture()];
    Texture::Unit& glowTex = Texture::list[list[postProcessDownscale8].renderTexture()];
    Texture::Unit& velocityTex = Texture::list[list[postProcess].velocityTexture()];
    Texture::Unit& depthTex = Texture::list[list[postProcess].depthTexture()];
    glowTex.use(1);
    depthTex.use(2);
    velocityTex.use(3);

    Shader::list[Shader::finalPass].use();
    _renderFullScreenImage(tex);
  }

#if DEBUG_TRAILS

  void showTrails(IntroObject& intro)
  {
    unUse();
    glViewport(0, 0, intro.xres, intro.yres);

    Texture::Unit::unUse();
    Texture::Unit& tex = Texture::list[list[trails].renderTexture()];

//     Shader::list[Shader::noPost].use();
    Shader::Program::unUse();
    glColor4f(1.f, 1.f, 1.f, 1.f);
    _renderFullScreenImage(tex);
  }

#endif // DEBUG_TRAILS

}
