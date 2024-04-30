#include "intro.hh"

#include "algebra/quaternion.hh"
#include "algebra/vector2.hxx"
#include "algebra/vector3.hxx"
#include "algebra/vector4.hxx"
#include "allTextures.hh"
#include "anim.hxx"
#include "anims.hh"
#include "array.hxx"
#include "camera.hh"
#include "demo.hh"
#include "fbos.hh"
#include "files.hh"
#include "interpolation.hxx"
#include "light.hh"
#include "lsystem.hh"
#include "materials.hh"
#include "meshes.hh"
#include "picoc_binding.hh"
#include "project_loader.hh"
#include "server.hh"
#include "shaders.hh"
#include "snd/sound.hh"
#include "textures.hh"
#include "timing.hh"
#include "vbos.hh"

#include "tweakval.hh"

#include "sys/msys.h"
#include "sys/msys_forbidden.h"
#include <GL/glu.h>
#include "sys/events.h"

extern IntroObject intro;

#ifdef DEBUG

void setDebugFlags(const char*)
{
  intro.debug.nodesUpdate                  = (_TV(1) != 0);
  intro.debug.animation                    = (_TV(1) != 0);
  intro.debug.renderDrawcall               = (_TV(1) != 0);
  intro.debug.shadowMapDrawcall            = (_TV(1) != 0);
  intro.debug.applyShadowMapPostProcessing = (_TV(1) != 0);
  intro.debug.generateDownScale            = (_TV(1) != 0);
  intro.debug.generateStreak               = (_TV(1) != 0);
  intro.debug.generateGlow                 = (_TV(1) != 0);
  intro.debug.generateBokeh                = (_TV(0) != 0);
  intro.debug.finalPass                    = (_TV(1) != 0);
  intro.debug.lensOrb                      = (_TV(1) != 0);
  intro.debug.displayOverlayText           = (_TV(1) != 0);
  intro.debug.showCameraRepresentation     = (_TV(1) != 0);
}

//--------------------------------------------------------

LSystem::System * l = NULL;
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
Array<Node *> * load_lsystem(char *file)
{
    FILE *fp = fopen(file, "r");
    Array<char> s1(301);
    Array<char> s2(301);
    Array<char> s3(301);
    Array<char> s4(301);
    char i[10];
    get_line(s1.elt, 300, fp);
    get_line(s2.elt, 300, fp);
    get_line(s3.elt, 300, fp);
    get_line(s4.elt, 300, fp);
    fgets(i, 9, fp);
    int iterations = atoi(i);
    fgets(i, 9, fp);
    float angle = (float)strtod(i, NULL); // - 6.f + 0.008f * (msys_rand() % 1000);
    fgets(i, 9, fp);
    float reduction = (float)strtod(i, NULL);
    fgets(i, 9, fp);
    float forward = (float)strtod(i, NULL);
    fgets(i, 9, fp);
    int nb_cubes = atoi(i);
    fclose(fp);

    if (l != NULL) delete l;
    l = new LSystem::System();
    l->setRules(s1.elt, s2.elt, s3.elt, s4.elt);
    l->develop(iterations);
    Array<Node *> * nodes = l->generateNodes(angle, reduction, forward, nb_cubes,
					     worldStartDate, -1, 0, 0);

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

    l = new LSystem::System(*intro.sceneRenderList, now + firstDuration, now,
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
    return nodes;
}

//---------------------------------------------------------------------

void translateCamera(float x, float y, float z)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(x, y, z);
  glMultMatrixf(intro.cameraPosition.m);
  getFromModelView(intro.cameraPosition);
  glPopMatrix();
}

void rotateCamera(float angle, float x, float y, float z)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glRotatef(angle, x, y, z);
  glMultMatrixf(intro.cameraPosition.m);
  getFromModelView(intro.cameraPosition);
  glPopMatrix();
}

void reloadCamera(const char *)
{
  // on doit initialiser avant de détruire pour éviter un assert dans array
  if (intro.cameraAnim->isCached() == false)
    intro.cameraAnim->createCache();

  delete intro.cameraAnim;
  intro.cameraAnim = Camera::reloadAnimation();
}

