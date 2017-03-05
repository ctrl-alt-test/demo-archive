//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//


#include "intro.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "tweakval.h"

#include "camera.hh"
#include "fbos.hh"
#include "files.hh"
#include "light.hh"
#include "revolution.hh"
#include "server.hh"
#include "textures.hh"
#include "timeline.hh"
#include "physic_test.hh"
#include "workshop.hh"

//---------------------------------------------------------------------

IntroObject intro;

//---------------------------------------------------------------------

void intro_updateTime()
{
  Clock & now = intro.now;

  date elapsed = (date)(msys_timerGet() - intro.mTo);

#if DEBUG

  now.real = elapsed;
  const int timeShift = now.pauseShift + (now.paused ? now.real - now.pauseDate : 0);
  // FIXME : on ne teste pas les bornes
  now.youtube = elapsed - timeShift + now.startTime;

#else

  now.youtube = elapsed;

#endif // DEBUG

  const Clock clock = Timeline::computeClock(now.youtube);
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

  Camera::Camera c1 = intro.cameraAnim->get(intro.now.camera);
  Camera::Camera c0 = intro.cameraAnim->get(intro.now.camera - 10);

  // Détection de changement de plan : si la différence est trop
  // importante, on estime qu'on a changé de plan et que l'ancienne
  // caméra n'a pas de sens
  const float d = norm(c1.t.v - c0.t.v);
  return (d < 5.f ? c0 : c1);
}

//---------------------------------------------------------------------
static void setupFogAndLight()
{
  const GLfloat fogDensity = _TV(0.4f); // 0: aucun fog; 2: opaque en zfar
  GLfloat fogColor[4] = {1.f, 0.94f, 0.8f, 1.f};
  Light::setLight(intro.now.story, intro.now.camera, Light::light0);
  Light::setSecondLight(Light::light1);
  Workshop::changeLightAndFog(intro.now.story,
			      Light::light0,
			      Light::light1,
			      fogColor);

  // Brouillard
//   glEnable(GL_FOG);
//   glFogi(GL_FOG_MODE, GL_EXP2); // Inutile avec les shaders
  glFogi(GL_FOG_COORD_SRC, GL_FOG_COORDINATE); // ???
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, fogDensity);

  // Lumière
  Light::applyLightsParams();
}

static void renderShadowMap(FBO::id dest)
{
  const GLenum buffer = GL_COLOR_ATTACHMENT0_EXT;

  glEnable(GL_DEPTH_TEST);
  Shader::list[Shader::createVSM].use();
  Texture::Unit::unUse();
  FBO::list[dest].use();
  oglDrawBuffers(1, &buffer);

  glClearColor(1.f, 1.f, 0.f, 1.f);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  if (intro.now.story < 6000)  // Gros hack : pas de shadow map pour la porte
    return;

  Light::setProjectorMatrix();

  // Rendu de la scène
  intro.sceneRenderList->renderShadows(intro.now.story);
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
  FBO::blurPass(FBO::shadowMap, FBO::shadowMapPong);
  oglGenerateMipmapEXT(GL_TEXTURE_2D);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

static void renderSceneRegular(FBO::id dest)
{
  static const GLenum buffers[] = {
    GL_COLOR_ATTACHMENT0_EXT,
    GL_COLOR_ATTACHMENT1_EXT
  };

  FBO::list[dest].use();
  glEnable(GL_DEPTH_TEST);

  // Couleur initialisée à la couleur du fog -- FIXME
  oglDrawBuffers(1, buffers);
  glClearColor(0, 0, 0, 1.f);
#if DEBUG
  if (intro.debugRenderMode != 0)
  {
    glClearColor(0, 0, 0, 1.f);
  }
#endif // DEBUG
  glClear(/* GL_COLOR_BUFFER_BIT | */ GL_DEPTH_BUFFER_BIT);

//  // Vitesse initialisée à 0
//  oglDrawBuffers(1, buffers + 1);
//  glClearColor(0.5f, 0.5f, 0, 1.f);
//  glClear(GL_COLOR_BUFFER_BIT);

  // On binde les deux, paré à dessiner
  oglDrawBuffers(2, buffers);

  // Rendu de la scène
  intro.sceneRenderList->render(intro.now.story, getOldCamera());
}

static void applyPostProcessing()
{
  Timeline::doFade(intro.now.story);
  Shader::state.time = intro.now.camera;
  VBO::Element::unUse();

  Camera::orthoProj(1.f, 1.f);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

#if DEBUG
  if (intro.debugRenderMode != 0)
  {
    FBO::noPostProcess();
  }
  else
#endif
  {
    FBO::generateDownscales();
    if (_TV(1))
    {
	FBO::generateBokeh();
    }
    FBO::finalPass();
    FBO::antialias();

//     FBO::noPostProcess();
  }

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

#if DEBUG
  if (intro.debugRenderMode == 0)
#endif
  intro.lensOrbs->apply();
}

static void displayOverlayText()
{
  assert(intro.font != NULL);
  assert(intro.xres * intro.yres > 1000);

  Shader::state.time = intro.now.youtube;
  Shader::list[Shader::text].use();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Camera::orthoProj(intro.xres, intro.yresVP);

  Texture::Unit::unUse();
  Timeline::printGreetings();
  // FIXME - processing du texte ici éventuellement
}

//---------------------------------------------------------------------

int intro_do( void )
{
    IFDBG(debugTools());
    intro_updateTime();

    int debugLoadingShaders = _TV(0);
    if (debugLoadingShaders) {
      Loading::drawLoading((float)(intro.now.story % 10000) / 5000.f);
      return 0;
    }

    Workshop::update(intro.now.story);

    // Mise à jour de la scene
    RenderList & sceneRL = *intro.sceneRenderList;
    sceneRL.fossoyeur(intro.now.story);

    for (int i = 0; i < Node::pool().size; ++i)
    {
      Node::pool()[i].update(intro.now.story);
    }
    sceneRL.moveInvisibleAtEnd();
    sceneRL.sort();


    // Foutoir OpenGL, à continuer de ranger
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    // Réglage du brouillard et de la lumière
    setupFogAndLight();

    // Rendu des ombres
    Camera::placeLightCamera();
    renderShadowMap(FBO::shadowMap);
	applyShadowMapPostProcessing();

    // Rendu normal
    Camera::placeCamera();
    renderSceneRegular(FBO::postProcess);

    // Représentation de la caméra et de la source de lumière
#if DEBUG
    Camera::placeCamera();
    Camera::drawRepresentation(intro.cameraAnim->get(intro.now.camera));
    Light::drawRepresentations();
#endif

    // Post-processing
    applyPostProcessing();

    // Affichage du texte
    displayOverlayText();

#if !STATIC_SHADERS
    debugShowTexture();
    debugShowBuffer();
    debugShowRenderableList();
#endif // STATIC_SHADERS


#if DEBUG
     return 0;
#else
     return (intro.now.youtube > demoEndDate);
#endif // !DEBUG
}
