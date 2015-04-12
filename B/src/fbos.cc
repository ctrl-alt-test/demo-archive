#include "fbos.hh"
#include <GL/GLU.h>
#include "fboid.hh"
#include "textures.hh"
#include "shaders.hh"

namespace FBO
{
  extern id _used;

  Unit * list = NULL;

  typedef void (*fboBuilder)(IntroObject&);

  void buildPostProcess(IntroObject&);
  void buildScreenCube(IntroObject&);

  const fboBuilder builders[] =
  {
    buildPostProcess,
    buildScreenCube,
  };

  void loadFBOs(IntroObject& intro, Loading::ProgressDelegate * pd, int low, int high)
  {
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
  }

  void buildPostProcess(IntroObject& intro)
  {
    list[postProcess].setListId(postProcess);
    list[postProcess].setRenderSize(intro.xres, intro.yres);
    list[postProcess].attachTexture(Texture::postProcessRender, FBO::color);
    list[postProcess].attachTexture(Texture::postProcessDepth, FBO::depth);
  }

  void buildScreenCube(IntroObject&)
  {
    list[screenCube].setListId(screenCube);
//     list[screenCube].setRenderSize(80, 45); // TODO
    list[screenCube].setRenderSize(128, 128);
    list[screenCube].attachTexture(Texture::screenCubeRender, FBO::color);
    list[screenCube].attachTexture(Texture::screenCubeDepth, FBO::depth);
  }

  void applyPostProcessing(Shader::id shaderId)
  {
    unUse();
    Texture::Unit& tex = Texture::list[list[postProcess].renderTexture()];
    Texture::Unit& depthTex = Texture::list[list[postProcess].depthTexture()];

    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 1., 0., 1.);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    tex.use(0);
    depthTex.use(1);
    Texture::list[Texture::transitionBurn].use(2);
    Shader::list[shaderId].use();

    float xRatio = tex.renderWidth() * 1.f / tex.width();
    float yRatio = tex.renderHeight() * 1.f / tex.height();
    glBegin(GL_QUADS);
    glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 0.f);
    glTexCoord2f(0.f, yRatio); glVertex2f(0.f, 1.f);
    glTexCoord2f(xRatio, yRatio); glVertex2f(1.f, 1.f);
    glTexCoord2f(xRatio, 0.f); glVertex2f(1.f, 0.f);
    glEnd();
  }
}
