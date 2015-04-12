//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//


#include "intro.h"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include "sys/msys_forbidden.h" // FIXME : virer dès que possible
#include <GL/gl.h>
#include <GL/glu.h>
#include "sys/events.h"

#include "anims.hh"
#include "city.hh"
#include "camera.hh"
#include "cube.hh"
#include "factory.hh"
#include "fbos.hh"
#include "forest.hh"
#include "keyframe.hh"
#include "light.hh"
#include "loading.hh"
#include "pong.hh"
#include "renderlist.hh"
#include "ramp.hh"
#include "rope.hh"
#include "screen.hh"
#include "shaderprogram.hh"
#include "shaders.hh"
#include "space.hh"
#include "textures.hh"
#include "timeline.hh"
#include "timing.hh"
#include "vbos.hh"
#include "../snd/sound.h"

#include "tweakval.h"


// À virer :
#include "gear.hh"
#include "sphere.hh"
#include "starfield.hh"
#include "treadmill.hh"
#include "zeus.hh"
//

//--------------------------------------------------------

Sphere::Cube * sphereCube = NULL;

IntroObject intro;

void play(void);

#if DEBUG
// recherche (approximativement) le youtubeTime à partir du storyTime
// sert pour le intro.startTime de debug
int findYoutubeTime(date storyTime)
{
  Timeline::doubleDate times;
  for (int i = 0; ; i += 500)
  {
    times = Timeline::time_control(i);
    if (storyTime < times.story)
      return i;
  }
}
#endif

void setupIntroParams(int xr, int yr)
{
  intro.xres = xr;
  intro.yres = yr;

#if DEBUG

  intro.renderList = NULL;
  intro.cameraAnim = NULL;
  intro.ropeCameraAnim = NULL;

  intro.realNow = 0;
  // Pour choisir la date de départ en debug, remplacer 0 par un *StartDate.
  intro.startTime = findYoutubeTime(0);
  intro.pauseDate = 0;
  intro.pauseShift = 0;
  intro.paused = false;
  intro.manualCamera = false;
  intro.fov = 39;
  intro.dof = DEFAULT_DOF;

  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0, -5.f, 0);
  glGetFloatv(GL_MODELVIEW_MATRIX, intro.cameraPosition);
  glPopMatrix();

  intro.manualLight = false;
  intro.lightPosition[0] = 0;
  intro.lightPosition[1] = 0;
  intro.lightPosition[2] = 0;
  intro.lightPosition[3] = 1.f;

#endif // DEBUG
}

int intro_init(int xr, int yr, bool music, Font * font,
	       Loading::ProgressDelegate * pd)
{
  DBG("init");

  Loading::setupRendering(xr, yr);
  Shader::loadFirstShader();
  pd->func(pd->obj, 0); // progress report : 0 .. 200


  setupIntroParams(xr, yr);
  pd->func(pd->obj, 5);

#if SOUND
  if (music) Sound::init();
#endif
  pd->func(pd->obj, 10);

  Shader::loadShaders(pd, 10, 20);
  Texture::loadTextures(pd, 20, 150);
  City * city = new City(80, 60);
  Anim::loadAnims(pd, 150, 160);

  intro.font = font;
  int min = intro.yres;
  if (intro.xres < min)
    min = intro.xres;
  intro.textHeight = min / 16;
  DBG(NULL == font ? "Fonte non chargée" : "Fonte chargée");

  intro.renderList = new RenderList(100000);
  RenderList & renderList = *intro.renderList;

//   renderList.initRenderToTexture(intro.xres, intro.yres);
  FBO::loadFBOs(intro, pd, 160, 165);

  pd->func(pd->obj, 170);

  VBO::loadVBOs(city, pd, 170, 180);
  city->generate(renderList);
  delete city;

  intro.cameraAnim = Camera::newAnimation();
  pd->func(pd->obj, 185);

  Space::create(renderList);
  pd->func(pd->obj, 188);

  Factory::create(renderList);
  pd->func(pd->obj, 190);

  Pong::create(renderList);
  pd->func(pd->obj, 192);

  Forest::create(renderList);
  pd->func(pd->obj, 194);

  Rope::create(renderList);
  intro.ropeCameraAnim = Rope::newCameraAnim();
  pd->func(pd->obj, 196);

  Ramp::create(renderList);
  pd->func(pd->obj, 198);

  Screen::createScreen(renderList);
  pd->func(pd->obj, 200);

#if SOUND
  if (music)
  {
    Sound::play();
  }
#endif // SOUND

  // Synchronisation à la fin du loading
  intro.mTo = POST_LOAD_DURATION + msys_timerGet();

  return 1;
}

