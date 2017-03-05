//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//


#include "intro.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include "sys/msys_forbidden.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "sys/events.h"

#include "anim.hh"
#include "camera.hh"
#include "fbos.hh"
#include "files.hh"
#include "interpolation.hh"
#include "light.hh"
#include "lsystem.hh"
#include "picoc_binding.hh"
#include "quaternion.hh"
#include "../snd/sound.h"
#include "server.hh"
#include "shaders.hh"
#include "textures.hh"
#include "timing.hh"
#include "vbos.hh"
#include "vector.hh"
#include "workshop.hh"

#include "tweakval.h"

extern IntroObject intro;

//--------------------------------------------------------

#ifdef DEBUG

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

static void reloadScenes(const char *)
{
  Node::pool().empty();
  for (int i = 0; i < VBO::numberOfVBOs; i++) VBO::elementSize[i] = 0;
  VBO::loadVBOs();
  Workshop::create();
  Anim::loadAnims();
}

// Gestion des touches. Actif qu'en debug.
void intro_key(int key, bool shift, bool ctrl)
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
      intro.showProfiling = !intro.showProfiling;
    }
    else
    {
      tweakval::isEnable = !tweakval::isEnable;
    }
    break;
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

  case KEY_C:
    Texture::buildStickers();
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
    if (intro.manualCamera)
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
      // FIXME
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

  case KEY_T:
    intro.now.pauseDate = intro.now.real;
    intro.now.pauseShift = intro.now.real;
    break;
  case KEY_Y:					// Lecture arrière / avant
    intro.now.pauseShift += _TV(1000);
    break;
  case KEY_U:
    intro.now.pauseShift += (int) (shift_coef * 10.f);
    break;
  case KEY_I:
    intro.now.pauseShift -= (int) (shift_coef * 10.f);
    break;
  case KEY_O:
    intro.now.pauseShift -= _TV(1000);
    break;
  case KEY_P:					// Pause
    intro.now.paused = !intro.now.paused;
    if (intro.now.paused)
    {
      intro.now.pauseDate = intro.now.real;
    }
    else
    {
      if (intro.sound) {
        Sound::stop();
        Sound::init();
      }
      // On fait le stop/init avant, pour tenir compte du temps que ça prend

      intro.now.pauseShift += intro.now.real - intro.now.pauseDate;
      intro_updateTime();

      if (intro.sound)
        Sound::play(intro.now.youtube);
    }
    break;
  case KEY_S:
    if (intro.sound)
      Sound::stop();
    else {
      Sound::init();
      Sound::play(intro.now.youtube);
    }
    intro.sound = !intro.sound;
    break;
  case KEY_Z:
    if (ctrl)
    {
      ++intro.debugRenderMode;
      intro.debugRenderMode %= 4;
    }
    else
    {
      ++intro.showBuffer;
    }
    break;
  case KEY_A:
    ++intro.showTexture;
    break;
  case KEY_Q:
    --intro.showTexture;
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
	const float moveX = 0.5f * mouse.dx * (!shift || abs(mouse.dx) > abs(mouse.dy) ? 1.f : 0);
	const float moveZ = -0.5f * mouse.dy * (!shift || abs(mouse.dx) < abs(mouse.dy) ? 1.f : 0);
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

