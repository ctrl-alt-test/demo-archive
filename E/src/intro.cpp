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
#include "camera.hh"
#include "cube.hh"
#include "factory.hh"
#include "fbos.hh"
#include "files.hh"
#include "keyframe.hh"
#include "light.hh"
#include "loading.hh"
#include "renderlist.hh"
#include "shaderprogram.hh"
#include "shaders.hh"
#include "textures.hh"
#include "timeline.hh"
#include "timing.hh"
#include "trails.hh"
#include "vbos.hh"
#include "../snd/sound.h"


// Test
#include "cylinder.hh"
#include "mesh.hh"
#include "shaderprogram.hh"


#include "tweakval.h"

//--------------------------------------------------------

IntroObject intro;

typedef struct
{
  quad vertices;
  float intensity;
  int group;
  date birth;
} trailDesc;
Array<trailDesc> trailElements;

void play(void);

#if DEBUG
// recherche (approximativement) le youtubeTime à partir du storyTime
// sert pour le intro.startTime de debug
int findYoutubeTime(date storyTime)
{
  Clock clock;
  for (int i = 0; ; i += 500)
  {
    clock = Timeline::computeClock(i);
    if (storyTime < clock.story)
      return i;
  }
}
#endif

void setupIntroParams(int xr, int yr)
{
  intro.xres = xr;
  intro.yres = yr;

#if DEBUG

  intro.sceneRenderList = NULL;
  intro.cameraAnim = NULL;

  intro.now.real = 0;
  intro.now.pauseDate = 0;
  intro.now.pauseShift = 0;
  intro.now.paused = false;

  // Pour choisir la date de départ en debug, remplacer 0 par un *StartDate.
  intro.startTime = findYoutubeTime(0);

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
  intro.lightPosition[3] = 0;

#endif // DEBUG
}

int intro_init(int xr, int yr, bool music, Font * font,
	       Loading::ProgressDelegate * pd)
{
  DBG("init...");

  Loading::setupRendering(xr, yr);
  pd->func(pd->obj, 0); // progress report : 0 .. 200


  setupIntroParams(xr, yr); // (0,03 s)
  pd->func(pd->obj, 1);

  Trails::init();

#if SOUND
  if (music) Sound::init(); // (0,8 s)
#endif
  pd->func(pd->obj, 4);


  Shader::loadShaders(pd, 4, 13); // (4 s)
  Texture::loadTextures(pd, 13, 126); // (90 s)
  Anim::loadAnims(pd, 126, 127); // (0,02 s)

  intro.font = font;
  int min = intro.yres;
  if (intro.xres < min)
    min = intro.xres;
  intro.textHeight = min / 12;
  DBG(NULL == font ? "Fonte non chargée" :  "Fonte chargée");

  intro.sceneRenderList = new RenderList(4); // Sol + plafond + bâtiments
  RenderList & sceneRL = *intro.sceneRenderList;

  FBO::loadFBOs(intro, pd, 127, 128); // (0,15 s)
  pd->func(pd->obj, 170);

  VBO::loadVBOs(pd, 128, 182); // (5 s)

  intro.cameraAnim = Camera::newAnimation(); // (0 s)
  pd->func(pd->obj, 183);

  Factory::create(sceneRL); // (0 s)
  pd->func(pd->obj, 200);

  IFDBG(debugToolsInit());
  return 1;
}

void intro_init_synchronization()
{
  intro.mTo = msys_timerGet();
}

void intro_end( void )
{
  // deallocate your stuff here
  if (SOUND) Sound::stop();
  msys_end();
}

//---------------------------------------------------------------------

void intro_updateTime()
{
  Clock & now = intro.now;

#if DEBUG
  now.real = (date)(msys_timerGet() - intro.mTo);
  const int timeShift = now.pauseShift + (now.paused ? now.real - now.pauseDate : 0);
  // FIXME : on ne teste pas les bornes
  now.youtube = now.real - timeShift + now.startTime;

#else

  now.youtube = (date)(msys_timerGet() - intro.mTo);

#endif // DEBUG

  const Clock clock = Timeline::computeClock(now.youtube);
  now.story = clock.story;
  now.camera = clock.camera;
}

