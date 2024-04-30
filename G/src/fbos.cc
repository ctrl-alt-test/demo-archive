#include "fbos.hh"

#include "camera.hh"
#include "fbodata.hxx"
#include "fboid.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"
#include "textureunit.hxx"
#include "vbodata.hh"

#include "tweakval.hh"

#include "sys/msys_glext.h"

namespace FBO
{
  extern id _used;

  Unit * list = NULL;

  void loadFBOs()
  {
    START_TIME_EVAL;

    list = new Unit[numberOfFBOs];


    // Full size buffers
    // -----------------
    list[baseRender].setRenderSize(intro.xres, intro.yresVP);
    list[baseRender].attachTexture(Texture::colorBuffer, FBO::color, GL_CLAMP_TO_EDGE DBGARG("colorBuffer"));
    list[baseRender].attachTexture(Texture::velocityMap, FBO::color2, GL_CLAMP_TO_EDGE DBGARG("velocityMap"));
    list[baseRender].attachTexture(Texture::depthMap, FBO::depth, GL_CLAMP_TO_EDGE DBGARG("depthMap"));
    OGL_FBO_ERROR_CHECK("loadFBOs: baseRender");

    list[fullSizePong].setRenderSize(intro.xres, intro.yresVP);
    list[fullSizePong].attachTexture(Texture::fullSizePong1, FBO::color, GL_CLAMP_TO_EDGE DBGARG("fullSizePong 1/2"));
    list[fullSizePong].attachTexture(Texture::fullSizePong2, FBO::color2, GL_CLAMP_TO_EDGE DBGARG("fullSizePong 2/2"));
    list[fullSizePong].attachTexture(Texture::circleOfConfusion, FBO::coc, GL_CLAMP_TO_EDGE DBGARG("fullSizePong CoC"));
    OGL_FBO_ERROR_CHECK("loadFBOs: fullSizePong");


    // Down scale buffers
    // ------------------
#define DOWNSCALE_SIZE 256
    int downScaleWidth = DOWNSCALE_SIZE;
    int downScaleHeight = (DOWNSCALE_SIZE * intro.yresVP) / intro.xres;

    // Y a peut-etre un cas pourri si 4 * downScaleWidth > intro.xres
    list[postProcessDownscale2].setRenderSize(4 * downScaleWidth, 4 * downScaleHeight);
    list[postProcessDownscale2].attachTexture(Texture::postProcessDownscale2, FBO::color, GL_CLAMP_TO_EDGE DBGARG("postProcessDownscale2"));
    OGL_FBO_ERROR_CHECK("loadFBOs: postProcessDownscale2");

    list[postProcessDownscale4].setRenderSize(2 * downScaleWidth, 2 * downScaleHeight);
    list[postProcessDownscale4].attachTexture(Texture::postProcessDownscale4, FBO::color, GL_CLAMP_TO_EDGE DBGARG("postProcessDownscale4"));
    OGL_FBO_ERROR_CHECK("loadFBOs: postProcessDownscale4");

    list[downScaleRender].setRenderSize(downScaleWidth, downScaleHeight);
    list[downScaleRender].attachTexture(Texture::downScaleRender, FBO::color, GL_CLAMP_TO_BORDER DBGARG("downScaleRender"));
    OGL_FBO_ERROR_CHECK("loadFBOs: downScaleRender");

    list[downScalePong].setRenderSize(downScaleWidth, downScaleHeight);
    list[downScalePong].attachTexture(Texture::downScalePong1, FBO::color, GL_CLAMP_TO_BORDER DBGARG("downScalePong 1/2"));
    list[downScalePong].attachTexture(Texture::downScalePong2, FBO::color2, GL_CLAMP_TO_BORDER DBGARG("downScalePong 2/2"));
    OGL_FBO_ERROR_CHECK("loadFBOs: downScalePong");

    list[glow].setRenderSize(downScaleWidth, downScaleHeight);
    list[glow].attachTexture(Texture::glowBuffer, FBO::color, GL_CLAMP_TO_EDGE DBGARG("glow"));
    OGL_FBO_ERROR_CHECK("loadFBOs: glow");

    list[hStreak].setRenderSize(downScaleWidth, downScaleHeight);
    list[hStreak].attachTexture(Texture::hStreakBuffer1, FBO::color, GL_CLAMP_TO_BORDER DBGARG("hStreak 1/2"));
    list[hStreak].attachTexture(Texture::hStreakBuffer2, FBO::color2, GL_CLAMP_TO_BORDER DBGARG("hStreak 2/2"));
    OGL_FBO_ERROR_CHECK("loadFBOs: hStreak");

    list[vStreak].setRenderSize(downScaleWidth, downScaleHeight);
    list[vStreak].attachTexture(Texture::vStreakBuffer1, FBO::color, GL_CLAMP_TO_BORDER DBGARG("vStreak 1/2"));
    list[vStreak].attachTexture(Texture::vStreakBuffer2, FBO::color2, GL_CLAMP_TO_BORDER DBGARG("vStreak 2/2"));
    OGL_FBO_ERROR_CHECK("loadFBOs: vStreak");

    // Shadow map buffers
    // ------------------
#define SHADOW_MAP_SIZE 1024
    list[shadowMap].setRenderSize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    list[shadowMap].attachTexture(Texture::varianceShadowMap, FBO::moments, GL_CLAMP_TO_EDGE DBGARG("VarianceShadowMap"));
    list[shadowMap].attachTexture(Texture::shadowMap, FBO::depth, GL_CLAMP_TO_EDGE DBGARG("shadowMap"));
    OGL_FBO_ERROR_CHECK("loadFBOs: shadowMap");

    list[shadowMapPong].setRenderSize(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    list[shadowMapPong].attachTexture(Texture::varianceShadowMapPong, FBO::moments, GL_CLAMP_TO_EDGE DBGARG("VarianceShadowMapPong"));
    OGL_FBO_ERROR_CHECK("loadFBOs: shadowMapPong");

    // Particles (genre de compute shader)
    // ---------
    // Attention : ces deux nombres apparaissent dans particules.cc et particules.vert
    const int numParticules = 131072;//65536;
    const int numCols = 512;//256;
    const int numLines = numParticules / numCols;

    list[particlesPositionsPing].setRenderSize(numCols, numLines);
    list[particlesPositionsPing].attachTexture(Texture::particlesPositionsPing, FBO::compute, GL_CLAMP_TO_EDGE DBGARG("particlesPositionsPing"));
    OGL_FBO_ERROR_CHECK("loadFBOs: particlesPositionsPing");

    list[particlesPositionsPong].setRenderSize(numCols, numLines);
    list[particlesPositionsPong].attachTexture(Texture::particlesPositionsPong, FBO::compute, GL_CLAMP_TO_EDGE DBGARG("particlesPositionsPong"));
    OGL_FBO_ERROR_CHECK("loadFBOs: particlesPositionsPong");

    Loading::update();
    END_TIME_EVAL("FBO setup");
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

#define UPDATE_DT 16

  bool updateParticleTexture(date d)
  {
    static int lastDate = -1;
    static bool pingOrPong = false;

    // Retour dans le temps en debug.
    if (lastDate >= (int)d) {
      lastDate = d;
      return pingOrPong;
    }

    VBO::Element::unUse();
    Camera::orthoProj(1.f, 1.f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glCullFace(GL_BACK);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    int calcDate = lastDate;
    while ((int)d > calcDate + UPDATE_DT)
    {
      pingOrPong = !pingOrPong;

      FBO::id oldState = ( pingOrPong ? FBO::particlesPositionsPing : FBO::particlesPositionsPong);
      FBO::id newState = (!pingOrPong ? FBO::particlesPositionsPing : FBO::particlesPositionsPong);

      Shader::uniforms[Shader::time].set(calcDate);
      Shader::list[Shader::attractor].use();
      Texture::Unit& src = Texture::list[list[oldState].renderTexture()];
      list[newState].use();
      _renderFullScreenImage(src);

      OGL_FBO_ERROR_CHECK("updateParticleTexture");
      calcDate += UPDATE_DT;
    }
    lastDate = calcDate;

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    return pingOrPong;
  }

  void blurPass(FBO::id srcBuffer, FBO::id dstBuffer, FBO::id tmpBuffer, int pass)
  {
    Shader::uniforms[Shader::pass].set(pass);
    Shader::list[Shader::hBlurPass].use();
    Texture::Unit& src1 = Texture::list[list[srcBuffer].renderTexture()];
    list[tmpBuffer].use(true);
    _renderFullScreenImage(src1);

    Shader::list[Shader::vBlurPass].use();
    Texture::Unit& src2 = Texture::list[list[tmpBuffer].renderTexture()];
    list[dstBuffer].use(true);
    _renderFullScreenImage(src2);
  }

  void generateDownScale()
  {
    unUse();

    Shader::list[Shader::noPost].use();

    Texture::Unit& src1 = Texture::list[list[baseRender].renderTexture()];
    list[postProcessDownscale2].use();
    _renderFullScreenImage(src1);

    Texture::Unit& src2 = Texture::list[list[postProcessDownscale2].renderTexture()];
    list[postProcessDownscale4].use();
    _renderFullScreenImage(src2);

    Texture::Unit& src3 = Texture::list[list[postProcessDownscale4].renderTexture()];
    list[downScaleRender].use();
    _renderFullScreenImage(src3);
  }

  void generateGlow()
  {
    unUse();

    int i = 0;
    blurPass(downScaleRender, glow, downScalePong, i++);

    while (i < _TV(3))
      blurPass(glow, glow, downScalePong, i++);
  }


  static void _streakPass(Shader::id shaderId, Texture::id src1, Texture::id src2, const FBO::Unit & dstBuffer, int pass)
  {
    Texture::list[src2].use(Texture::velocity);
    dstBuffer.use();

    Shader::uniforms[Shader::pass].set(pass);
    Shader::list[shaderId].use();
    _renderFullScreenImage(Texture::list[src1]);
  }

  void generateStreak()
  {
    unUse();

    const Unit & src = list[downScaleRender];
    const Unit & tmp = list[downScalePong];
    {
      const Unit & dst = list[hStreak];
      int i = 0;
      _streakPass(Shader::hStreakPass, src.renderTexture(), src.renderTexture(), tmp, i++);
      _streakPass(Shader::hStreakPass, tmp.renderTexture(), tmp.renderTexture2(), dst, i++);
      _streakPass(Shader::hStreakPass, dst.renderTexture(), dst.renderTexture2(), tmp, i++);
      _streakPass(Shader::hStreakPass, tmp.renderTexture(), tmp.renderTexture2(), dst, -i);
    }
    {
      const Unit & dst = list[vStreak];
      int i = 0;
      _streakPass(Shader::vStreakPass, src.renderTexture(), src.renderTexture(), tmp, i++);
      _streakPass(Shader::vStreakPass, tmp.renderTexture(), tmp.renderTexture2(), dst, i++);
      _streakPass(Shader::vStreakPass, dst.renderTexture(), dst.renderTexture2(), tmp, i++);
      _streakPass(Shader::vStreakPass, tmp.renderTexture(), tmp.renderTexture2(), dst, -i);
    }
  }


  //
  // Bokeh en deux passes séparées tel que décrit dans le talk de DICE
  // Shader de urs/Mercury
  //
  void generateBokeh()
  {
    unUse();

    OGL_FBO_ERROR_CHECK("generateBokeh");

    // Première passe
    Texture::Unit& colorTex = Texture::list[list[baseRender].renderTexture()];
    Texture::Unit& depthTex = Texture::list[list[baseRender].depthTexture()];
    depthTex.use(Texture::depth);
    list[fullSizePong].use();

    OGL_FBO_ERROR_CHECK("generateBokeh - fbo use 1ere passe");

    Shader::list[Shader::bokehFstPass].use();
    _renderFullScreenImage(colorTex);

    OGL_FBO_ERROR_CHECK("generateBokeh - fin 1ere passe");

    // Deuxième passe
    Texture::Unit& src2A = Texture::list[list[fullSizePong].renderTexture()];
    Texture::Unit& src2B = Texture::list[list[fullSizePong].renderTexture2()];
    Texture::Unit& src2C = Texture::list[list[fullSizePong].cocTexture()];
    src2B.use(Texture::albedo2);
    src2C.use(Texture::depth);
    list[baseRender].use(true);

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

    Texture::Unit& colorTex    = Texture::list[list[baseRender].renderTexture()];
    Texture::Unit& velocityTex = Texture::list[list[baseRender].renderTexture2()];
    Texture::Unit& depthTex    = Texture::list[list[baseRender].depthTexture()];
    Texture::Unit& glowTex     = Texture::list[list[glow].renderTexture()];
    Texture::Unit& hStreakTex  = Texture::list[list[hStreak].renderTexture()];
    Texture::Unit& vStreakTex  = Texture::list[list[vStreak].renderTexture()];
    Texture::Unit& grainTex    = Texture::list[Texture::grainMap];
    velocityTex.use(Texture::velocity);
    glowTex.use(Texture::glow);
    hStreakTex.use(Texture::hStreak);
    vStreakTex.use(Texture::vStreak);
    depthTex.use(Texture::depth);
    grainTex.use(Texture::data);

    Shader::list[Shader::finalPass].use();
    list[fullSizePong].use(true);

    _renderFullScreenImage(colorTex);
  }

  // Passe d'anti-aliasing
  // A priori on ne fait rien d'autre pendant cette passe
  void antialias()
  {
    unUse();

    Texture::Unit& tex = Texture::list[list[fullSizePong].renderTexture()];
    Shader::list[Shader::antialias].use();
//     list[baseRender].use();
    _renderFullScreenImage(tex);
  }

#if DEBUG

  void noPostProcess()
  {
    unUse();

    Texture::Unit& tex = Texture::list[list[baseRender].renderTexture()];
    Shader::list[Shader::noPost].use();
    _renderFullScreenImage(tex);
  }

#endif // DEBUG

}