void reloadLSystem(const char *)
{
  delete intro.sceneRenderList;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Array<Node *> * nodes = load_lsystem("data\\lsystem.txt");
  delete nodes;
  intro.sceneRenderList = RenderList::createFromScene(Node::pool());
}

void translateLight(float dx, float dy, float dz)
{
  intro.lightPosition[0] += dx;
  intro.lightPosition[1] += dy;
  intro.lightPosition[2] += dz;
}

void rotateLight(float dLat, float dLon)
{
  const vector3f pos(intro.lightPosition);

  const float r = norm(pos);
  const float theta = msys_min(PI, msys_max(0.f, msys_acosf(pos.y / r) + dLat));
  const float phi = msys_atan2f(pos.z, pos.x) + dLon;

  const vector3f newPos = vector3f(msys_sinf(theta) * msys_cosf(phi),
				   msys_cosf(theta),
				   msys_sinf(theta) * msys_sinf(phi)) * r;
  intro.lightPosition[0] = newPos.x;
  intro.lightPosition[1] = newPos.y;
  intro.lightPosition[2] = newPos.z;
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

void printCamera(const Camera::Camera & c)
{
  const compressedQuaternion cq = compressQuaternion(c.t.q);
  int prec = _TV(18); // precision des floats en bits

  DBG("{    0, %u,   %d, %d, %d, %d,   %ff, %ff, %ff,   %d}, // %u",
      unsigned char(c.fov),
      cq.x, cq.y, cq.z, cq.w,
      roundb(c.t.v.x, prec), roundb(c.t.v.y, prec), roundb(c.t.v.z, prec),
      int(255.f * c.focus), idOfTransformation(c.t));
}

void printOutCameraFrames()
{
  const Camera::Camera & c1 = intro.cameraAnim->getPrev(intro.now.camera);
  const Camera::Camera & c2 = intro.cameraAnim->getNext(intro.now.camera);

  DBG("t = %d :", intro.now.camera);
  printCamera(c1);
  printCamera(c2);
  DBG("--------");
}

void printOutManualCamera()
{
  Camera::Camera camera;
  matrix4 inv = intro.cameraPosition;
  invMatrix(inv);
  camera.t = computeTransformation(inv);
  camera.fov = intro.fov;
  camera.focus = intro.focus;

  printCamera(camera);
}

void printOutManualLight()
{
  DBG("Lumière :  {1000, %ff, %ff, %ff, %ff},",
      intro.lightPosition[0],
      intro.lightPosition[1],
      intro.lightPosition[2],
      intro.lightPosition[3]);
}

void debugActivateLightOptions()
{
  if (intro.debug.lightingOnly)
  {
    Shader::uniforms[Shader::retroParams].set(vector4f(1.f, 1.f, 1.f, 0.f));
    Shader::uniforms[Shader::retroEdgeColor].set(vector4f(0.f, 0.f, 0.f, 1.f));
    Shader::uniforms[Shader::retroFaceColor].set(vector4f(1.f, 1.f, 1.f, 1.f));
  }
  if (!intro.debug.light0)
  {
    Shader::uniforms[Shader::l0Color].set(vector4f(0.f, 0.f, 0.f, 1.f));
  }
  if (!intro.debug.light1)
  {
    Shader::uniforms[Shader::l1Color].set(vector4f(0.f, 0.f, 0.f, 1.f));
  }
  if (!intro.debug.ambient)
  {
    Shader::uniforms[Shader::ambient].set(vector4f(0.f, 0.f, 0.f, 1.f));
  }
  if (!intro.debug.lensEffects)
  {
    Shader::uniforms[Shader::vignetting].set(vector2f::zero);
    Shader::uniforms[Shader::lensOrbsColor].set(vector4f::zero);
    Shader::uniforms[Shader::lensFlareColor].set(vector4f::zero);
    Shader::uniforms[Shader::lensGlowColor].set(vector4f::zero);
    Shader::uniforms[Shader::lensStreakColor].set(vector4f::zero);
  }
}

static void reloadScenes(const char *)
{
  if (_TV(1))
  {
    for (int i = 0; i < VBO::numberOfVBOs; i++) VBO::elementSize[i] = 0;
    VBO::loadVBOs();
  }
  if (_TV(1))
  {
    Node::pool().empty();
    Demo::create();
  }
}

void relativeSeek(int delta)
{
  intro.now.pauseShift += delta;
  Intro::updateTime();
  if (intro.sound && !intro.now.paused)
    Sound::play(intro.now.youtube);
  if (intro.now.youtube < 20000)
    Demo::preloadFFT(intro.now.youtube);
}

void absoluteSeek(int time)
{
  intro.now.pauseDate = intro.now.real;
  intro.now.pauseShift = intro.now.real - time;
  relativeSeek(0);
}

void pause()
{
  if (intro.now.paused) return;
  intro.now.paused = true;
  if (intro.sound) Sound::stop();
  intro.now.pauseDate = intro.now.real;
}

void play()
{
  if (!intro.now.paused) return;
  intro.now.paused = false;
  // On fait le stop/init avant, pour tenir compte du temps que ça prend
  if (intro.sound)
  {
    Sound::stop();
    Sound::init();
  }
  relativeSeek(intro.now.real - intro.now.pauseDate);
}

// Gestion des touches. Actif qu'en debug.
void intro_key(KeyCode key, bool shift, bool ctrl)
{
  const float shift_coef = shift ? 0.2f : 1.f;
  const float camera_speed = _TV(10.f) * shift_coef;
  switch (key)
  {
  case KEY_ARROW_UP:				// Position de la caméra
    if (intro.manualCamera)
      if (ctrl)
      {
	translateCamera(0, camera_speed , 0);
      }
      else
      {
	translateCamera(0, 0, camera_speed);
      }
    break;
  case KEY_ARROW_DOWN:
    if (intro.manualCamera)
      if (ctrl)
      {
	translateCamera(0, -camera_speed , 0);
      }
      else
      {
  	translateCamera(0, 0, -camera_speed);
      }
    break;
  case KEY_M:
    if (ctrl)
    {
      intro.debug.showProfiling = !intro.debug.showProfiling;
    }
    else
    {
#if TV_USE_TWEAKVAL
      tweakval::isEnable = !tweakval::isEnable;
#endif
    }
    break;
  case KEY_F1: intro.debug.lightingOnly = !intro.debug.lightingOnly; break;
  case KEY_F2: intro.debug.light0 = !intro.debug.light0; break;
  case KEY_F3: intro.debug.light1 = !intro.debug.light1; break;
  case KEY_F4: intro.debug.ambient = !intro.debug.ambient; break;
  case KEY_F5: intro.debug.shadows = !intro.debug.shadows; break;
  case KEY_F6: intro.debug.lensEffects = !intro.debug.lensEffects; break;

  case KEY_ARROW_LEFT:
    if (intro.manualCamera)
      translateCamera(camera_speed, 0, 0);
    break;
  case KEY_ARROW_RIGHT:
    if (intro.manualCamera)
      translateCamera(-camera_speed, 0, 0);
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
    if (intro.manualCamera)
    {
      intro.focus -= (shift ? 1.f/255.f : 0.05f);
      if (intro.focus < 0.f)
	intro.focus = 0.f;
    }
    break;
  case KEY_N:
    if (ctrl)
    {
      if (intro.manualLight)
      {
	intro.lightPosition[3] = float(intro.lightPosition[3] == 0);
      }
    }
    else if (intro.manualCamera)
    {
      intro.focus += (shift ? 1.f/255.f : 0.05f);
      if (intro.focus > 1.f)
	intro.focus = 1.f;
    }
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
      intro.lightPosition[0] = Light::light0Position[0];
      intro.lightPosition[1] = Light::light0Position[1];
      intro.lightPosition[2] = Light::light0Position[2];
      intro.lightPosition[3] = Light::light0Position[3];
    }
    else
    {
      Camera::Camera camera = intro.cameraAnim->get(intro.now.camera);

      intro.cameraPosition = computeInvMatrix(camera.t);
      intro.fov = (unsigned char)camera.fov;
      intro.focus = camera.focus;
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

  case KEY_T: absoluteSeek(0); break; // Retour au début
  case KEY_Y: relativeSeek(_TV(1000)); break; // Lecture arrière / avant
  case KEY_U: relativeSeek((int) (shift_coef * 10.f)); break;
  case KEY_I: relativeSeek((int) (shift_coef * -10.f)); break;
  case KEY_O: relativeSeek(-_TV(1000)); break;
  case KEY_P: if (intro.now.paused) play(); else pause(); break;
  case KEY_S:
    Intro::updateTime();
    if (intro.sound)
      Sound::stop();
    else {
      Sound::init();
      Sound::play(intro.now.youtube);
    }
    intro.sound = !intro.sound;
    break;
  case KEY_Z:
    {
      int next = (intro.debug.displayMode + 1) % numberOfDisplayModes;
      intro.debug.displayMode = (displayMode)next;
    }
    break;
  case KEY_A:
    switch (intro.debug.displayMode)
    {
    case displayScene:
      {
	int next = (intro.debug.renderMode + 1) % numberOfRenderModes;
	intro.debug.renderMode = (renderMode)next;
      }
    case displayBuffer:
      ++intro.debug.showBuffer;
      break;
    case displayTexture:
      ++intro.debug.showTexture;
      break;
    case displayMaterial:
      ++intro.debug.showMaterial;
      break;
    case displayMesh:
      ++intro.debug.showMesh;
      break;
    }
    break;
  case KEY_Q:
    switch (intro.debug.displayMode)
    {
    case displayScene:
      {
	int prev = (intro.debug.renderMode - 1);
	if (prev < 0)
	  prev = numberOfRenderModes - 1;
	intro.debug.renderMode = (renderMode)prev;
      }
    case displayBuffer:
      --intro.debug.showBuffer;
      break;
    case displayTexture:
      --intro.debug.showTexture;
      break;
    case displayMaterial:
      --intro.debug.showMaterial;
      break;
    case displayMesh:
      --intro.debug.showMesh;
      break;
    }
    break;
  }
}

void executeCommand(int command, const char* args)
{
  DBG("Command: %d", command);
  switch (command)
  {
  case 1: relativeSeek(1000); break;
  case 2: relativeSeek(-1000); break;
  case 3: pause(); break;
  case 4: play(); break;
  case 5:
    intro.manualCamera = !intro.manualCamera;
    break;
  case 6:
    absoluteSeek(atoi(args));
  case 7:
    sscanf(args, "%d %d", &intro.now.loopDate, intro.now.loopDuration);
    intro.now.looping = intro.now.loopDuration != 0;
  default: break;
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
	if (intro.lightPosition[3] == 0)
	{
	  const float dLon = 0.01f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	  const float dLat = 0.01f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	  rotateLight(dLat, dLon);
	}
	else
	{
	  const float moveX = 0.5f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	  const float moveZ = -0.5f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	  translateLight(moveX, 0, moveZ);
	}
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
	const float moveY = -0.5f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	translateLight(0, moveY, 0);
      }
    }
    else
    {
      if (intro.manualCamera)
      {
	const float moveX = 0.5f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	const float moveY = -0.5f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
	const float speed = shift ? 0.1f : 1.f;
	translateCamera(speed * moveX, speed * moveY, 0);
      }
    }
  }

  translateCamera(0, /* (shift ? 0.1f : 1.f) */ 100.f * mouse.dz, 0);
}

