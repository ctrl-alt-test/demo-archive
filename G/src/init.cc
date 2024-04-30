#include "intro.hh"

#include "anims.hh"
#include "camera.hh"
#include "demo.hh"
#include "fbos.hh"
#include "files.hh"
#include "loading.hh"
#include "materials.hh"
#include "physic_test.hh"
#include "project_loader.hh"
#include "renderlist.hh"
#include "sceneimporter.hh"
#include "shaders.hh"
#include "textures.hh"
#include "timeline.hh"
#include "timing.hh"
#include "vbos.hh"
#include "snd/sound.hh"
#include "snd/midisync.hh"

#include "sys/msys_debug.h"
#include "sys/msys_glext.h"

extern IntroObject intro;

namespace Intro
{

#if DEBUG

void _setupPause()
{
  intro.now.real = 0;
  intro.now.loopDate = 0;
  intro.now.loopDuration = 0;
  intro.now.looping = false;
  intro.now.pauseDate = 0;
  intro.now.pauseShift = 0;
  intro.now.paused = false;

  // Pour choisir la date de départ en debug, remplacer 0 par un *StartDate.
  intro.now.startTime = Timeline::findYoutubeTime(0);
}

void _setupManualCamera()
{
  intro.manualCamera = false;
  intro.fov = DEFAULT_FOV;
  intro.focus = DEFAULT_FOCUS;

  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0, -5.f, 0);
  getFromModelView(intro.cameraPosition);
  glPopMatrix();
}

void _setupManualLight()
{
  intro.manualLight = false;
  intro.lightPosition[0] = 0;
  intro.lightPosition[1] = 1.f;
  intro.lightPosition[2] = 0;
  intro.lightPosition[3] = 1.f;
}

#endif // DEBUG

int init(int xr, int yr, float ratio, bool showCredits,
	       Font::Font * font, Font::Sticker * sticker)
{
  DBG("Init");

  DBG("sizeof(Texture::id) = %d", sizeof(Texture::id));
  DBG("sizeof(Shader::id) = %d", sizeof(Shader::id));
  DBG("sizeof(VBO::id) = %d", sizeof(VBO::id));

  DBG("sizeof(vector3f) = %d", sizeof(vector3f));
  DBG("sizeof(matrix4) = %d", sizeof(matrix4));

  DBG("sizeof(Material::Element) = %d (~6x %d)", sizeof(Material::Element),  sizeof(Material::Element) / 6);
  DBG("sizeof(Node) = %d (~4x %d)", sizeof(Node), sizeof(Node) / 4);
  DBG("sizeof(Renderable) = %d (~4x %d)", sizeof(Renderable),  sizeof(Renderable) / 4);
  DBG("sizeof(RenderableContainer) = %d", sizeof(RenderableContainer));

  intro.xres = xr;
  intro.yres = yr;
  intro.aspectRatio = ratio;

  intro.aspectRatioVP = 16.f / 9.f;
  intro.yresVP = (int)(yr * intro.aspectRatio / intro.aspectRatioVP);
  intro.yVP = (yr - intro.yresVP) / 2;

  intro.sceneRenderList = NULL;
  intro.cameraAnim = NULL;
  intro.font = font;
  intro.sticker = sticker;
  intro.textHeight = msys_min(intro.xres, intro.yresVP) / 12;
  intro.showCredits = showCredits;
  DBG(NULL == font ? "Font not loaded" : "Font loaded");

  Loading::clearScreen();
  Loading::setupRendering();
  Loading::update();

#if DEBUG

  intro.initDone = false;

  intro.debug.renderMode = releaseRender;
  intro.debug.displayMode = displayScene;
  intro.debug.shaderStatus = Shader::notReady;
  intro.debug.showTexture = 0;
  intro.debug.showMaterial = 0;
  intro.debug.showMesh = 0;
  intro.debug.showBuffer = 0;
  intro.debug.lightingOnly = false;
  intro.debug.light0 = true;
  intro.debug.light1 = true;
  intro.debug.ambient = true;
  intro.debug.shadows = true;
  intro.debug.lensEffects = true;
  intro.debug.showProfiling = false;

  setDebugFlags(NULL);

  intro.sound = SOUND;
  _setupPause();
  _setupManualCamera();
  _setupManualLight();

#endif // DEBUG

  Loading::update();

  if (!Sound::precalcFFT()) return MP3_LOAD_ERROR;
  if (!Sound::init()) return MP3_LOAD_ERROR;
  if (!Sync::init()) return MIDI_LOAD_ERROR;

  Loading::update();

  IFDBG(debugToolsInit());
  ProjectLoader::load("data\\project");
  Shader::loadShaders();
  Texture::loadTextures();
  Material::loadMaterials();
  FBO::loadFBOs();
  VBO::loadVBOs();
  Demo::preloadFFT(0);

  intro.cameraAnim = Camera::newAnimation();
  Loading::update();

  Demo::create();
//  intro.sceneGraph = importScene();
  intro.sceneRenderList = RenderList::createFromScene(Node::pool());
  intro.lensOrbs = new PostProcessing::LensOrbs();
  Loading::update();

  IFDBG(VBO::exportIds());
#if DEBUG
  // tests de physique
  //phy_init();
  intro.initDone = true;
#endif

  return 0;
}

void initSynchronization()
{
  intro.mTo = msys_timerGet();
}

void end()
{
  // deallocate your stuff here
  if (SOUND IFDBG(&& intro.sound))
    Sound::stop();
  msys_end();
}

}
