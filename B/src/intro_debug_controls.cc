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

#include "anim.hh"
#include "camera.hh"
#include "keyframe.hh"
#include "light.hh"
#include "lsystem.hh"
#include "lsystemrule.hh"
#include "quaternion.hh"
#include "rope.hh"
#include "../snd/sound.h"
#include "shaders.hh"
#include "timing.hh"
#include "vector.hh"

#include "tweakval.h"

//--------------------------------------------------------

#ifdef DEBUG

LSystem::System * l = NULL;
LSystem::state userSystem = NULL;
/*
Anim::Anim * userSystemAnim1 = NULL;
Anim::Anim * userSystemAnim2 = NULL;
*/

void get_line(char *s, int len, FILE* fp)
{
  fgets(s, len, fp);
  while (*s != '\0' && *s != '\r' && *s != '\n') s++;
  *s = '\0';
}

// Charge un LSystem à partir d'un fichier.
void load_lsystem(char *file)
{
    FILE *fp = fopen(file, "r");
    char s1[301], s2[301], s3[301], s4[301], i[10];
    get_line(s1, 300, fp);
    get_line(s2, 300, fp);
    get_line(s3, 300, fp);
    get_line(s4, 300, fp);
    fgets(i, 9, fp);
    int iter = atoi(i);
    fgets(i, 9, fp);
    float angle = (float)strtod(i, NULL); // - 6.f + 0.008f * (msys_rand() % 1000);
    fgets(i, 9, fp);
    float reduction = (float)strtod(i, NULL);
    fgets(i, 9, fp);
    float forward = (float)strtod(i, NULL);
    fgets(i, 9, fp);
    int nb_cubes = atoi(i);
    fclose(fp);


    const date storyNow = intro.storyNow;
    glPushMatrix();
    l = new LSystem::System(*intro.renderList, storyNow + 1000, storyNow,
			    Shader::default, Anim::none, Texture::none,
			    angle, reduction, forward, nb_cubes,
                            0, 0);
    userSystem = l->iterate(s1, s2, s3, s4, iter);
    l->addStateToList(userSystem);

    glPopMatrix();

/*
    const date firstDuration = 1000;
    const date secondDuration = 500;
    const int firstPropagation = 2;
    const int secondPropagation = 0;
    if (NULL == userSystemAnim2)
    {
      userSystemAnim1 = new Anim::Anim(4, firstDuration);
      userSystemAnim2 = new Anim::Anim(4, secondDuration);

      glPushMatrix();
      glLoadIdentity();
      Anim::Keyframe growStart(0);
      Anim::Keyframe growOver(0.1f, 2.f);
      Anim::Keyframe growMiddle(0.2f);
      Anim::Keyframe growEnd(1.f, 0);
      glPopMatrix();

      userSystemAnim1->add(growStart);
      userSystemAnim1->add(growOver);
      userSystemAnim1->add(growMiddle);
      userSystemAnim1->add(growEnd);
      userSystemAnim1->createCache();

      userSystemAnim2->add(growStart);
      userSystemAnim2->add(growOver);
      userSystemAnim2->add(growMiddle);
      userSystemAnim2->add(growEnd);
      userSystemAnim2->createCache();
    }

    delete l;

    const date now = intro.now;

    l = new LSystem::System(*intro.renderList, now + firstDuration, now,
			    Shader::ion, NULL,
			    angle, reduction, forward, nb_cubes,
			    firstPropagation, firstPropagation);
    userSystem = l->iterate(s1, s2, s3, s4, iter);

    glPushMatrix();
    glLoadIdentity();
    glRotatef(0.1f * (msys_rand() % 3600), 0, 1.f, 0);
    glTranslatef(0, 5.f, 0);
    glRotatef(0.1f * (msys_rand() % 3600), 0, 0, 1.f);
    glScalef(0.05f, 0.05f, 0.05f);
    l->addStateToList(userSystem);

    //l->setAnimation(userSystemAnim2);
    //l->setPropagation(now + firstDuration + 200,
		  //    now + firstDuration + 200 + secondDuration,
		  //    secondPropagation, secondPropagation);
    //l->addStateToList(userSystem);
    glPopMatrix();

    */
}

//---------------------------------------------------------------------

void translateCamera(float x, float y, float z)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(x, y, z);
  glMultMatrixf(intro.cameraPosition);
  glGetFloatv(GL_MODELVIEW_MATRIX, intro.cameraPosition);
  glPopMatrix();
}