void intro_end( void )
{
  // deallocate your stuff here
  if (SOUND) Sound::stop();
  msys_end();
  //dead_code();
}

//---------------------------------------------------------------------

void intro_updateTime()
{
#if DEBUG
  intro.realNow = (date)(msys_timerGet() - intro.mTo);
  const int timeShift = intro.pauseShift + (intro.paused ? intro.realNow - intro.pauseDate : 0);
  // FIXME : on ne teste pas les bornes
  intro.youtubeNow = intro.realNow - timeShift + intro.startTime;
#else
  intro.youtubeNow = (date)(msys_timerGet() - intro.mTo);
#endif // DEBUG
  Timeline::doubleDate times = Timeline::time_control(intro.youtubeNow);
  intro.storyNow = times.story;
  intro.cameraNow = times.camera;
}

vector3f placeCamera()
{
#if DEBUG
  if (intro.manualCamera)
  {
    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(intro.fov, float(intro.xres) / float(intro.yres), _TV(0.1f), _TV(500.f));
    broadcast1f(Shader::dof, intro.dof);

    // Position
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(intro.cameraPosition);

    // Représentation de la caméra :
    {
      glPushMatrix();
      Anim::Anim * cameraAnim = intro.cameraAnim;
      date animDate = intro.cameraNow;

      Transformation t = Timeline::getCamera(intro);
      float m[16];
      computeMatrix(m, t.q, t.v);
      glMultMatrixf(m);
      RenderList::renderSingleCube(Shader::default, Texture::validatorKO);
      glPopMatrix();
    }

    // Position de la caméra dans le monde
    float m[16];
    for (unsigned int i = 0; i < 16; ++i)
      m[i] = intro.cameraPosition[i];
    invMatrix(m);
    vector3f v = {m[12], m[13], m[14]};
    return v;
  }
  else
#endif // DEBUG
  {
    Anim::Anim * cameraAnim = intro.cameraAnim;
    date animDate = intro.cameraNow;

    Transformation t = Timeline::getCamera(intro);
    Timeline::shakeCamera(t, intro.storyNow);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(t.s, float(intro.xres) / float(intro.yres), 0.1f, 500.f); // FIXME : vrai frustrum
    broadcast1f(Shader::dof, t.dof);

    // Position
    glMatrixMode(GL_MODELVIEW);
    float m[16];
    computeInvMatrix(m, t.q, t.v);

    glLoadMatrixf(m);

    return t.v;
  }

//     glOrtho(-6.4f, 6.4f, -4.f, 4.f, 0.1f, 200.f);
//     glOrtho(-16.f, 16.f, -10.f, 10.f, 0.1f, 200.f);
//     glOrtho(-64.f, 64.f, -40.f, 40.f, 0.1f, 200.f);
}

//---------------------------------------------------------------------

int intro_do( void )
{
    ReloadChangedTweakableValues();

    glDisable(GL_BLEND);

    intro_updateTime();
    RenderList & renderList = *intro.renderList;

    renderList.fossoyeur(intro.storyNow);

    if (intro.storyNow >= screenStartDate && intro.storyNow < screenEndDate)
    {
      FBO::list[FBO::screenCube].use();
      renderList.renderSpinningCube(intro.storyNow);
    }

    {
      Texture::Unit & t = Texture::list[Texture::postProcessRender];
      broadcast1f(Shader::screenCX, 0.5f * (float)intro.xres / t.width());
      broadcast1f(Shader::screenCY, 0.5f * (float)intro.yres / t.height());
    }
    FBO::list[FBO::postProcess].use();

    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // Couleur de la texture mélangée à la couleur des sommets
//     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Plaquage correct des textures
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    // Positionnement de la caméra et rendu de la skybox
    const vector3f cameraPosition = placeCamera();
    renderList.renderSkyBox(intro.storyNow, cameraPosition);

//     glPolygonMode(GL_FRONT, GL_LINE);
//     glPolygonMode(GL_BACK, GL_LINE);

   Timeline::doFade(intro.storyNow);

    // -----------------------------
    {
      glEnable( GL_LIGHTING );
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

      GLfloat ambientLight[] = { 0.2f, 0.25f, 0.3f, 1.f };
      GLfloat diffuseLight0[] = { 0.8f, 0.75f, 0.7f, 0.95f };
      GLfloat specularLight0[] = { 1.f, 1.f, 1.f, 0.5f };
      GLfloat position0[] = { 10.f,// * msys_sinf(0.001f * intro.realNow),
			      20.f,
			      10.f,// * msys_cosf(0.001f * intro.realNow),
			      1.f };
      Light::setLight(intro.storyNow, intro.cameraNow,
		      ambientLight, diffuseLight0,
		      specularLight0, position0);
      glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
      glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight0 );
      glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight0 );

