#include "intro.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "anims.hh"
#include "camera.hh"
#include "fbos.hh"
#include "files.hh"
#include "loading.hh"
#include "physic_test.hh"
#include "renderlist.hh"
#include "sceneimporter.hh"
#include "shaders.hh"
#include "textures.hh"
#include "timeline.hh"
#include "timing.hh"
#include "vbos.hh"
#include "workshop.hh"
#include "../snd/sound.h"

extern IntroObject intro;

#if DEBUG

void _setupPause()
{
  intro.now.real = 0;
  intro.now.pauseDate = 0;
  intro.now.pauseShift = 0;
  intro.now.paused = false;

  // Pour choisir la date de départ en debug, remplacer 0 par un *StartDate.
  intro.startTime = Timeline::findYoutubeTime(0);
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
  intro.lightPosition[1] = 0;
  intro.lightPosition[2] = 0;
  intro.lightPosition[3] = 1.f;
}

#endif // DEBUG

void intro_init(int xr, int yr, float ratio, bool music,
	       Font::Font * font, Font::Sticker * sticker)
{
  DBG("init...");

  DBG("Taille d'un Texture::id : %d octets", sizeof(Texture::id));
  DBG("Taille d'un Shader::id : %d octets", sizeof(Shader::id));
  DBG("Taille d'un VBO::id : %d octets", sizeof(VBO::id));

  DBG("Taille d'un vector3f : %d octets", sizeof(vector3f));
  DBG("Taille d'un matrix4 : %d octets", sizeof(matrix4));

  DBG("Taille d'un Node : %d octets (~4x %d)", sizeof(Node), sizeof(Node) / 4);
  DBG("Taille d'un Renderable : %d octets (~4x %d)", sizeof(Renderable),  sizeof(Renderable) / 4);
  DBG("Taille d'un RenderableContainer : %d octets", sizeof(RenderableContainer));

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
  intro.textHeight = min(intro.xres, intro.yresVP) / 12;
  DBG(NULL == font ? "Fonte non chargée" : "Fonte chargée");

  Loading::clearScreen();
  Loading::setupRendering();
  Loading::update();

#if DEBUG

  intro.initDone = false;
  intro.showBuffer = 0;
  intro.showProfiling = false;
  intro.debugRenderMode = 0;
  intro.sound = SOUND;
  _setupPause();
  _setupManualCamera();
  _setupManualLight();

#endif // DEBUG

  Loading::update();

  Sound::init();
  Loading::update();

  IFDBG(debugToolsInit());
  Shader::loadShaders();
  Texture::loadTextures();
  Anim::loadAnims();
  FBO::loadFBOs();
  VBO::loadVBOs();

  intro.cameraAnim = Camera::newAnimation();
  Loading::update();

  Workshop::create();
//  intro.sceneGraph = importScene();
  intro.sceneRenderList = RenderList::createFromScene(Node::pool());
  intro.lensOrbs = new PostProcessing::LensOrbs();
  Loading::update();

#if DEBUG
  // tests de physique
  //phy_init();
  intro.initDone = true;
#endif
}

void intro_init_synchronization()
{
  intro.mTo = msys_timerGet();
}

void intro_end()
{
  // deallocate your stuff here
  if (SOUND IFDBG(&& intro.sound))
    Sound::stop();
  msys_end();
}