void reloadLoadingShaders(const char *)
{
  Loading::setupRendering();
}

void debugToolsInit()
{
  Server::init();
  Picoc::init();

  Files::onFileChange("src/debug_controls.cc", setDebugFlags);

//  Files::onFileChange("data/lsystem.txt", reloadLSystem);
  Files::onFileChange("data/camera/factory.txt", reloadCamera);
  Files::onFileChange("data/light/light0.txt", Light::reloadFrames);
  Files::onFileChange("data/light/light1.txt", Light::reloadFrames);
  Files::onFileChange("shaders/progress.frag", reloadLoadingShaders);
  Files::onFileChange("shaders/progress.vert", reloadLoadingShaders);

  Files::onFileChange("data\\project", ProjectLoader::load);

  // Rechargement des scènes et vbos
  const char* meshFiles[] = {
     "data/highwaySlice.txt",
     "data/highway1.txt",
     "data/highway2.txt",
     "data/highway3.txt",
     "data/highway4.txt",
     "data/highway5.txt",
     "data/highway6.txt",
  };
  for (int i = 0; i < ARRAY_LEN(meshFiles); ++i)
    Files::onFileChange(meshFiles[i], reloadScenes);

  // Rechargement des scènes et vbos
  HANDLE hFind;
  WIN32_FIND_DATA data;
  hFind = FindFirstFile("src/specific/*.cc", &data);
  if (hFind != INVALID_HANDLE_VALUE) {
    do {
      char file[200] = "src/specific/";
      strcat(file, data.cFileName);
      Files::onFileChange(file, reloadScenes);
    } while (FindNextFile(hFind, &data));
    FindClose(hFind);
  }

  // Rechargement des textures
  for (int i = 0; AllTextures::textureFilenames[i] != NULL; i++)
  {
    // convertit le chemin en relatif (les chemins se terminent par "data/textures/file.cc")
    const char * file = strstr(AllTextures::textureFilenames[i], "data");
    Files::onFileChange(file, Texture::reloadSingleTexture);
  }
  Files::onFileChange("src/textures.cc", Texture::reloadSingleTexture);

  // Rechargement des meshes
  for (int i = 0; Meshes::meshBuilders[i].file != NULL; i++)
  {
    // convertit le chemin en relatif (les chemins se terminent par "data/textures/file.cc")
    const char * file = strstr(Meshes::meshBuilders[i].file, "data");
    Files::onFileChange(file, Meshes::reloadSingleMesh);
  }
}