#if DEBUG

      glLightfv( GL_LIGHT0, GL_POSITION, intro.manualLight ? intro.lightPosition : position0 );
      if (intro.manualLight)
      {
	glPushMatrix();
	glTranslatef(intro.lightPosition[0], intro.lightPosition[1], intro.lightPosition[2]);
	RenderList::renderSingleCube(Shader::texture, Texture::highParticle);
	glPopMatrix();
      }

#else

      glLightfv( GL_LIGHT0, GL_POSITION, position0 );

#endif

      glEnable( GL_LIGHT0 );
    }

    // -----------------------------

    Space::update(renderList, intro.storyNow);
    Factory::update(renderList, intro.storyNow);

    renderList.moveInvisibleAtEnd();
    renderList.sort();
    /*
    glMatrixMode(GL_TEXTURE);
    //glLoadIdentity();
    //glScalef(1.01f, 1.01f, 1.01f);
    glTranslatef(0.1f, 0.1f, 0.1f);
    glMatrixMode(GL_MODELVIEW);
    */

    renderList.render(intro.storyNow);


    // Post-processing
    glViewport(0, 0, intro.xres, intro.yres);
    FBO::applyPostProcessing(Timeline::getPostProcess(intro.storyNow));



    // Affichage du texte
    assert(intro.font != NULL);
    assert(intro.xres * intro.yres > 1000);

    //Shader::Program::unUse();
    //Texture::list[Texture::roadH].use(2);
    //Shader::send1i(Shader::text, Shader::specularMap, 2);
    Shader::list[Shader::text].use();
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, intro.xres, intro.yres, 0);

    Timeline::printText(intro);
    Shader::Program::unUse();

#if DEBUG
     return 0;
#else
     return (intro.youtubeNow > 6*60*1000); // 6 minutes
#endif // !DEBUG
}

// Mettre ici le code mort qui doit être présent dans le binaire en release.
// Cela permet de mieux estimer la taille du binaire final.
static void fakeTextures(void)
{
  Texture::Channel a;
  Texture::Channel b;

  a.Clear();
  a.Gradient(1.1f, 0.3f, 0.2f, 0.6f);
  a.AddCuts(100, 10);
  a.Cells(42);
  a.Conic();//unsigned char, char);
  a.Square();
  a.Fade(45);
  a.Perlin(10, 1, 1, 1.2f);
  a.Sinus(10, 5);
  a.Cosinus(10, 7);
  a.Random();
  a.Cut(4);
  a.Abs();
  a.Clamp(0.2f, 0.5f);
  a.Scale(0.5f, 0.8f);
  a.NegativeNormalized();
  a.Segment(0.3f);
  a.Bump();
  a.Blur();
  a.GaussianBlur();
  a.HorizontalMotionBlur(2);
  a.VerticalMotionBlur(3);
  a.VerticalDribble();
  a.Translate(5, 8);
  a.HorizontalFlip();
  a.VerticalFlip();
  a.Transpose();
  a.Rotate90();
  a.MinimizeTo(b);
  a.MaximizeTo(b);
  a += b;
  b -= a;
  a *= b;
  a *= 0.7f;
  a /= 2.2f;
}

void dead_code(void)
{
  if (msys_rand() == 42) // pour tromper le compilateur.
  {
    Sound::play();

    RenderList & renderList = *intro.renderList;
    // Sphère qui devient un cube
    sphereCube = new Sphere::Cube(0, 20000, 30000, renderList);
    sphereCube->queueRendering(0);

    Zeus::addToList(renderList, 0, -1);
    Treadmill::addToList(renderList, 0, -1, -1, 20, 5);
    Gear::addToList(renderList, 0, -1, -1, Anim::none, 4, 8, 5, 2, 0.05f);

    City * city = new City(80, 60);
    VBO::loadVBOs(city, 0, 130, 160);
    delete city;
    Space::create(renderList);
    Forest::create(renderList);
    Factory::create(renderList);
    Rope::create(renderList);
    Screen::createScreen(renderList);

    fakeTextures();
  }
}
