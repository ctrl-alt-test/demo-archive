#include "fbos.hh"

#include <GL/GLU.h>

#include "camera.hh"
#include "fboid.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"
#include "vbodata.hh"

namespace FBO
{
  extern id _used;

  Unit * list = NULL;

  typedef void (*fboBuilder)();

  void buildPostProcess();
  void buildPostProcessDownscales();
  void buildShadowMap();

  const fboBuilder builders[] =
  {
    buildPostProcess,
    buildPostProcessDownscales,
    buildShadowMap,
  };

  void loadFBOs()
  {
    START_TIME_EVAL;

    const unsigned int numberOfBuilders = sizeof(builders) / sizeof(fboBuilder);

    list = new Unit[numberOfFBOs];

    for (unsigned int i = 0; i < numberOfBuilders; ++i)
    {
      DBG("loading FBO %u...", i);
      builders[i]();
      Loading::update();
      OGL_ERROR_CHECK("loadFBOs i = %u", i);
    }

    END_TIME_EVAL("FBO setup");
  }

  void buildPostProcess()
  {
    list[postProcess].setRenderSize(intro.xres, intro.yresVP);
    list[postProcess].attachTexture(Texture::postProcessRender, FBO::color DBGARG("postProcessRender"));
    list[postProcess].attachTexture(Texture::velocityMap, FBO::color2 DBGARG("velocityMap"));
    list[postProcess].attachTexture(Texture::depthMap, FBO::depth DBGARG("depthMap"));

    list[postProcessPong].setRenderSize(intro.xres, intro.yresVP);
    list[postProcessPong].attachTexture(Texture::postProcessPong, FBO::color DBGARG("postProcessPong 1/2"));
    list[postProcessPong].attachTexture(Texture::postProcessPong2, FBO::color2 DBGARG("postProcessPong 2/2"));
  }

  void buildPostProcessDownscales()
  {
    list[postProcessDownscale2].setRenderSize(intro.xres / 2, intro.yresVP / 2);
    list[postProcessDownscale2].attachTexture(Texture::postProcessDownscale2, FBO::color DBGARG("postProcessDownscale2"));

    list[postProcessDownscale4].setRenderSize(intro.xres / 4, intro.yresVP / 4);
    list[postProcessDownscale4].attachTexture(Texture::postProcessDownscale4, FBO::color DBGARG("postProcessDownscale4"));

    list[postProcessDownscale8].setRenderSize(intro.xres / 8, intro.yresVP / 8);
    list[postProcessDownscale8].attachTexture(Texture::postProcessDownscale8, FBO::color DBGARG("postProcessDownscale8"));

    list[postProcessDownscale8Pong].setRenderSize(intro.xres / 8, intro.yresVP / 8);
    list[postProcessDownscale8Pong].attachTexture(Texture::postProcessDownscale8Pong, FBO::color DBGARG("postProcessDownscale8Pong"));
  }

  void buildShadowMap()
  {
#define SHADOW_MAP_SIZE 1024
    list[shadowMap].setRenderSize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    list[shadowMap].attachTexture(Texture::varianceShadowMap, FBO::moments DBGARG("VarianceShadowMap"));
    list[shadowMap].attachTexture(Texture::shadowMap, FBO::depth DBGARG("shadowMap"));

    list[shadowMapPong].setRenderSize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    list[shadowMapPong].attachTexture(Texture::varianceShadowMapPong, FBO::moments DBGARG("VarianceShadowMapPong"));
  }

  static void _renderFullScreenImage(Texture::Unit & src)
  {
    src.use(Texture::color);

	// Un seul triangle, qui couvre tout l'écran
    const float x = src.renderWidth() * 2.f / src.width();
    const float y = src.renderHeight() * 2.f / src.height();
    const float vertices[] =
      {
	0, 0, 0,   0,
	x, 0, 2.f, 0,
	0, y, 0, 2.f,
      };

    glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);
    glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);

    // On suppose que les render states sont corrects
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  /*
  static bool ping = true;

  void applyPostProcessing(Shader::id shaderId, bool isFinal)
  {
    id src = ping ? postProcess : postProcessPong;
    id dst = ping ? postProcessPong : postProcess;
    ping = !ping;

    if (isFinal)
    {
      ping = true;
      unUse();
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

  void blurPass(FBO::id buffer1, FBO::id buffer2)
  {
    Shader::list[Shader::hBlurPass].use();
    Texture::Unit& src1 = Texture::list[list[buffer1].renderTexture()];
    list[buffer2].use();
    _renderFullScreenImage(src1);

    Shader::list[Shader::vBlurPass].use();
    Texture::Unit& src2 = Texture::list[list[buffer2].renderTexture()];
    list[buffer1].use();
    _renderFullScreenImage(src2);
  }

  void generateDownscales()
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
    blurPass(postProcessDownscale8, postProcessDownscale8Pong);

    // Test de rendu à l'écran
//     unUse();
//     Texture::Unit& final = Texture::list[list[postProcessDownscale8].renderTexture()];
//     _renderFullScreenImage(final);
  }

  //
  // Bokeh en deux passes séparées tel que décrit dans le talk de DICE
  // Shader de urs/Mercury
  //
  void generateBokeh()
  {
    static const GLenum buffers[] = {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT
    };

    // Première passe
    Texture::Unit& colorTex = Texture::list[list[postProcess].renderTexture()];
    Texture::Unit& depthTex = Texture::list[list[postProcess].depthTexture()];
    depthTex.use(Texture::depth);
    list[postProcessPong].use();
    oglDrawBuffers(2, buffers);

    Shader::list[Shader::bokehFstPass].use();
    _renderFullScreenImage(colorTex);

    // Deuxième passe
    Texture::Unit& src2A = Texture::list[list[postProcessPong].renderTexture()];
    Texture::Unit& src2B = Texture::list[list[postProcessPong].renderTexture2()];
    src2B.use(Texture::velocity);
    list[postProcess].use();

    oglDrawBuffers(1, buffers);

    Shader::list[Shader::bokehSndPass].use();
    _renderFullScreenImage(src2A);
  }

  //
  // Dernière passe de post processing, où on combine le glow, filtre,
  // ajoute le bruit et le vignetting...
  //
  void finalPass()
  {
    unUse();

    Texture::Unit& colorTex    = Texture::list[list[postProcess].renderTexture()];
    Texture::Unit& velocityTex = Texture::list[list[postProcess].renderTexture2()];
    Texture::Unit& depthTex    = Texture::list[list[postProcess].depthTexture()];
    Texture::Unit& glowTex     = Texture::list[list[postProcessDownscale8].renderTexture()];
    Texture::Unit& grainTex    = Texture::list[Texture::grainMap];
    velocityTex.use(Texture::velocity);
    depthTex.use(Texture::depth);
    glowTex.use(Texture::glow);
    grainTex.use(Texture::grain);

    Shader::list[Shader::finalPass].use();
    list[postProcessPong].use();
    _renderFullScreenImage(colorTex);
  }

  // Passe d'anti-aliasing
  // A priori on ne fait rien d'autre pendant cette passe
  void antialias()
  {
    unUse();

    Texture::Unit& tex = Texture::list[list[postProcessPong].renderTexture()];
    Shader::list[Shader::antialias].use();
//     list[postProcess].use();
    _renderFullScreenImage(tex);
  }

#if DEBUG

  void noPostProcess()
  {
    unUse();

    Texture::Unit& tex = Texture::list[list[postProcess].renderTexture()];
    Shader::list[Shader::noPost].use();
    _renderFullScreenImage(tex);
  }

#endif // DEBUG

}