// Appelée à chaque frame en debug
void debugTools()
{
  static long lastCheck = 0;
  long now = msys_timerGet();
  if (now >= lastCheck && now - lastCheck < 100)
    return;
  lastCheck = now;

  // Ce code est appelé toutes les 100ms
  ReloadChangedTweakableValues();
  Files::checkFileChanges();
  Server::doIO();
}

#if !STATIC_SHADERS

void showTexture(Texture::id textureId, float x, float y, bool opaque)
{
  FBO::unUse();

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glColor3f(1.f, 1.f, 1.f);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  if (textureId == Texture::shadowMap ||
      textureId == Texture::depthMap)
  {
    Shader::list[Shader::debugNonLinear].use();
  }
  else
  {
    Shader::list[Shader::noPost].use();
  }
  VBO::Element::unUse();

  Camera::orthoProj(intro.aspectRatio, 1.f);
  glScalef(0.8f, 0.8f, 0.8f);
  glTranslatef(x, y, 0);

  Texture::Unit & src = Texture::list[textureId];
  src.use(Texture::color);

  float w = 1.f;
  float h = float(src.height()) / float(src.width());
  if (h > w)
  {
    w = 1.f / h;
    h = 1.f;
  }
  const float vertices[] =
    {
      0, 0, 0,   0,
      0, h, 0,   1.f,
      w, h, 1.f, 1.f,
      w, 0, 1.f, 0
    };

  if (opaque)
  {
    glDisable(GL_BLEND);
  }
  else
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);
  glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glDrawArrays(GL_QUADS, 0, 4);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);


  Shader::list[Shader::text].use();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Camera::orthoProj(intro.xres, intro.yresVP);

  Texture::Unit::unUse();
  glColor4f(1.f, 0.8f, 1.f, 1.f);
  intro.font->print(Texture::list[textureId].description,
		    0.5f * intro.textHeight,
		    20.f + 0.33f * x * intro.xres,
		    10.f + intro.textHeight + 0.33f * y * intro.xres);
  char resolution[20];
  sprintf(resolution, "%dx%d", Texture::list[textureId].width(), Texture::list[textureId].height());
  intro.font->print(resolution,
		    0.5f * intro.textHeight,
		    20.f + 0.33f * x * intro.xres,
		    10.f + 0.33f * y * intro.xres);
  glDisable(GL_BLEND);
}