void rotateCamera(float angle, float x, float y, float z)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(angle, x, y, z);
  glMultMatrixf(intro.cameraPosition);
  glGetFloatv(GL_MODELVIEW_MATRIX, intro.cameraPosition);
  glPopMatrix();
}

void translateLight(float x, float y, float z)
{
  intro.lightPosition[0] += x;
  intro.lightPosition[1] += y;
  intro.lightPosition[2] += z;
}

unsigned int idOfTransformation(const Transformation & t)
{
  const unsigned int qxi = *(unsigned int *)&t.q.x;
  const unsigned int qyi = *(unsigned int *)&t.q.y;
  const unsigned int qzi = *(unsigned int *)&t.q.z;
  const unsigned int qwi = *(unsigned int *)&t.q.w;

  const unsigned int vxi = *(unsigned int *)&t.v.x;
  const unsigned int vyi = *(unsigned int *)&t.v.y;
  const unsigned int vzi = *(unsigned int *)&t.v.z;

  return (qxi + qyi + qzi + qwi + vxi + vyi + vzi) / 4; // Suppression de 2 bits
}

void printOutCameraFrames()
{
  const Transformation & t1 = intro.cameraAnim->getPrevTransformation(intro.cameraNow, 0);
  const Transformation & t2 = intro.cameraAnim->getNextTransformation(intro.cameraNow, 0);

  DBG("t = %d :", intro.cameraNow);
  DBG("Frame 1: {----, %u,   %ff, %ff, %ff, %ff,   %ff, %ff, %ff,   %d}, // %u",
      unsigned char(t1.s),
      t1.q.x, t1.q.y, t1.q.z, t1.q.w,
      t1.v.x, t1.v.y, t1.v.z,
      (int)(255 * t1.dof), idOfTransformation(t1));

  DBG("Frame 2: {----, %u,   %ff, %ff, %ff, %ff,   %ff, %ff, %ff,   %d}, // %u",
      unsigned char(t2.s),
      t2.q.x, t2.q.y, t2.q.z, t2.q.w,
      t2.v.x, t2.v.y, t2.v.z,
      (int)(255 * t1.dof), idOfTransformation(t2));
}

void printOutManualCamera()
{
  float m[16];
  msys_memcpy(m, intro.cameraPosition, sizeof m);
  invMatrix(m);

  const quaternion q = quaternionFromMatrix(m);
  const vector3f v = {m[12], m[13], m[14]};
  const Transformation t = {1.f, intro.dof, 0, q, v};

  // FIXME : trouver un moyen de mettre le dt correct
  DBG("  {1000, %u,  %ff, %ff, %ff, %ff,  %ff, %ff, %ff, %d}, // %u",
      intro.fov, t.q.x, t.q.y, t.q.z, t.q.w, t.v.x, t.v.y, t.v.z,
      (int)(255.f * t.dof), idOfTransformation(t));
}

void printOutManualLight()
{
  /*
  DBG("Lumière :  {1000, %ff, %ff, %ff, %ff},",
      intro.lightPosition[0],
      intro.lightPosition[1],
      intro.lightPosition[2],
      intro.lightPosition[3]);
  */
  DBG("Lumière :  {1000, %ff, %ff, %ff},",
      intro.lightPosition[0],
      intro.lightPosition[1],
      intro.lightPosition[2]);
}