vector3f placeCamera()
{
  intro.zNear = _TV(0.1f);
  intro.zFar = _TV(200.f);
#if DEBUG
  if (intro.manualCamera)
  {
    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(intro.fov, float(intro.xres) / float(intro.yres),
		   intro.zNear, intro.zFar);
    Shader::state.dof = intro.dof;

    // Position
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(intro.cameraPosition);

    msys_memcpy(Shader::state.cameraMatrix, intro.cameraPosition,
		sizeof(Shader::state.cameraMatrix));

    // Représentation de la caméra :
    Camera::drawRepresentation(Timeline::getCamera(intro));

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
    Transformation t = Timeline::getCamera(intro);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(t.s, float(intro.xres) / float(intro.yres),
		   intro.zNear, intro.zFar); // FIXME : vrai frustrum
    Shader::state.dof = t.dof;

    // Position
    glMatrixMode(GL_MODELVIEW);
    float m[16];
    computeInvMatrix(m, t.q, t.v);

    glLoadMatrixf(m);

    msys_memcpy(Shader::state.cameraMatrix, m,
		sizeof(Shader::state.cameraMatrix));

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
    IFDBG(Files::checkFileChanges());

    intro_updateTime();

    GLenum buffers[] = {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT
    };

    // -------------------------------------------------------------
    // Rendu des motifs
    Trails::render(intro.now.story);

#if DEBUG_TRAILS

    FBO::showTrails(intro);

#else

    // -------------------------------------------------------------
    // Rendu de la scene
    RenderList & sceneRL = *intro.sceneRenderList;
    sceneRL.fossoyeur(intro.now.story);

    {
      Texture::Unit & t = Texture::list[Texture::postProcessRender];
      Shader::state.screenCX = 0.5f * (float)intro.xres / t.width();
      Shader::state.screenCY = 0.5f * (float)intro.yres / t.height();
    }
    FBO::list[FBO::postProcess].use();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    // Couleur de la texture mélangée à la couleur des sommets
//     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Plaquage correct des textures
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    // Positionnement de la caméra et rendu de la skybox
    const vector3f cameraPosition = placeCamera();

    Shader::state.zNear = intro.zNear;
    Shader::state.zFar = intro.zFar;

//     glPolygonMode(GL_FRONT, GL_LINE);
//     glPolygonMode(GL_BACK, GL_LINE);

    Timeline::doFade(intro.now.story);

    // -----------------------------
    // Réglage du brouillard et de la lumière
    {
      const GLfloat fogDensity = 0.01f;
      GLfloat fogColor[4] = {167.f/255.f,
			     182.f/255.f,
			     213.f/255.f,
			     1.f};

      GLfloat ambientLight[] = { 1.f, 1.f, 1.f, 1.f };
      GLfloat diffuseLight0[] = { 0.8f, 0.75f, 0.7f, 0.05f };
      /*
      GLfloat diffuseLight1[] = { 1.f, 0, 0.52f, 0.95f };
      GLfloat diffuseLight2[] = { 0.4f, 1.f, 0.88f, 0.95f };
      */

      GLfloat specularLight0[] = { 1.f, 1.f, 1.f, 0 };
      GLfloat position0[] = { 0, 1.f, 0, 0 };
      /*
      GLfloat position1[] = { 0, 0, 0, 1.f };
      GLfloat position2[] = { 0, 0, 0, 1.f };
      */

      Light::setLight(intro.now.story, intro.now.camera, position0);
      Factory::changeLightAndFog(intro.now.story,
				 ambientLight,
				 diffuseLight0,
				 specularLight0,
				 fogColor);

      // -----------------------------
      // Effacement des buffers

      // Couleur initialisée à la couleur du fog -- FIXME
      oglDrawBuffers(1, buffers);
      glClearColor(fogColor[0], fogColor[1], fogColor[2], 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Vitesse initialisée à 0
      oglDrawBuffers(1, buffers + 1);
      glClearColor(0, 0, 0, 1.f);
      glClear(GL_COLOR_BUFFER_BIT);

      // On binde les deux, paré à dessiner
      oglDrawBuffers(2, buffers);

      // /!\
      // /!\ Tout dessin avant cette partie vient d'être effacé !
      // /!\
      // -----------------------------

      /*
      position1[0] = position0[0] + 10.f;
      position1[1] = position0[1] - 10.f;
      position1[2] = position0[2];

      position2[0] = position0[0];
      position2[1] = position0[1] - 10.f;
      position2[2] = position0[2] + 10.f;
      */

      // -----------------------------
      // Brouillard
      glEnable(GL_FOG);
      glFogi(GL_FOG_MODE, GL_EXP2); // Inutile avec les shaders
      glFogi(GL_FOG_COORD_SRC, GL_FOG_COORDINATE);
      glFogfv (GL_FOG_COLOR, fogColor);
      glFogf (GL_FOG_DENSITY, fogDensity);

      // -----------------------------
      // Lumière
      glEnable( GL_LIGHTING );
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

      glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
      glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight0 );
      glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight0 );

      /*
      glLightfv( GL_LIGHT1, GL_AMBIENT, ambientLight );
      glLightfv( GL_LIGHT1, GL_DIFFUSE, diffuseLight1 );
      glLightfv( GL_LIGHT1, GL_SPECULAR, specularLight0 );

      glLightfv( GL_LIGHT2, GL_AMBIENT, ambientLight );
      glLightfv( GL_LIGHT2, GL_DIFFUSE, diffuseLight2 );
      glLightfv( GL_LIGHT2, GL_SPECULAR, specularLight0 );

      glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.005f);
      glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
      glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.1f);
      */

#if DEBUG
      /*
      if (intro.manualLight)
      {
	position1[0] = intro.lightPosition[0] + 10.f;
	position1[1] = intro.lightPosition[1] - 10.f;
	position1[2] = intro.lightPosition[2];

	position2[0] = intro.lightPosition[0];
	position2[1] = intro.lightPosition[1] - 10.f;
	position2[2] = intro.lightPosition[2] + 10.f;
      }
      */

      glLightfv(GL_LIGHT0, GL_POSITION, intro.manualLight ? intro.lightPosition : position0);
      /*
      glLightfv(GL_LIGHT1, GL_POSITION, position1);
      glLightfv(GL_LIGHT2, GL_POSITION, position2);
      */
      if (intro.manualLight)
      {
	glPushMatrix();
	glTranslatef(intro.lightPosition[0], intro.lightPosition[1], intro.lightPosition[2]);
	RenderList::renderSingleCube(Shader::parallax, Texture::none);
	glPopMatrix();

	/*
	glPushMatrix();
	glTranslatef(position1[0], position1[1], position1[2]);
	RenderList::renderSingleCube(Shader::parallax, Texture::none);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(position2[0], position2[1], position2[2]);
	RenderList::renderSingleCube(Shader::parallax, Texture::none);
	glPopMatrix();
	*/
      }

#else

      glLightfv( GL_LIGHT0, GL_POSITION, position0 );
      /*
      glLightfv( GL_LIGHT1, GL_POSITION, position1 );
      glLightfv( GL_LIGHT2, GL_POSITION, position2 );
      */

#endif

      // Inutile vu qu'on ne lit pas l'information dans le shader
      glEnable(GL_LIGHT0);
      glEnable(GL_LIGHT1);
      glEnable(GL_LIGHT2);
    }

    // -----------------------------
    /*
    glPushMatrix();
    Shader::list[Shader::parallax].use();
    Texture::list[Texture::test].use(0);
    Texture::list[Texture::testBump2].use(2);
    Texture::list[Texture::none].use(3);
    const int vTanIndex = oglGetAttribLocation(Shader::list[Shader::parallax].id(), "vTan");

    glTranslatef(0, 10.f, 0); immediateArrayDraw(*Mesh::mesh1, vTanIndex);
    glTranslatef(5.f, 0, 0); immediateArrayDraw(*Mesh::mesh2, vTanIndex);
    glTranslatef(5.f, 0, 0); immediateArrayDraw(*Mesh::mesh3, vTanIndex);
    glTranslatef(5.f, 0, 0); immediateArrayDraw(*Mesh::mesh4, vTanIndex);
    glTranslatef(5.f, 0, 0); immediateArrayDraw(*Mesh::mesh5, vTanIndex);

    Texture::list[Texture::tire].use(0);
    Texture::list[Texture::tireBump].use(2);
    Texture::list[Texture::tireSpecular].use(3);
    glTranslatef(-5.f, 0, 5.f); immediateArrayDraw(*Mesh::mesh6, vTanIndex);
    glPopMatrix();
    */
    // -----------------------------

    sceneRL.moveInvisibleAtEnd();
    sceneRL.sort();

#if DEBUG
    Transformation oldCamera = Timeline::getOldCamera(intro);

    if (intro.manualCamera)
    {
      const float * m = intro.cameraPosition;
      oldCamera.s = intro.fov / 255.f;
      oldCamera.dof = intro.dof;
      oldCamera.q = quaternionFromMatrix(m);
      vector3f v = {m[12], m[13], m[14]};
      oldCamera.v = v;
    }
#else
    const Transformation oldCamera = Timeline::getOldCamera(intro);
#endif // DEBUG

    sceneRL.render(intro.now.story, oldCamera);


    // -------------------------------------------------------------
    // Post-processing
    Shader::state.time = intro.now.camera;

    FBO::generateDownscales(intro);
    FBO::finalPass(intro);

//     FBO::showTrails(intro);

    // -------------------------------------------------------------
    // Affichage du texte
    assert(intro.font != NULL);
    assert(intro.xres * intro.yres > 1000);

    Shader::state.time = intro.now.youtube;
    Shader::list[Shader::text].use();
    // glDisable(GL_LIGHTING);
    // glDisable(GL_CULL_FACE);
    // glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, intro.xres, intro.yres, 0);

    Texture::Unit::unUse();
    Timeline::printText(intro);
    glDisable(GL_BLEND);

    // FIXME - processing du texte ici éventuellement

#endif // !DEBUG_TRAILS


#if DEBUG
     return 0;
#else
     return (intro.now.youtube > (3*60 + 21) * 1000); // 3 minutes 21
#endif // !DEBUG
}