void debugShowBuffer()
{
  // Les textures qu'on veut voir en appuyant sur Z
  Texture::id buffers[] = {
    Texture::colorBuffer,
    Texture::depthMap,
    Texture::velocityMap,

    Texture::downScaleRender,
    Texture::glowBuffer,
    Texture::hStreakBuffer1,
    Texture::vStreakBuffer1,

    Texture::shadowMap,
    Texture::varianceShadowMap,

    Texture::postProcessDownscale4,

    Texture::particlesPositionsPing,
    Texture::particlesPositionsPong,
    Texture::fftMap,
  };
  while (intro.debug.showBuffer < 0)
    intro.debug.showBuffer += ARRAY_LEN(buffers);
  intro.debug.showBuffer %= ARRAY_LEN(buffers);

  showTexture(buffers[intro.debug.showBuffer], 0.1f, 0.1f, false);
}

void debugShowTexture()
{
  // Les textures qu'on veut voir en appuyant sur A
  Texture::id buffers[] = {
    Texture::font,
    Texture::roundBokeh,
    Texture::grainMap,
    //Texture::titleScreen,
  };
  while (intro.debug.showTexture < 0)
    intro.debug.showTexture += ARRAY_LEN(buffers);
  intro.debug.showTexture %= ARRAY_LEN(buffers);

  bool opaque = true;
  if (Texture::font == buffers[intro.debug.showTexture] ||
      Texture::roundBokeh == buffers[intro.debug.showTexture] ||
      Texture::grainMap == buffers[intro.debug.showTexture])
    opaque = false;

  showTexture(buffers[intro.debug.showTexture], 0.1f, 0.1f, opaque);
}