// Gestion des touches. Actif qu'en debug.
void intro_key(int key, bool shift, bool ctrl)
{
  const float camera_speed = _TV(1.f);
  switch (key)
  {
  case KEY_L:					// Chargement du L-System / Shader
    load_lsystem("lsystem.txt");
    break;
  case KEY_S:
    {
      delete[] Shader::list;
      Shader::loadFirstShader();
      Shader::loadShaders(NULL, 0, 0);
    }
    break;
//   case KEY_M:
//     Shader::broadcast3f(Shader::lightPos, msys_frand(), msys_frand(), msys_frand());
//     break;

  case KEY_ARROW_UP:				// Position de la caméra
    if (intro.manualCamera)
      translateCamera(0, 0, camera_speed);
    break;
  case KEY_ARROW_DOWN:
    if (intro.manualCamera)
      translateCamera(0, 0, -camera_speed);
    break;
  case KEY_ARROW_LEFT:
    if (intro.manualCamera)
      translateCamera(camera_speed, 0, 0);
    break;
  case KEY_ARROW_RIGHT:
    if (intro.manualCamera)
      translateCamera(-camera_speed, 0, 0);
    break;
  case KEY_A:
    if (intro.manualCamera)
      translateCamera(0, camera_speed , 0);
    break;
  case KEY_Q:
    if (intro.manualCamera)
      translateCamera(0, -camera_speed , 0);
    break;

  case KEY_W:					// Angle d'ouverture
    if (intro.manualCamera && intro.fov < 130)
      ++intro.fov;
    break;
  case KEY_X:
    if (intro.manualCamera && intro.fov > 3)
      --intro.fov;
    break;

  case KEY_B:					// Profondeur de champ
    if (intro.manualCamera && intro.dof > 0.05f)
      intro.dof -= 0.05f;
    break;
  case KEY_N:
    if (intro.manualCamera && intro.dof < 0.95f)
      intro.dof += 0.05f;
    break;

  case KEY_D:					// Keyframes courantes de la caméra
    printOutCameraFrames();
    break;
  case KEY_F:					// Dump de la caméra/lumière manuelle
    if (ctrl)
    {
      printOutManualLight();
    }
    else
    {
      printOutManualCamera();
    }
    break;
  case KEY_G:					// Caméra/lumière auto -> manuelle
    if (ctrl)
    {
      // FIXME
    }
    else
    {
      Anim::Anim * cameraAnim = intro.cameraAnim;
      date animDate = intro.cameraNow;
 //     if (animDate >= elevationStartDate && animDate < elevationEndDate)
 //     {
	//cameraAnim = intro.ropeCameraAnim;
	//animDate -= elevationStartDate;
 //     }
      Transformation t = cameraAnim->getTransformation(animDate, 0);
      float m[16];
      computeInvMatrix(m, t.q, t.v);
      msys_memcpy(intro.cameraPosition, m, sizeof m);
      intro.fov = (unsigned char)t.s;
      intro.dof = t.dof;
    }
    break;
  case KEY_H:					// Mode manual / auto
    if (ctrl)
    {
      intro.manualLight = !intro.manualLight;
    }
    else
    {
      intro.manualCamera = !intro.manualCamera;
    }
    break;
  case KEY_C:
    if (ctrl)
    {
      Light::reloadFrames();
    }
    else
    {
      msys_mallocFree(intro.cameraAnim);
      intro.cameraAnim = Camera::reloadAnimation();
    }
    break;

  case KEY_T:
    intro.pauseShift = intro.realNow;
    break;
  case KEY_Y:					// Lecture arrière / avant
    intro.pauseShift += _TV(1000);
    break;
  case KEY_U:
    intro.pauseShift += 10;
    break;
  case KEY_I:
    intro.pauseShift -= 10;
    break;
  case KEY_O:
    intro.pauseShift -= _TV(1000);
    break;
  case KEY_P:					// Pause
    intro.paused = !intro.paused;
    if (intro.paused)
    {
      intro.pauseDate = intro.realNow;
    }
    else
    {
      intro.pauseShift += intro.realNow - intro.pauseDate;
      intro_updateTime();
#if SOUND
      Sound::stop();
      Sound::init();
      Sound::play((intro.youtubeNow + POST_LOAD_DURATION));
#endif
    }
    break;
  }
}

void intro_mouse(const input_mouse & mouse, bool shift, bool ctrl)
{
  if (0 != mouse.buttons[0] && 0 != mouse.buttons[1])
  {
    if (intro.manualCamera)
    {
      const float angleZ = -0.1f * mouse.dx;
      rotateCamera(angleZ, 0, 0, 1.f);
    }
  }
  else if (0 != mouse.buttons[0])
  {
    if (ctrl)
    {
      if (intro.manualLight)
      {
	const float moveX = 0.05f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	const float moveZ = -0.05f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	translateLight(moveX, 0, moveZ);
      }
    }
    else
    {
      if (intro.manualCamera)
      {
	const float angleX = -0.005f * intro.fov * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	const float angleY = -0.005f * intro.fov * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	rotateCamera(angleX, 0, 1.f, 0);
	rotateCamera(angleY, 1.f, 0, 0);
      }
    }
  }
  else if (0 != mouse.buttons[1])
  {
    if (ctrl)
    {
      if (intro.manualLight)
      {
	const float moveY = -0.05f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	translateLight(0, moveY, 0);
      }
    }
    else
    {
      if (intro.manualCamera)
      {
	const float moveX = 0.05f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	const float moveY = -0.05f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	const float speed = shift ? 0.1f : 1.f;
	translateCamera(speed * moveX, speed * moveY, 0);
      }
    }
  }
}

#endif // DEBUG