//  Files::onFileChange("data/lsystem.txt", reloadLSystem);
  Files::onFileChange("data/camera/factory.txt", reloadCamera);
  Files::onFileChange("data/light/factory.txt", Light::reloadFrames);
  Files::onFileChange("shaders/progress.frag", reloadLoadingShaders);
  Files::onFileChange("shaders/progress.vert", reloadLoadingShaders);

  // Rechargement des scènes et vbos
  const char* source_files[] = {
     "data/rail.txt",
     "src/anims.cc",
     "src/armchair.cc",
     "src/bookshelf.cc",
     "src/carousel.cc",
     "src/chess.cc",
     "src/chimney.cc",
     "src/clock.cc",
     "src/desk.cc",
     "src/drum.cc",
     "src/harp.cc",
     "src/lousianne.cc",
     "src/luxo.cc",
     "src/musicbox.cc",
     "src/pascontent.cc",
     "src/orgalame.cc",
     "src/puppet.cc",
     "src/shelf.cc",
     "src/stool.cc",
     "src/table.cc",
     "src/workshop.cc",
     "src/xylotrain.cc",
     "data/revolution.txt",
  };
  for (int i = 0; i < sizeof(source_files) / sizeof(source_files[0]); i++)
    Files::onFileChange(source_files[i], reloadScenes);

  // Rechargement des textures
  for (int i = 0; Texture::textureFilenames[i] != NULL; i++)
  {
    // récupère le chemin après le premier / pour convertir en relatif
    // (car __FILE__ est un chemin en absolu, se terminant par "../data/texture.cc")
    int k;
    for (k = 0; Texture::textureFilenames[i][k] != '/'; k++);
    Texture::textureFilenames[i] += k + 1;
    Files::onFileChange(Texture::textureFilenames[i], Texture::reloadSingleTexture);
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

  Camera::orthoProj(3.f, 3.f / intro.aspectRatio);
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

void debugShowTexture()
{
  // Les textures qu'on veut voir en appuyant sur A
  Texture::id buffers[] = {
    Texture::none,
    Texture::font,
//     Texture::black,
//     Texture::flatBump,
//     Texture::lightBulb,
//     Texture::candle,
//     Texture::aluminium,
//     Texture::brass,
//     Texture::bronze,
//     Texture::gold,
//     Texture::iron,
//     Texture::silver,
    Texture::armchair,
    Texture::armchairBump,
    Texture::brick,
    Texture::ceilBump,
    Texture::clock,
    Texture::doorBump,
    Texture::painting,
    Texture::paintingBump,
    Texture::paintingV,
    Texture::sky,
    Texture::wall,
    Texture::wallBump,
    Texture::paper,
    Texture::pen,
    Texture::penBump,
    Texture::mbCylinder,
    Texture::mbCylinderBump,
    Texture::book,
    Texture::bookBump,
    Texture::greetingsBooks,
    Texture::greetingsBooksBump,
    Texture::flute,
    Texture::fluteOrnaments,
//     Texture::soufflet,
    Texture::souffletBump,
    Texture::puppetHead,
    Texture::puppetBody,
    Texture::puppetBump,
    Texture::flag,
    Texture::trainSign,
    Texture::railBump,
    Texture::orgalameBump,
    Texture::wood,
    Texture::wood_Pauvre,
    Texture::wood_TendreFrais,
    Texture::wood_TendreSec,
    Texture::wood_Lambris,
    Texture::wood_MerisierClair,
    Texture::wood_MerisierClairAliceJuliette,
    Texture::wood_MerisierFonce,
    Texture::wood_Chene,
    Texture::wood_Noyer,
//     Texture::wood6,
    Texture::wood_NoyerB,
//     Texture::wood8,
    Texture::woodBump1,
    Texture::woodBump2,
    Texture::woodBump3,
    Texture::chessBoard,
    Texture::chessBoardBump,
    Texture::irisBokeh,
    Texture::grainMap,
  };
  while (intro.showTexture < 0)
    intro.showTexture += ARRAY_LEN(buffers);
  intro.showTexture %= ARRAY_LEN(buffers);

  if (0 == intro.showTexture)
    return;

  bool opaque = true;
  if (Texture::font == intro.showTexture ||
      Texture::irisBokeh == intro.showTexture ||
      Texture::grainMap == intro.showTexture)
    opaque = false;

  showTexture(buffers[intro.showTexture], 1.95f, 0.05f, opaque);
}

void debugShowBuffer()
{
  // Les textures qu'on veut voir en appuyant sur Z
  Texture::id buffers[] = {
    Texture::none,
    Texture::font,
//     Texture::stickerTest1,
//     Texture::stickerTest2,
//     Texture::stickerTest3,
    Texture::shadowMap,
    Texture::varianceShadowMap,

    Texture::velocityMap,
    Texture::depthMap,
    Texture::postProcessRender,
    Texture::postProcessPong,
    Texture::postProcessPong2,
    Texture::postProcessDownscale2,
    Texture::postProcessDownscale4,
    Texture::postProcessDownscale8,
  };
  intro.showBuffer %= ARRAY_LEN(buffers);

  if (0 == intro.showBuffer)
    return;

  showTexture(buffers[intro.showBuffer], 0.05f, 0.05f, false);
}

#endif // !STATIC_SHADERS

void debugShowRenderableList()
{
  if (!intro.showProfiling)
    return;

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

    glLoadIdentity();
    glTranslatef(10.f + float(i), 20.f, 0);

    // Vivant ?
    if (i > intro.sceneRenderList->_lastVisible)
      glColor3f(0, 0, 0);
    else
      glColor3f(0, 1.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Shader ?
    glColor3fv(&colors[3 * (((int)element.shaderId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Texture ?
    glColor3fv(&colors[3 * (((int)element.textureId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // Texture de bump ?
    glColor3fv(&colors[3 * (((int)element.bumpTextureId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);

    // VBO ?
    glColor3fv(&colors[3 * (((int)element.vboId()) % 12)]);
    glTranslatef(0, 16.f, 0);
    glDrawArrays(GL_QUADS, 0, 4);
  }
  glDisableClientState(GL_VERTEX_ARRAY);
}

#endif // DEBUG