void debugShowObject(VBO::id mesh, Material::Element material)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(_TV(90.f), intro.aspectRatio, intro.zNear, intro.zFar);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(_TV(0.f), _TV(0.f), _TV(-400.f));
  glRotatef(_TV(0.f), 1.f, 0, 0);
  glRotatef(_TV(0.f), 0, 1.f, 0);
  glRotatef(_TV(0.f), 0, 0, 1.f);
  RenderList::renderObject(mesh, material);
  glPopMatrix();
}

void debugShowMaterial()
{
  if (intro.debug.showMaterial < 0)
    intro.debug.showMaterial = Material::numberOfMaterials - 1;
  intro.debug.showMaterial %= Material::numberOfMaterials;

  //debugShowObject(VBO::debugBidule, Material::list[intro.debug.showMaterial]);
}

void debugShowMesh()
{
  if (intro.debug.showMesh < 0)
    intro.debug.showMesh = VBO::numberOfVBOs - 1;
  intro.debug.showMesh %= VBO::numberOfVBOs;

  debugShowObject((VBO::id)intro.debug.showMesh, Material::list[Material::none]);


  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);

  Shader::list[Shader::text].use();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Camera::orthoProj(intro.xres, intro.yresVP);

  Texture::Unit::unUse();
  glColor4f(1.f, 0.8f, 1.f, 1.f);
  intro.font->print(VBO::list[intro.debug.showMesh].description,
		    0.5f * intro.textHeight,
		    20.f + 0.033f * intro.xres,
		    10.f + intro.textHeight + 0.033f * intro.xres);
  char weight[20];
  sprintf(weight, "%d vertices", VBO::list[intro.debug.showMesh].numberOfVertices);
  intro.font->print(weight,
		    0.5f * intro.textHeight,
		    20.f + 0.033f * intro.xres,
		    10.f + 0.033f * intro.xres);
  glDisable(GL_BLEND);
}

