#include "intro.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "tweakval.hh"

#include "specific/sky.hh"
#include "specific/storyboard.hh"
#include "specific/titlescreen.hh"

#include "camera.hh"
#include "demo.hh"
#include "fbos.hh"
#include "files.hh"
#include "light.hh"
#include "server.hh"
#include "textures.hh"
#include "timeline.hh"
#include "physic_test.hh"
#include "vbodata.hh"

//---------------------------------------------------------------------

IntroObject intro;

//---------------------------------------------------------------------

namespace Intro
{

void updateTime()
{
  Clock & now = intro.now;

  date elapsed = (date)(msys_timerGet() - intro.mTo);

#if DEBUG

  now.real = elapsed;
  const int timeShift = now.pauseShift + (now.paused ? now.real - now.pauseDate : 0);
  // FIXME : on ne teste pas les bornes
  now.youtube = elapsed - timeShift + now.startTime;
  if (now.looping)
  {
    now.youtube = now.loopDate + (now.youtube - now.loopDate) % now.loopDuration;
  }

#else

  now.youtube = elapsed;

#endif // DEBUG

  const Clock clock = Demo::computeClock(now.youtube);
  now.story = clock.story;
  now.camera = clock.camera;
}

Camera::Camera getOldCamera()
{

#if DEBUG

  // En mode caméra manuelle, on renvoie la caméra courante, pour
  // éviter d'obtenir n'importe quoi.
  if (intro.manualCamera)
  {
    matrix4 inv = intro.cameraPosition;
    invMatrix(inv);
    Camera::Camera sameCamera;
    sameCamera.t = computeTransformation(inv);
    sameCamera.fov = float(intro.fov);
    sameCamera.focus = intro.focus;

    return sameCamera;
  }

#endif // DEBUG

  Camera::Camera c1 = Camera::getCamera(intro.now.camera);
  Camera::Camera c0 = Camera::getCamera(intro.now.camera - 16);

  // Détection de changement de plan : si la différence est trop
  // importante, on estime qu'on a changé de plan et que l'ancienne
  // caméra n'a pas de sens
  const float d = norm(c1.t.v - c0.t.v);
  return (d < 10.f ? c0 : c1);
}

//---------------------------------------------------------------------

static void updateScene()
{
  RenderList & sceneRL = *intro.sceneRenderList;
  sceneRL.fossoyeur(intro.now.story);

  DBG_IF_FLAG(nodesUpdate)
    for (int i = 0; i < Node::pool().size; ++i)
    {
      Node::pool()[i].update(intro.now.story);
    }

  sceneRL.moveInvisibleAtEnd();
  sceneRL.sort();
}

static void drawShadowMap(FBO::id dest)
{
  glEnable(GL_DEPTH_TEST);
  FBO::list[dest].use();

  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

#if DEBUG
  if (!intro.debug.shadows) return;
#endif

  Light::setProjectorMatrix();

  // Rendu de la scène
  intro.sceneRenderList->renderShadows(intro.now.story);
}

static void drawMain(FBO::id dest)
{
  FBO::list[dest].use();
  glEnable(GL_DEPTH_TEST);

#if DEBUG
  if (intro.debug.renderMode != releaseRender)
  {
    glClearColor(0, 0, 0, 1.f);
  }
  else
#endif // DEBUG
  if (_TV(1))
  {
    static const GLenum buffers[] = {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT
    };

    // Couleur initialisée à la couleur du fog -- FIXME
    float bg[4];
    VAR(fogColor)->get((float)intro.now.youtube, 4, bg);
    oglDrawBuffers(1, buffers);
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Vitesse initialisée à 0
    oglDrawBuffers(1, buffers + 1);
    glClearColor(0.5f, 0.5f, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // On binde les deux, paré à dessiner
    oglDrawBuffers(2, buffers);
  }
  else
  {
    // La couleur et la vitesse ne sont pas initialisées
    glClear(GL_DEPTH_BUFFER_BIT);
  }

  // Storyboard
  if (_TV(0)) Storyboard::renderFullScreen(dest);
  Sky::renderBackground();

  // Rendu de la scène
  intro.sceneRenderList->render(intro.now.story, getOldCamera());
}

static void applyShadowMapPostProcessing()
{
  VBO::Element::unUse();
  Camera::orthoProj(1.f, 1.f);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // Passe de flou sur l'ombre
  FBO::blurPass(FBO::shadowMap, FBO::shadowMap, FBO::shadowMapPong, 0);
  oglGenerateMipmap(GL_TEXTURE_2D);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void applyMainPostProcessing()
{
  Shader::uniforms[Shader::time].set(intro.now.camera);
  VBO::Element::unUse();

  Camera::orthoProj(1.f, 1.f);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

#if DEBUG
  if (intro.debug.renderMode != releaseRender)
  {
    FBO::noPostProcess();
  }
  else
#endif
  {
    DBG_IF_FLAG(generateDownScale) FBO::generateDownScale();
    DBG_IF_FLAG(generateStreak) FBO::generateStreak();
    DBG_IF_FLAG(generateGlow) FBO::generateGlow();
#if DEBUG
    DBG_IF_FLAG(generateBokeh) FBO::generateBokeh();
#endif
    DBG_IF_FLAG(lensOrb) intro.lensOrbs->apply();
    DBG_IF_FLAG(finalPass)
    {
      FBO::finalPass();
      TitleScreen::renderTitleScreen(intro.showCredits);
      FBO::antialias();
    }
#if DEBUG
    else
    {
      TitleScreen::renderTitleScreen(intro.showCredits);
      FBO::noPostProcess();
    }
#endif
  }

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void shadowMapRenderPass()
{
  Camera::placeLightCamera();
  drawShadowMap(FBO::shadowMap);
  DBG_IF_FLAG(applyShadowMapPostProcessing) applyShadowMapPostProcessing();
}

static void mainRenderPass()
{
  Camera::placeCamera();
  drawMain(FBO::baseRender);

  // Représentation de la caméra et de la source de lumière
#if DEBUG
  if (intro.manualCamera && intro.debug.showCameraRepresentation)
  {
    Camera::placeCamera();
    Camera::drawRepresentation(Camera::getCamera(intro.now.camera));
  }
  Light::drawRepresentations();
#endif

  // Post-processing
  applyMainPostProcessing();
}

static void displayOverlayText()
{
  assert(intro.font != NULL);
  assert(intro.xres * intro.yres > 1000);

  Shader::uniforms[Shader::time].set(intro.now.youtube);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Camera::orthoProj(intro.xres, intro.yresVP);

  Texture::Unit::unUse();
  Timeline::printGreetings();
  // FIXME - processing du texte ici éventuellement
}

//---------------------------------------------------------------------

int mainLoop( void )
{
    IFDBG(debugTools());
    updateTime();

    // Hack pour synchroniser
    intro.now.story += _TV(2000);
    intro.now.youtube += _TV(2000);
    intro.now.camera += _TV(2000);

    int debugLoadingShaders = _TV(0);
    if (debugLoadingShaders) {
      Loading::drawLoading((float)(intro.now.story % 10000) / 5000.f);
      return 0;
    }

    Demo::update(intro.now.youtube);

    updateScene();

    // Foutoir OpenGL, à continuer de ranger
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);


    if (intro.now.story < 100000 || intro.now.story > 250000)
      Texture::list[Texture::seagulAnimationsMap].use(Texture::data);
    else if (intro.now.story >= 117000 && intro.now.story <= 207000)
    {
      const bool pingOrPong = FBO::updateParticleTexture(intro.now.youtube - 117000);
      FBO::id oldState = ( pingOrPong ? FBO::particlesPositionsPing : FBO::particlesPositionsPong);
      FBO::id newState = (!pingOrPong ? FBO::particlesPositionsPing : FBO::particlesPositionsPong);
      Texture::list[FBO::list[newState].renderTexture()].use(Texture::data);
      Texture::list[FBO::list[oldState].renderTexture()].use(Texture::data2);
    }

    // Réglage des états fonction de t
    Demo::setFrameParams(intro.now);
#if DEBUG
    debugActivateLightOptions();
#endif // DEBUG

    // Rendu des ombres
    shadowMapRenderPass();

    // Rendu normal
    mainRenderPass();

    // Affichage du texte
    DBG_IF_FLAG(displayOverlayText) displayOverlayText();

#if !STATIC_SHADERS
    debugShowOverlayInfo();
#endif // STATIC_SHADERS


#if DEBUG
     return 0;
#else
     return (intro.now.youtube > demoEndDate);
#endif // !DEBUG
}

}