void debugShowRenderableList()
{
  FBO::unUse();
  VBO::Element::unUse();
  Shader::Program::unUse();
  Texture::Unit::unUse();

  Camera::orthoProj(intro.xres, intro.yres);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  const float vertices[] =
    {
      0, 0,
      0, 15.f,
      1.f, 15.f,
      1.f, 0,
    };
  const float colors[] =
    {
      1.f, 0.f, 1.f,
      1.f, 0.f, .5f,
      1.f, 0.f, 0.f,
      1.f, .5f, 0.f,
      1.f, 1.f, 0.f,
      .5f, 1.f, 0.f,
      0.f, 1.f, 0.f,
      0.f, 1.f, .5f,
      0.f, 1.f, 1.f,
      0.f, .5f, 1.f,
      0.f, 0.f, 1.f,
      .5f, 0.f, 1.f,
    };

  glVertexPointer(2, GL_FLOAT, 2 * sizeof(float), vertices);
  glEnableClientState(GL_VERTEX_ARRAY);


  Array<RenderableContainer> & list = intro.sceneRenderList->_renderables;

  int stop = list.size;
  if (stop > intro.xres - 20)
    stop = intro.xres - 20;

  for (int i = 0; i < stop; ++i)
  {
    const Renderable & element = *(list[i].renderable);
    const Material::Element & material = element.material();

    glLoadIdentity();
    glTranslatef(10.f + float(i), 20.f, 0);

    // Vivant ?
    if (i > intro.sceneRenderList->_lastVisible)
      glColor3f(0, 0, 0);
    else
      glColor3f(0, 1.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Shader ?
    glColor3fv(&colors[3 * (((int)material.shaderId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Texture ?
    glColor3fv(&colors[3 * (((int)material.textureId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Texture de bump ?
    glColor3fv(&colors[3 * (((int)material.bumpTextureId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // VBO ?
    glColor3fv(&colors[3 * (((int)element.vboId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);
  }
  glDisableClientState(GL_VERTEX_ARRAY);
}

void debugShowCameraInfo()
{
  FBO::unUse();
  VBO::Element::unUse();
  Shader::Program::unUse();
  Texture::Unit::unUse();

  Camera::orthoProj(1.f, 1.f);

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f(1.f, 1.f, _TV(0.5f), _TV(10.5f));

  glBegin(GL_LINES);

  // Repères pour les trois tiers
  glVertex2f(0.f, 0.333f); glVertex2f(1.f, 0.333f);
  glVertex2f(0.f, 0.667f); glVertex2f(1.f, 0.667f);
  glVertex2f(0.333f, 0.f); glVertex2f(0.333f, 1.f);
  glVertex2f(0.667f, 0.f); glVertex2f(0.667f, 1.f);

  const float hDash = _TV(0.025f);
  const float vDash = hDash * intro.aspectRatio;

  // Réticule
  glVertex2f(0.5f - hDash, 0.5f); glVertex2f(0.5f + hDash, 0.5f);
  glVertex2f(0.5f, 0.5f - vDash); glVertex2f(0.5f, 0.5f + vDash);

  // Repères pour l'alignement horizontal
  glVertex2f(0.000f        , 0.5f); glVertex2f(0.000f + hDash, 0.5f);
  glVertex2f(0.333f - hDash, 0.5f); glVertex2f(0.333f + hDash, 0.5f);
  glVertex2f(0.667f - hDash, 0.5f); glVertex2f(0.667f + hDash, 0.5f);
  glVertex2f(1.000f - hDash, 0.5f); glVertex2f(1.000f        , 0.5f);

  // Repères pour l'alignement vertical
  glVertex2f(0.5f, 0.000f        ); glVertex2f(0.5f, 0.000f + vDash);
  glVertex2f(0.5f, 0.333f - vDash); glVertex2f(0.5f, 0.333f + vDash);
  glVertex2f(0.5f, 0.667f - vDash); glVertex2f(0.5f, 0.667f + vDash);
  glVertex2f(0.5f, 1.000f - vDash); glVertex2f(0.5f, 1.000f        );

  // Repères pour le format carré
  glColor4f(1.f, _TV(-0.5f), _TV(-0.5f), _TV(0.1f));
  glVertex2f(0.5f + 0.5f / intro.aspectRatio, 0.f); glVertex2f(0.5f + 0.5f / intro.aspectRatio, 1.f);
  glVertex2f(0.5f - 0.5f / intro.aspectRatio, 0.f); glVertex2f(0.5f - 0.5f / intro.aspectRatio, 1.f);

  glEnd();

  /*
  Camera::orthoProj(intro.xres, intro.yresVP);
  char cameraInfo[20];
  sprintf(cameraInfo, "fov: %.1f", intro.fov);
  intro.font->print(cameraInfo,
		    0.5f * intro.textHeight,
		    20.f, intro.yres - 20.f - 0.5f * intro.textHeight);
  */
}

void debugShowOverlayInfo()
{
  switch (intro.debug.displayMode)
  {
  case displayBuffer:
    debugShowBuffer();
    break;
  case displayTexture:
    debugShowTexture();
    break;
  case displayMaterial:
    debugShowMaterial();
    break;
  case displayMesh:
    debugShowMesh();
    break;
  }

  if (intro.debug.showProfiling)
  {
    debugShowRenderableList();
  }
  if (intro.manualCamera)
  {
    debugShowCameraInfo();
  }
}

#endif // !STATIC_SHADERS

#endif // DEBUG
