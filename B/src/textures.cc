//
// Tous les VBOs utilisés
//

#include "textures.hh"
#include "textures_urban.hh"

#include "sys/msys.h"

#include "array.hh"
#include "textureid.hh"
#include "texgen/texture.hh"

namespace Texture
{
  Unit * list = NULL;

#ifdef DEBUG

  static void buildTestTexture();
  static void buildBumpTestTexture();

#endif // DEBUG

  static void buildPlainTextures();
  static void buildDefaultBump();
//   static void buildDEMO();

  // Usine
  static void buildDangerStripes();
  static void buildConcrete();
  static void buildConcreteBump();
  static void buildConcreteSpecular();
  static void buildAntiSlip();
  static void buildManipulatorAndValidator();
  static void buildRubiks();
  static void buildWeightedCompanionCube();
  static void buildMario();
  static void buildCrate();

  // Ville
  static void buildCityAndForestSky();
  static void buildRoad();
  static void buildPavement();
  static void buildGrass();
  static void buildMoss();
  static void buildContainer();
  static void buildWindows1();
  static void buildWalls();
  static void buildTaxiCar();
  static void buildCarBump();
  static void buildTrafficLights();
  static void buildPharmacy();
  static void buildStatue();

  // Forêt
  static void buildRosace();

  // Espace
  static void buildSpaceBox();

  // Ramp
  static void buildRampBox();
  static void buildRampBox2();

  static void buildTransitionBurn();

  static void setupRenderTextures();
  static void setupRenderDepthTextures();

  static unsigned int getCurrentBuilderIndex();

  typedef void(*textureBuilder)();

  const textureBuilder threadUnsafeBuilders[] =
    {

#ifdef DEBUG

      buildTestTexture,

#endif // DEBUG

      buildPlainTextures,
      buildMario
    };

  const textureBuilder threadSafeBuilders[] =
    {
#ifdef DEBUG

      buildBumpTestTexture,

#endif // DEBUG

      buildWalls,  // au debut de la liste, car tres long

      buildDefaultBump,
//       buildDEMO,
      buildDangerStripes,
      buildConcrete,
      buildConcreteBump,
      buildConcreteSpecular,
      buildAntiSlip,
      buildManipulatorAndValidator,
      buildRubiks,
      buildWeightedCompanionCube,
      buildCrate,

      buildCityAndForestSky,
      buildRoad,
      buildPavement,
      buildGrass,
      buildContainer,
      buildWindows1,

      buildTaxiCar,
      buildCarBump,
      buildTrafficLights,
      buildPharmacy,
      buildStatue,

      buildSpaceBox,
      buildRampBox,
      buildRampBox2,

      buildMoss,
      buildRosace,

      buildTransitionBurn,

      setupRenderTextures,
      setupRenderDepthTextures
    };


  Channel * perlin7 = NULL;

  Channel *getPerlin7()
  {
    if (perlin7 == NULL)
    {
      perlin7 = new Channel();
      perlin7->Perlin(7, 1, 1, 1.f);
    }
    return perlin7;
  }

  void createTextureList()
  {
    list = new Unit[numberOfTextures];
  }


  typedef struct
  {
    id		texId;
    Channel *	r;
    Channel *	g;
    Channel *	b;
    Channel *	a;
    GLint	min;
    GLint	max;
    GLfloat	maxAnisotropy;
    GLint	tiling;
  } textureDescription;

  Array<textureDescription> * textureQueue = NULL;

  CRITICAL_SECTION builderMutex;
  CRITICAL_SECTION textureMutex;
  static unsigned int currentBuilderIndex = 0;
  static int currentTextureIndex = 0;
  static unsigned threadsNumber = 2;

  static void callBuilders(void*);
  static void dequeueCurrentTexture(textureDescription* desc);
  static void queueTexture(id texId,
			   const Channel & a,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling);

  static void queueTexture(id texId,
			   const Channel & r,
			   const Channel & g,
			   const Channel & b,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling);

  static void queueTexture(id texId,
			   const Channel & r,
			   const Channel & g,
			   const Channel & b,
			   const Channel & a,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling);


  static float maxAnisotropy = 0;

  void loadTextures(Loading::ProgressDelegate * pd, int low, int high)
  {
    assert(list != NULL);

    textureQueue = new Array<textureDescription>(numberOfTextures);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

    DBG("Anisotropie max : %.2f", maxAnisotropy);
    maxAnisotropy = min(maxAnisotropy, 8.f);
    DBG("Anisotropie choisir : %.2f", maxAnisotropy);

    getPerlin7();

    //
    // Création des données des textures qui ne peuvent être que dans
    // le thread principal
    //
    const unsigned numberOfThreadUnsafeBuilders = sizeof(threadUnsafeBuilders) / sizeof(textureBuilder);
    for (unsigned int i = 0; i < numberOfThreadUnsafeBuilders; ++i)
    {
      DBG("loading texture %u...", i);
      const long int t1 = GetTickCount();
      threadUnsafeBuilders[i]();
      const long int t2 = GetTickCount();
      DBG("-> %d ms", t2 - t1);
//       pd->func(pd->obj, currentLd); currentLd += ldStep;
      OGL_ERROR_CHECK("loadTextures i = %u", i);
    }


    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    threadsNumber = sysinfo.dwNumberOfProcessors;
    DBG("textures: using %u threads", threadsNumber);

    const unsigned int numberOfThreadSafeBuilders = sizeof(threadSafeBuilders) / sizeof(textureBuilder);
    const int ldStep = (high - low) / (numberOfThreadSafeBuilders + 1);
    unsigned int builderIndex;
    textureDescription* desc;
    long* threadsIds = new long[threadsNumber];

    InitializeCriticalSection(&builderMutex);
    InitializeCriticalSection(&textureMutex);

    const long int t1 = GetTickCount();

    for (unsigned t = 0; t < threadsNumber; ++t)
      threadsIds[t] = msys_threadNew(&callBuilders);

    do
    {
      builderIndex = getCurrentBuilderIndex();

      pd->func(pd->obj, low + ldStep * (builderIndex > numberOfThreadSafeBuilders ? numberOfThreadSafeBuilders : builderIndex));

      desc = 0;
      EnterCriticalSection(&textureMutex);
      if (currentTextureIndex < textureQueue->size)
	desc = &(*textureQueue)[currentTextureIndex++];
      LeaveCriticalSection(&textureMutex);

      if (desc)
        dequeueCurrentTexture(desc);
    }
    while (desc || (builderIndex < numberOfThreadSafeBuilders + threadsNumber));

    for (unsigned t = 0; t < threadsNumber; ++t)
      msys_threadDelete(threadsIds[t]);

    const long int t2 = GetTickCount();
    DBG("all textures done (total: %d ms)", t2 - t1);

    DeleteCriticalSection(&textureMutex);
    DeleteCriticalSection(&builderMutex);

    delete[] threadsIds;
    delete textureQueue;

    pd->func(pd->obj, high);
  }

  // FIXME : destruction des textures


  // ==========================================================================


  static void setupTextureFormatForRendering(id texId, GLenum format)
  {
//    Channel empty;
    Unit & target = list[texId];
//    target.setData(empty, empty, empty);
    target.setFiltering(GL_LINEAR, GL_LINEAR);
    target.setTiling(GL_CLAMP, GL_CLAMP);
    target.setFormat(format);
//    target.load();
  }

  static void setupRenderTextures()
  {
    setupTextureFormatForRendering(screenCubeRender, GL_RGBA);
    setupTextureFormatForRendering(postProcessRender, GL_RGBA);
  }

  static void setupRenderDepthTextures()
  {
//     setupTextureFormatForRendering(screenCubeDepth, GL_DEPTH_COMPONENT);
    setupTextureFormatForRendering(postProcessDepth, GL_DEPTH_COMPONENT);
  }

  static void sendTextureToCard(id texId,
				const Channel & a,
				GLint min, GLint max,
				GLfloat maxAnisotropy,
				GLint tiling)
  {
    Unit & target = list[texId];
    target.setData(a);
    target.setFiltering(min, max, maxAnisotropy);
    target.setTiling(tiling, tiling);
    target.load();
  }

  static void sendTextureToCard(id texId,
				const Channel & r,
				const Channel & g,
				const Channel & b,
				GLint min, GLint max,
				GLfloat maxAnisotropy,
				GLint tiling)
  {
    Unit & target = list[texId];
    target.setData(r, g, b);
    target.setFiltering(min, max, maxAnisotropy);
    target.setTiling(tiling, tiling);
    target.load();
  }

  static void sendTextureToCard(id texId,
				const Channel & r,
				const Channel & g,
				const Channel & b,
				const Channel & a,
				GLint min, GLint max,
				GLfloat maxAnisotropy,
				GLint tiling)
  {
    Unit & target = list[texId];
    target.setData(r, g, b, a);
    target.setFiltering(min, max, maxAnisotropy);
    target.setTiling(tiling, tiling);
    target.load();
  }


  // --------------------------------------------------------------------------

  static unsigned int getCurrentBuilderIndex()
  {
    EnterCriticalSection(&builderMutex);
    const unsigned int index = currentBuilderIndex;
    LeaveCriticalSection(&builderMutex);
    return index;
  }

  static void callBuilders(void*)
  {
    unsigned int i;
    const unsigned int numberOfThreadSafeBuilders = sizeof(threadSafeBuilders) / sizeof(textureBuilder);

    while (true)
    {
      EnterCriticalSection(&builderMutex);
      i = currentBuilderIndex++;
      LeaveCriticalSection(&builderMutex);

      if (i >= numberOfThreadSafeBuilders)
	break;

      DBG("texture %u: begin", i);
      const long int t1 = GetTickCount();
      threadSafeBuilders[i]();
      const long int t2 = GetTickCount();
      DBG("texture %u: end (%d ms)", i, t2 - t1);
    }
  }

  static void queueTexture(id texId,
			   const Channel & a,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling)
  {
    textureDescription desc;
    desc.texId = texId;
    desc.r = NULL;
    desc.g = NULL;
    desc.b = NULL;
    desc.a = new Channel(a);
    desc.min = min;
    desc.max = max;
    desc.maxAnisotropy = maxAnisotropy;
    desc.tiling = tiling;

    EnterCriticalSection(&textureMutex);
    textureQueue->add(desc);
    LeaveCriticalSection(&textureMutex);
  }

  static void queueTexture(id texId,
			   const Channel & r,
			   const Channel & g,
			   const Channel & b,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling)
  {
    textureDescription desc;
    desc.texId = texId;
    desc.r = new Channel(r);
    desc.g = new Channel(g);
    desc.b = new Channel(b);
    desc.a = NULL;
    desc.min = min;
    desc.max = max;
    desc.maxAnisotropy = maxAnisotropy;
    desc.tiling = tiling;

    EnterCriticalSection(&textureMutex);
    textureQueue->add(desc);
    LeaveCriticalSection(&textureMutex);
  }

  static void queueTexture(id texId,
			   const Channel & r,
			   const Channel & g,
			   const Channel & b,
			   const Channel & a,
			   GLint min, GLint max,
			   GLfloat maxAnisotropy,
			   GLint tiling)
  {
    textureDescription desc;
    desc.texId = texId;
    desc.r = new Channel(r);
    desc.g = new Channel(g);
    desc.b = new Channel(b);
    desc.a = new Channel(a);
    desc.min = min;
    desc.max = max;
    desc.maxAnisotropy = maxAnisotropy;
    desc.tiling = tiling;

    EnterCriticalSection(&textureMutex);
    textureQueue->add(desc);
    LeaveCriticalSection(&textureMutex);
  }

  static void dequeueCurrentTexture(textureDescription* desc)
  {
    assert(desc);

    DBG("texture %u: sending to opengl", currentTextureIndex - 1);

    if (NULL == desc->r)
    {
      assert(desc->a != NULL);
      sendTextureToCard(desc->texId,
			*desc->a,
			desc->min, desc->max,
			desc->maxAnisotropy,
			desc->tiling);
      delete desc->a;
    }
    else if (NULL == desc->a)
    {
      assert(desc->r != NULL && desc->g != NULL && desc->b != NULL);
      sendTextureToCard(desc->texId,
			*desc->r,
			*desc->g,
			*desc->b,
			desc->min, desc->max,
			desc->maxAnisotropy,
			desc->tiling);
      delete desc->r;
      delete desc->g;
      delete desc->b;
    }
    else
    {
      assert(desc->r != NULL && desc->g != NULL && desc->b != NULL && desc->a != NULL);
      sendTextureToCard(desc->texId,
			*desc->r,
			*desc->g,
			*desc->b,
			*desc->a,
			desc->min, desc->max,
			desc->maxAnisotropy,
			desc->tiling);
      delete desc->r;
      delete desc->g;
      delete desc->b;
      delete desc->a;
    }
    desc->r = NULL;
    desc->g = NULL;
    desc->b = NULL;
    desc->a = NULL;
  }

  //
  // Bump mapping
  //

  static void buildAndQueueBumpMapFromHeightMap(id texId, Channel & h, bool repeat = true)
  {
    Channel r = h;
    Channel g = h;

    if (repeat)
    {
      r.Translatei(-1, 0);
      g.Translatei(0, 1);
    }
    else
    {
      r.ClampedTranslatei(-1, 0);
      g.ClampedTranslatei(0, 1);
    }
    r -= h; r.Scale(0, 1.f);
    g -= h; g.Scale(0, 1.f);
    Channel b;
    b.Flat(2.);
    b -= r;
    b -= g;

    queueTexture(texId,
		 r, g, b, h,
 		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy,
		 repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  }

  // ==========================================================================

  //
  // Diagonales à 45° (FIXME : tester en remplaçant par un fade + mod)
  //

  static void obliqueCosinus(Channel & dest, unsigned char freq)
  {
    Channel temp;
    dest.Cosinus(freq, freq);
    temp.Sinus(freq, freq);
    dest += temp;
  }

  //
  // Random puis blur
  //
  static void randomThenBlur(Channel & t)
  {
    t.Random();
    t.Blur();
  }

  //
  // Random puis gaussian blur
  //
  static void randomThenGaussian(Channel & t)
  {
    t.Random();
    t.GaussianBlur();
  }

  //
  // Random puis motion blur
  //
  static void randomThenMotion(Channel & t, float vblur, float hblur)
  {
    t.Random();
    t.HorizontalMotionBlur(hblur);
    t.VerticalMotionBlur(vblur);
  }

  //
  // De passes de motion blur
  //
  static void motionMotion(Channel & t, float first, float second)
  {
    t.HorizontalMotionBlur(first);
    t.VerticalMotionBlur(first);
    t.HorizontalMotionBlur(second);
    t.VerticalMotionBlur(second);
  }


  // ==========================================================================


#ifdef DEBUG

  static unsigned char testTextureALPHA[4] = { 0, 255, 255, 0 };
  static unsigned char testTextureRGBA[16] = { 255,   0,   0, 255,
					       255, 255, 255,   0,
					         0, 255,   0, 127,
					         0,   0, 255, 255 };


  //
  // Texture de test
  // Faites ce que vous voulez ici ; ne sera pas compilé en RELEASE
  //
  static void buildTestTexture()
  {
//     Channel a;
//     Channel b;
//     Channel c;
//     a.Gradient(1.1f, 0.3f, 0.2f, 0.6f);
//     b.Cells(42);
//     c.Conic();

    Unit & target = list[test];
//     target.setData(a, b, c);
    target.setData(2, 2, GL_RGBA, testTextureRGBA);
    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_REPEAT, GL_REPEAT);
    target.load();
 }

  static void buildBumpTestTexture()
  {
    Channel m;
    m.Square();
    m.ClampAndScale(0.8f, 0.5f);

    Channel c;
    c.Conic();
    c.ClampAndScale(0.9f, 0.95f); c *= 0.2f;
    c.Translate(-0.383f, -0.383f);

    m += c;

    c.Translate(0.25f, 0);
    m += c;
    c.Translate(0.25f, 0);
    m += c;
    c.Translate(0.25f, 0);
    m += c;

    m.Scale(0, 1.f);

    buildAndQueueBumpMapFromHeightMap(testBump, m);
  }

#endif // DEBUG

  typedef struct
  {
    id			textureId;
    unsigned char	r;
    unsigned char	g;
    unsigned char	b;
    unsigned char	a;
  } plainTextureDescription;

  static void buildPlainColor(const plainTextureDescription & desc)
  {
    Unit & target = list[desc.textureId];
    const unsigned char col[] = { desc.r, desc.g, desc.b, desc.a };

    target.setData(1, 1, GL_RGBA, col);
    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP, GL_CLAMP);
    target.load(false);
  }

  static void buildPlainColor(Texture::id id,
			      unsigned char a)
  {
    Unit & target = list[id];

    target.setData(1, 1, GL_ALPHA, &a);
    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP, GL_CLAMP);
    target.load(false);
  }

  static const plainTextureDescription plainTextureDescs[] =
    {
      {none,		255, 255, 255, 255},
      {highParticle,	255, 225,  97,  50},
      {thunderBolt,	161, 187, 255, 180},
      {smoke,		214, 177, 199, 240},
      {black,	          0,   0,   0, 255},
      {darkGray,	 13,  13,  13, 255},
      {whiteCar,	238, 238, 238, 255},
      {redCar,		160,  45,  45, 255},
      {blueCar,		 75, 100, 140, 255},
      {blackCar,	 50,  50,  50, 255},
      {greyCar,		190, 190, 190, 255},
      {glowingSodium,	255, 174,  40,   0},
      {fuchsia,		227,   0,  76, 255},
    };

  static void buildPlainTextures()
  {
    const unsigned int numberOfPlainTextures = sizeof(plainTextureDescs) / sizeof(plainTextureDescription);
    for (unsigned int i = 0; i < numberOfPlainTextures; ++i)
    {
      buildPlainColor(plainTextureDescs[i]);
    }
    buildPlainColor(defaultSpecular, 0);
  }

  static void buildDefaultBump()
  {
    Texture::Channel t;
    t.Fade(0);
    t.Pow(1/6.f);

    Texture::Channel tmp = t;
    tmp.VerticalFlip();
    t += tmp;

    t.Scale(0, 1);
    tmp = t;
    tmp.Rotate90();

    t *= tmp;

    buildAndQueueBumpMapFromHeightMap(defaultBump, t, false);
  }


  /*
  static void buildNoise()
  {
    Channel r;
    r.Random();
    r.HorizontalMotionBlur(0.01f);
    r.VerticalMotionBlur(0.01f);
    r.Scale(0, 1.f);
  }
  */

  /*
  //
  // "DEMO" sur aluminium brossé
  //

  static void buildDEMO()
  {
    Channel bloc;
    bloc.Square();
    bloc.ClampAndScale(0.8f, 0.805f);

    Channel o_;
    o_.Conic();
    o_.ClampAndScale(0.8f, 0.805f);

    Channel d_;
    {
      Channel cutleft;
      cutleft.Fade(90);
      cutleft.ClampAndScale(0.5f, 0.502f);
      Channel cutright = cutleft;
      cutright.Scale(1.f,0);

      cutright *= bloc;
      cutleft *= o_;
      d_ = cutright;
      d_ += cutleft;
    }

    Channel diam;
    diam.Fade(0);
    {
      Channel tri;
      tri.Fade(90);
      tri += diam;
      tri.ClampAndScale(1.f, 0.995f);

      diam = tri;
      diam.Rotate90();
      diam.MaximizeTo(tri);
    }
    diam.Scale(1.f, 0);

    diam.MaximizeTo(bloc);
    Channel m_ = diam;

    Channel e_ = m_;
    e_.Rotate90();

    d_.Translate(0.3125f, 0);
    e_.Translate(0.107f, 0);
    m_.Translate(-0.1f, 0);
    o_.Translate(-0.308f, 0);

    Channel demo_ = d_;
    demo_ += e_;
    demo_ += m_;
    demo_ += o_;

    buildAndQueueBumpMapFromHeightMap(demoBump, demo_);
    demo_.Emboss();

    Channel conic;
    conic.Conic();
    demo_ *= conic;

    Channel vfade;
    vfade.Fade(0);
    vfade.Scale(1.f, 0.5f);

    demo_.Mix(vfade, 0.5f);

    Channel brush;
    brush.Random();

    brush.HorizontalMotionBlur(0.04f);
    brush.Blur();
    brush.Scale(-0.03f, 0.03f);

    demo_ += brush;

    // ----------------------------------
    demo_.GaussianBlur();
    demo_.GaussianBlur();
    demo_.Scale(0, 1.f);

    // ----------------------------------

    queueTexture(demo,
		      demo_, demo_, demo_,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_CLAMP);
  }
  */

  //
  // Lignes jaunes et noires de chantier peintes sur du béton, avec des impacts
  //

  static void buildDangerStripes()
  {
    // Hachures
    Channel hb;
    obliqueCosinus(hb, 3);
    hb.Clamp(-0.05f, 0.05f);
    hb.Rotate90();

    Channel rnd;
    randomThenGaussian(rnd);
    rnd.Bump();
    rnd.Scale(-0.15f, 0.15f);

    Channel hr = hb; hr.Scale(0, 0.886f);
    Channel hg = hb; hg.Scale(0, 0.765f);
    hb.Scale(0, 0.176f);

    hr += rnd;
    hg += rnd;
    hb += rnd;


    // Impacts dans le béton
    randomThenMotion(rnd, 0.004f, 0.029f);
    rnd.Scale(0, 1.f);
    rnd.HorizontalMotionBlur(0.01f);
    rnd.ClampAndScale(0, 0.38f);

    // Specular map
    queueTexture(dangerStripesSpecular,
		 rnd,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    // Béton
    Channel beton;
    beton.Flat(0.7f);
//     Channel beton = rnd;
//     beton.Bump();
//     beton.Scale(0.5f, 1.f);

//     rnd.Clamp(0.7f, 1.f);
    rnd.Scale(1.f, 0);

    // Final
    hr.Mask(rnd, beton);
    hg.Mask(rnd, beton);
    hb.Mask(rnd, beton);

    queueTexture(dangerStripes,
		 hr, hg, hb,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildConcrete()
  {
    Channel t;
    randomThenMotion(t, 0.019f, 0.019f);
    motionMotion(t, 0.098f, 0.039f);

    t.Scale(0.65f, 0.7f);

    Channel rnd;
    randomThenMotion(rnd, 0.01f, 0.019f);
    rnd.HorizontalMotionBlur(0.01f);
    rnd.ClampAndScale(0.49f, 0); rnd *= -0.07f;

    t += rnd;

    randomThenMotion(rnd, 0.019f, 0.039f);
    rnd.HorizontalMotionBlur(0.019f);
    rnd.ClampAndScale(0.49f, 0.4f); rnd *= -0.05f;

    t += rnd;

    queueTexture(concrete,
		      t, t, t,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_REPEAT);
  }

  static void buildConcreteBump()
  {
    Channel t;
    randomThenMotion(t, 0.004f, 0.019f);
    t.HorizontalMotionBlur(0.01f);
    t.ClampAndScale(0, 0.44f);
    buildAndQueueBumpMapFromHeightMap(concreteBump, t);
  }

  static void buildConcreteSpecular()
  {
    Channel t;
    t.Random();
    t.ClampAndScale(0.9f, 1.f); t *= 0.3f;

    queueTexture(concreteSpecular,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  //
  // Plaque de métal avec motif antidérapant
  //

  static void buildAntiSlip()
  {
    Channel shape;
    obliqueCosinus(shape, 4); // Lignes obliques dans une direction

    {
      Channel stripes2 = shape; // Lignes obliques dans l'autre direction
      shape.Rotate90();
      {
	Channel satStripes = shape;
	satStripes.Clamp(-0.05f, 0.05f);
	satStripes.Scale(-1.f, 1.f);
	stripes2 *= satStripes;
	stripes2.Clamp(-1.f, 0);

	satStripes.Rotate90();
	shape *= satStripes;
      }
      shape.Clamp(0, 1.f);
      shape += stripes2;
    }

    {
      // Motifs anti dérapants

      Channel diag;
      obliqueCosinus(diag, 32);
      diag.Scale(3.f, 0); // Décallage de phase

      Channel shape2 = shape;
      shape2.Scale(1.f, -1.f);

      shape *= diag;

      diag.Rotate90();
      shape2 *= diag;


      shape.Clamp(0, 1.f);
      shape2.Clamp(0, 1.f);

      shape += shape2;
    }
    shape.ClampAndScale(0.5f, 0.95f);

    // Bump map
    buildAndQueueBumpMapFromHeightMap(antiSlipBump, shape);

    // Specular map
    shape.ClampAndScale(0.9f, 1.f);

    queueTexture(antiSlipSpecular,
		 shape,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    // Aluminium brossé
    Channel brush;
    brush.Random();
    brush.HorizontalMotionBlur(0.006f);
    brush.Blur();
    brush.Scale(0.5f, 0.8f);

    queueTexture(antiSlip,
		      brush, brush, brush,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_REPEAT);
  }

#define DEG2RAD 0.0174532f

  static float dcos(float x, float d)
  {
    return d * msys_cosf(DEG2RAD * x) / 2.f + 0.5f;
  }
  static float dsin(float x, float d)
  {
    return d * sinf(DEG2RAD * x) / 2.f + 0.5f;
  }

  //
  // Gros cube manipulateur en acier, et cubes de validation
  //
  static void buildManipulatorAndValidator()
  {
    Channel m;
    Channel v;

    // Cadre
    // ================================
    {
      Channel h;
      h.Fade(90); h *= 2.f; h.Mod();
      h.ClampAndScale(0.2f, 0.3f);
      h.Scale(1.f, 0);
      h *= h;
      h.Scale(1.f, 0);

      Channel v = h;
      v.Rotate90();
      h *= v;

      h.Scale(0.8f, 0);

      m += h;
    }
    v = m;

    // FIXME : relief du voyant central

    // Impacts
    // ================================
    {
      Channel r;
      r.Random();
      r.Scale(0.f, 0.93f);
      r.AddCuts(0.06f, 0.019f);
      r.AddCuts(0.06f, 0.058f);
      r.AddCuts(0.06f, 0.078f);
      r.GaussianBlur();
      r.Clamp(0.62f, 1.f);

      Channel rnd;
      randomThenBlur(rnd);
      rnd.Scale(0.9f, 1.f);
      r *= rnd;
      r.Scale(0.1f, 0);

      m += r; // Le manipulateur a des impacts partout
      v.MinimizeTo(r); // Le validateur n'a d'impact qu'au centre
    }

    // Rivets
    // ================================
    {
      Channel c;
      c.Conic();
      c.Clamp(0.96f, 1.f);
      c.Translate(-0.445f, -0.445f);

      Channel l = c;
      for (unsigned int i = 0; i < 6; ++i)
      {
	c.Translate(0.125f, 0);
	l += c;
      }

      c = l;
      l.Rotate90(); c += l;
      l.Rotate90(); c += l;
      l.Rotate90(); c += l;

      c.Scale(1.0f, 0);
      c *= c;
      c.Scale(1.f, 0.8f);
      l = c;
      l.ClampAndScale(0.8f, 0.801f);
      c.MaximizeTo(l);

      m.MinimizeTo(c);

      c.Scale(0.5f, 1.f);

      queueTexture(manipulatorSpecular,
		   c,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

    buildAndQueueBumpMapFromHeightMap(manipulatorBump, m);
    buildAndQueueBumpMapFromHeightMap(validatorBump, v);


    // Hachures
    // ================================
    {
      Channel h;
      obliqueCosinus(h, 5);
      h.ClampAndScale(-0.05f, 0.05f);
      m.ClampAndScale(0.81f, 0.8f);

      // FIXME : gné ? Comment ça on utilise m, qui a servit au bump,
      // pour la couleur ?
      m += h;
    }

    // Réticule
    // ================================
    {
      v.Fade(0); v *= 2.f; v.Mod();
      v.ClampAndScale(0.905f, 0.9f);

      Channel s = v; s.Rotate90();
      v.MaximizeTo(s);

      s.Square(); s.ClampAndScale(0.2f, 0.205f);

      v.MinimizeTo(s);
    }

    // Cadre central
    // ================================
    {
      Channel t;
      t.Square(); t *= 2.f; t.Mod();
      t.ClampAndScale(0.91f, 0.9f);
      m.MaximizeTo(t);
      v.MaximizeTo(t);
    }

    m.Clamp(0, 1.f);
    Channel rm; rm.Flat(0.886f);
    Channel gm; gm.Flat(0.765f);
    Channel bm; bm.Flat(0.176f);
    rm *= m;
    gm *= m;
    bm *= m;

    Channel rv; rv.Flat(1.f);
    Channel gv; gv.Flat(1.f);
    Channel bv; bv.Flat(1.f);
    rv *= v;
    gv *= v;
    bv *= v;

    // Voyant lumineux
    // ================================
    {
      Channel t;
      t.Square();
      t.ClampAndScale(0.65f, 0.655f);

      const float lightColors[] =
	{
	  1.0f, 0.4f, 0.0f, // orange
	  0.5f, 0.5f, 0.5f, // éteint
	  0.0f, 1.0f, 0.5f, // vert
	  1.0f, 0.1f, 0.0f  // rouge
	};

      const Texture::id targets[] =
	{
	  manipulator,
	  validator,
	  validatorOK,
	  validatorKO
	};

      Channel a = t; a.Scale(1.f, 0.5f);
      Channel lowa = t; lowa.Scale(1.f, 0.8f);
      for (unsigned int i = 0; i < 4; ++i)
      {
	const unsigned int index = 3 * i;
	Channel lr; lr.Flat(lightColors[index]);
	Channel lg; lg.Flat(lightColors[index + 1]);
	Channel lb; lb.Flat(lightColors[index + 2]);

	Channel r = (0 == i ? rm : rv);
	Channel g = (0 == i ? gm : gv);
	Channel b = (0 == i ? bm : bv);
	r.Mask(t, lr);
	g.Mask(t, lg);
	b.Mask(t, lb);

	queueTexture(targets[i],
		     r, g, b, (1 == i ? lowa : a),
		     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		     maxAnisotropy,
		     GL_REPEAT);
      }
    }
  }

  //
  // Rubik's cube
  //
  static void buildRubiks()
  {
    Channel t;
    t.Fade(0); t *= 6.f; t.Mod();
    t.ClampAndScale(0., 0.2f);

    {
      Channel v = t;
      v.Rotate90();
      t *= v;
    }
    t.GaussianBlur();

    t.Scale(0, 1.f);
    Channel hole = t;
    hole.ClampAndScale(0, 0.3f);

    t.ClampAndScale(0.4f, 0.5f);

    queueTexture(rubiks,
		 t, t, t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    t.Scale(0.6f, 0.85f);
    queueTexture(rubiksSpecular,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    t.Scale(0, 0.2f);
    t += hole;
    t.Scale(0, 1.f);
    buildAndQueueBumpMapFromHeightMap(rubiksBump, t);

  }

  //
  // Weighted companion cube
  //
  static void buildWeightedCompanionCube()
  {

    // -----------------------------------
    Channel con;
    con.Conic();

    Channel shield = con;
    shield.ClampAndScale(0.55f, 1.f);
    shield.Pow(0.5f);

    {
      Channel shield2 = con;
      shield2.ClampAndScale(0.56f, 0.6f);
      shield2.Pow(0.5f);
      shield += shield2;
    }
    shield.Scale(0, 0.5f);

    Channel contour = con;
    contour.ClampAndScale(0.25f, 0.05f);

    {
      Channel h;
      h.Fade(0); h *= 2.f; h.Mod();
      {
	Channel v = h;
	v.Rotate90();
	h.MaximizeTo(v);
	h.Clamp(0, 0.05f);
	h.Scale(0.2f, 1.f);
	contour *= h;
      }
    }

    // -----------------------------------

    Channel h;
    h.Fade(0); h *= 2.f; h.Mod();
    {
      Channel v = h;
      v.Rotate90();
      h.MinimizeTo(v);
    }
    Channel glow = h;

    {
      Channel h2 = h;
      h2.Clamp(0.8f, 0.82f);
      h2.Scale(0, 0.5f);

      h.ClampAndScale(0.75f, 0.55f);
      h.MinimizeTo(h2);
    }

    h.MaximizeTo(contour);
    h.MinimizeTo(shield);

    buildAndQueueBumpMapFromHeightMap(companionCubeBump, h);

    // -----------------------------------

    Channel mask = h;
    mask.ClampAndScale(0, 0.05f);

    glow -= mask;
    glow.ClampAndScale(0.985f, 0.98f);
    Channel a = glow;
    a.Scale(0.4f, 1.f);


    Channel greyr;
    greyr.Flat(0.55f);

    {
      Channel fg; fg.Flat(0.85f);
      greyr.Mask(mask, fg);
    }

    Channel greyg = greyr;
    Channel greyb = greyr;

    // A ce stade rgb représentent les deux gris

    mask.Scale(0.3f, 0.8f);
    queueTexture(companionCubeSpecular,
		 mask,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    // -----------------------------------
    // Version Aperture
    {
      Texture::Channel ap;
      Texture::Channel f1;
      Texture::Channel f2;
      for (unsigned int i = 0; i <8; ++i)
      {
	f1.Fade(i * 45.f + 15.f);
	f1.ClampAndScale(0.56f, 0.565f);
	f2.Fade(i * 45.f - 30.f);
	f2.ClampAndScale(0.545f, 0.54f);
	f1 *= f2;
	ap += f1;
      }
      Texture::Channel limit = con;
      limit.ClampAndScale(0.75f, 0.76f);
      ap *= limit;
      ap.Scale(1.f, 0.8f);

      Channel r; r.Flat(0); r.Mask(glow, greyr);
      Channel g; g.Flat(0.81f); g.Mask(glow, greyg);
      Channel b; b.Flat(0.79f); b.Mask(glow, greyb);
      r *= ap;
      g *= ap;
      b *= ap;

      queueTexture(storageCube,
		   r, g, b, a,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

    // -----------------------------------
    // Version companion cube
    {
      Channel cut;
      cut.Fade(0);
      cut.ClampAndScale(0.462f, 0.463f);
      {
	Channel bleft;
	bleft.Fade(45.f);
	bleft.ClampAndScale(0.551f, 0.55f);
	{
	  Channel bright = bleft;
	  bright.Rotate90();
	  bleft.MaximizeTo(bright);
	}
	cut.MaximizeTo(bleft);
      }
      {
	Channel disk = con;
	disk.ClampAndScale(0.855f, 0.86f);
	disk.Translate(0.055f, 0.087f);
	{
	  Channel disk2 = disk;
	  disk2.HorizontalFlip();
	  disk.MinimizeTo(disk2);
	}
	cut.MinimizeTo(disk);
      }
      {
	Channel exclude;
	exclude.Square();
	exclude.ClampAndScale(0.5f, 0.501f);
	cut.MaximizeTo(exclude);
      }
      cut.Translate(0, -0.05f);
      cut.Scale(1., 0);
      cut *= glow;

      Channel r; r.Flat(0.63f); r.Mask(cut, greyr);
      Channel g; g.Flat(0.47f); g.Mask(cut, greyg);
      Channel b; b.Flat(0.55f); b.Mask(cut, greyb);

      queueTexture(companionCube,
		   r, g, b, a,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

    //======================================
  }

  //
  // Cube Mario
  //
  static void buildMario()
  {
    const unsigned char colors[16 * 16] =
      {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	2, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 1, 0, 0, 1,
	2, 0, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 1, 0, 0, 1,
	2, 0, 0, 0, 2, 2, 1, 0, 0, 0, 2, 2, 1, 0, 0, 1,
	2, 0, 0, 0, 2, 2, 1, 0, 0, 0, 2, 2, 1, 0, 0, 1,
	2, 0, 0, 0, 2, 2, 1, 1, 1, 1, 2, 2, 0, 0, 0, 1,
	2, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 1,
	2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
	2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
      };
    unsigned char data[3 * 16 * 16];
    for (unsigned int i = 0; i < 256; ++i)
    {
      const unsigned char color = colors[i];
      const unsigned int index = 3 * i;
      if (0 == color)
      {
	data[index    ] = 252;
	data[index + 1] = 165;
	data[index + 2] = 66;
      }
      else if (2 == color)
      {
	data[index    ] = 232;
	data[index + 1] = 89;
	data[index + 2] = 15;
      }
      else
      {
	data[index    ] = 0;
	data[index + 1] = 0;
	data[index + 2] = 0;
      }
    }

    Unit & target = list[mario];
    target.setData(16, 16, GL_RGB, data);
    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP, GL_CLAMP);
    target.load(false);
  }

  //
  // Caisse en bois
  //
  static void buildCrate()
  {
    // =================================
    // Motif du bois
    Channel per;
    per.Perlin(4, 4, 1, 1);					// Perlin 4

    {
      Channel per2;
      per2.Perlin(13, 1, 1, 1);					// Perlin 13
      per2.Scale(-0.03f, 0.03f);
      per += per2;
    }

    per.Scale(0, 1.f);
    per.Cut(8);
    per.Pow(2);
    per.Scale(1.f, 0);

    // =================================
    // Planches

    Channel h;
    h.Fade(0); h *= 8.f; h.Mod();
    h.ClampAndScale(0.5f, 0.505f);
    h.Translate(0, 0.064f);

    Channel v;
    v.Fade(90); v *= 2.f; v.Mod();
    v.ClampAndScale(0.2f, 0.205f);

    Channel p1 = h;
    p1.MaximizeTo(per);
    p1.MaximizeTo(v);

    h.Scale(1.f, 0);
    Channel p2 = h;
    per.HorizontalFlip();
    per.Translate(0.137f, 0);
    p2.MaximizeTo(per);
    p2.MaximizeTo(v);

    v.Scale(1.f, 0);
    Channel p3 = v;
    per.Rotate90();
    p3.MaximizeTo(per);

    Channel p = p1;
    p.MinimizeTo(p2);
    p.MinimizeTo(p3);
    p.Scale(0, 1.f);

    // p : motif complet des planches, pour atténuer le bump
    p.Scale(0.3f, 0.1f);

    queueTexture(crateSpecular,
		 p,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
    // Black Mesa
    // ==========
    Channel bm;
    {
      Channel tmp;

      // Pente
      // ==============================
      bm.Fade(120.f);
      bm.ClampAndScale(0.6f, 0.605f);

      // Horizon
      // ==============================
      tmp.Fade(0.f);
      tmp.ClampAndScale(0.505f, 0.5f);
      bm.MinimizeTo(tmp);

      // Falaise
      // ==============================
      tmp.Fade(90.f);
      tmp.ClampAndScale(0.405f, 0.4f);
      bm.MinimizeTo(tmp);

      // Contre-bas
      // ==============================
      tmp.Fade(0.f);
      tmp.ClampAndScale(0.653f, 0.65f);
      bm.MaximizeTo(tmp);


      // Cerclage
      // ==============================
      tmp.Conic();
      tmp.ClampAndScale(0.505f, 0.495f);
      bm.MinimizeTo(tmp);

      {
	Channel tmp2;
	tmp2.Conic();
	tmp2.ClampAndScale(0.575f, 0.585f);
	tmp2.MinimizeTo(tmp);
	bm.MaximizeTo(tmp2);

	tmp = p;
	tmp2 = bm;
	tmp2.Scale(0.7f, 0);
	tmp.MinimizeTo(tmp2);
      }

      queueTexture(crateBlackMesaSpecular,
		   tmp,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

    // =================================
    // Couleurs

    // clair 1 : 0.99, 0.82, 0.53
    // foncé 1 : 0.74, 0.43, 0.16
    Channel r1 = p1; r1.Scale(0.74f, 0.99f);
    Channel g1 = p1; g1.Scale(0.43f, 0.82f);
    Channel b1 = p1; b1.Scale(0.16f, 0.53f);

    // clair 2 : 1., 0.81, 0.5
    // foncé 2 : 0.85, 0.53, 0.19
    Channel r2 = p2; r2.Scale(0.85f, 1.f);
    Channel g2 = p2; g2.Scale(0.53f, 0.81f);
    Channel b2 = p2; b2.Scale(0.19f, 0.5f);

    Channel r3 = p3; r3.Scale(0.85f, 1.f);
    Channel g3 = p3; g3.Scale(0.53f, 0.81f);
    Channel b3 = p3; b3.Scale(0.19f, 0.5f);

    Channel r = r1; r.MinimizeTo(r2); r.MinimizeTo(r3);
    Channel g = g1; g.MinimizeTo(g2); g.MinimizeTo(g3);
    Channel b = b1; b.MinimizeTo(b2); b.MinimizeTo(b3);

    // =================================
    // Grain du bois

    Channel hrnd;
    randomThenBlur(hrnd);
    hrnd.HorizontalMotionBlur(0.01f);

    Channel vrnd = hrnd;
    vrnd.Rotate90();

    vrnd.MaximizeTo(v);
    v.Scale(1., 0);
    hrnd.MaximizeTo(v);

    Channel rnd = hrnd;
    rnd.MinimizeTo(vrnd);

    // On atténue le bump sur les nervures
    rnd.Scale(-1.f, 1.f);
    p.Scale(0.1f, 1.f);
    rnd *= p;

    rnd.Scale(-0.1f, 0);

    // On ajoute la différence du cadre (FIXME : perturber un peu)
    v.Fade(90); v *= 2.f; v.Mod();
    v.ClampAndScale(0.21f, 0.2f);

    // Interstices entre les planches
    h.Bump();
    h.Abs();
    h.Scale(1.f, 0);
    h.Blur();

    v.MinimizeTo(h);
    v.Scale(-1.f, 0);

    rnd += v;

    buildAndQueueBumpMapFromHeightMap(crateBump, rnd);

    r += rnd;
    g += rnd;
    b += rnd;

    queueTexture(crate,
		 r, g, b,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    // Version Black Mesa
    r *= bm;
    g *= bm;
    b *= bm;
    queueTexture(crateBlackMesa,
		 r, g, b,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildCityAndForestSky()
  {
    Channel fade; fade.Fade(0);
    Channel rnd; randomThenGaussian(rnd);
    rnd.Scale(-0.006f, 0.006f);
    fade += rnd;

    fade.Scale(1.f, -0.95f);
    fade.Clamp(0, 1.f);

    // Horizon
    Channel horizon = fade;
    horizon.ClampAndScale(0.f, 0.01f);

    // Etoiles
    Channel st; st.Random();
    st.ClampAndScale(0.9995f, 1.f);
    st.Clamp(0.f, 0.15f);
    {
      Channel st2;
      rnd.Random();
      st2.Random();
      st2.MaximizeTo(rnd);
      st2.ClampAndScale(0.99f, 1.f);
      st2.Blur();
      st2.Pow(0.6f);
      st += st2;
    }
    st *= horizon;

    // Ville
    {
      Channel r = fade; r.Scale(0.88f, 0); r.Pow(1.9f);
      Channel g = fade; g.Scale(1.f, 0); g.Pow(1.8f); g.Scale(0, 0.66f);
      Channel b = fade; b.Pow(2.f); b.Scale(0.56f, 0.20f);
      r *= horizon;
      g *= horizon;
      b *= horizon;

      r += st;
      g += st;
      b += st;

      queueTexture(citySky,
		   r, g, b,
		   GL_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_CLAMP_TO_EDGE);
    }

    // Forêt
    {
      Channel r = fade; r.Pow(2.); r.Scale(0.78f, 0.44f);
      Channel g = fade; g.Pow(2.); g.Scale(0.76f, 0.72f);
      Channel b = fade; b.Pow(2.); b.Scale(0.82f, 1.f);
      r *= horizon;
      g *= horizon;
      b *= horizon;

      queueTexture(forestSky,
		   r, g, b,
		   GL_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_CLAMP_TO_EDGE);
    }

    // --------------------------------------------
    fade.Conic();
    fade.Clamp(0, 1.);

    randomThenGaussian(rnd);
    rnd.Scale(-0.006f, 0.006f);
    fade += rnd;

    // Ville
    {
      Channel r; r.Flat(0);
      Channel b = fade; b.Scale(0.20f, 0.1f);
      r += st;
      b += st;
      queueTexture(citySkyRoof,
		   r, r, b,
		   GL_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_CLAMP_TO_EDGE);
    }

    // Forêt
    {
      Channel r = fade; r.Scale(0.44f, 0.20f);
      Channel g = fade; g.Scale(0.72f, 0.68f);
      Channel b; b.Flat(1.f);
      queueTexture(forestSkyRoof,
		   r, g, b,
		   GL_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_CLAMP_TO_EDGE);
    }
  }

  // Fonctions auxilliaires pour les rosaces
  float dcos_(float x, float d) { return d * cos(DEG2RAD * x); }
  float dsin_(float x, float d) { return d * sin(DEG2RAD * x); }

  void curve(Texture::Channel &t, float a1, float a2, float d1, float d2, float r)
  {
    const float CURV_I = -0.007f; // intensité de la courbure
    const int INTERP   = 7; // nombre de segments dans une courbe

    float rx = dcos_(r, 1);
    float ry = dsin_(r, 1);

    float px = dcos(a1, d1);
    float py = dsin(a1, d1);
    float nx, ny;
    float na, nd;
    float d;

    for (int i = 1; i <= INTERP; ++i)
    {
      na = a1 + (i * (a2 - a1)) / INTERP;
      nd = d1 + (i * (d2 - d1)) / INTERP;
      d = dsin_((180.f * i) / INTERP, CURV_I);

      nx = dcos(na, nd) + rx * d;
      ny = dsin(na, nd) + ry * d;
      t.Line(px, py, nx, ny);
      px = nx;
      py = ny;
    }
  }

  const float TOTAL  = 18;
  const float STEP   = 4;
  const float SPAN   = 360.f * STEP / TOTAL;

  void petal(Texture::Channel& t, float angle)
  {
    const float INNER  = 0.2f;
    const float OUTER  = 0.9f;
    const float CURV_D = 90; // angle de la courbure (90 => centré)

    curve(t, angle - SPAN / 2, angle, INNER, OUTER, angle - CURV_D);
    curve(t, angle + SPAN / 2, angle, INNER, OUTER, angle + CURV_D);
  }

  static void buildRosace()
  {
    Texture::Channel t;
    float base = -90;

    int ITS = 10;
    for (int i = 0; i < ITS; ++i)
    {
      petal(t, base);
      petal(t, base + 20);
      base = base + SPAN;
    }

    t.GaussianBlur();

    t.FloodFill(1, 1, 1.f);
    t.GaussianBlur();
    t.Pow(0.2f);
    t.Scale(0.f, 1.f);
    queueTexture(rosace, t, t, t, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy, GL_REPEAT);
  }


  static void buildRoad()
  {
    Channel rnd;
    randomThenGaussian(rnd);
    rnd.Scale(-1.f, 1.f);
    rnd.Abs();

    Channel rnd2;
    randomThenBlur(rnd2);
    rnd2.Clamp(0.3f, 0.6f);
    rnd2.Scale(-0.25f, 0.25f);

    rnd += rnd2;
    rnd.Blur();

    Channel asphalte = rnd;

    // =============================
    // Blandes

    Channel marks1;
    marks1.Fade(90); marks1 *= 2.f; marks1.Mod();
    marks1.ClampAndScale(0.51f, 0.5f);

    Channel marks2 = marks1;
    marks2.Rotate90();

    Channel marks;
    marks.Fade(0); marks *= 8.f; marks.Mod();
    marks.ClampAndScale(0.63f, 0.7f);

    {
      Channel v = marks;
      v.Rotate90();
      marks.MaximizeTo(v);
    }
    marks.Scale(-0.5f, 1.3f);

    Channel m;
    randomThenGaussian(m);
    m.Clamp(0.25f, 0.75f);
    m.Scale(-0.5f, 0.5f);

    m += marks;
    m.Clamp(0, 1.f);

    // =============================

    // Asphaltes
    Channel ar; ar = asphalte; ar.Scale(0.05f, 0.65f);
    Channel ag; ag = asphalte; ag.Scale(0.05f, 0.72f);
    Channel ab; ab = asphalte; ab.Scale(0, 0.8f);

    // Jaunes :
    // 0.96 0.8 0.15
    // Plus ternes : #b56f14, #b56f14
    randomThenGaussian(rnd);
    rnd.GaussianBlur();
    rnd.Scale(-0.12f, 0.12f);

    Channel jr; jr.Flat(0.96f); jr += rnd;
    Channel jg; jg.Flat(0.8f);  jg += rnd;
    Channel jb; jb.Flat(0.15f); jb += rnd;

    Channel r;
    Channel g;
    Channel b;

    asphalte.Scale(0, 0.4f);
    Channel jrnd;
    randomThenGaussian(jrnd);
    jrnd.GaussianBlur();
    jrnd.Scale(0.4f, 0.6f);

    {
      marks = m;
      marks.MaximizeTo(marks1);

      // Speculaire
      Channel s = marks;
      s.Scale(0.7f, 0.6f);
      queueTexture(roadVSpecular,
		   s,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Couleur
      r = ar; r.Mask(marks, jr);
      g = ag; g.Mask(marks, jg);
      b = ab; b.Mask(marks, jb);

      queueTexture(roadV,
		   r, g, b,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Bump
      Channel bump;
      bump = asphalte;
      bump.Mask(marks, jrnd);
      buildAndQueueBumpMapFromHeightMap(roadVBump, bump);
    }

    {
      marks = m;
      marks.MaximizeTo(marks2);

      // Speculaire
      Channel s = marks;
      s.Scale(0.7f, 0.6f);
      queueTexture(roadHSpecular,
		   s,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Couleur
      r = ar; r.Mask(marks, jr);
      g = ag; g.Mask(marks, jg);
      b = ab; b.Mask(marks, jb);

      queueTexture(roadH,
		   r, g, b,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Bump
      Channel bump;
      bump = asphalte;
      bump.Mask(marks, jrnd);
      buildAndQueueBumpMapFromHeightMap(roadHBump, bump);
    }

    {
      marks.MaximizeTo(marks1);

      // Speculaire
      Channel s = marks;
      s.Scale(0.7f, 0.6f);
      queueTexture(roadOSpecular,
		   s,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Couleur
      ar.Mask(marks, jr);
      ag.Mask(marks, jg);
      ab.Mask(marks, jb);

      queueTexture(roadO,
		   ar, ag, ab,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      // Bump
      Channel bump;
      bump = asphalte;
      bump.Mask(marks, jrnd);
      buildAndQueueBumpMapFromHeightMap(roadOBump, bump);
    }
  }

  static void buildPavement()
  {
    Channel rnd;
    randomThenBlur(rnd);
    rnd.Scale(-1.f, 1.f);
    rnd.Abs();

    Channel tmp;
    tmp.Random();
    tmp.Clamp(0.4f, 0.6f);
    tmp.Scale(-0.25f, 0.25f);

    rnd += tmp;
    rnd.Blur();
    rnd.Scale(0.5f, 1.f);

    tmp.Fade(0); tmp *= 20.f; tmp.Mod();
    tmp.ClampAndScale(0.05f, 0.25f);
    tmp.Pow(0.5f);
    tmp.Scale(0.5f, 1.f);
    rnd *= tmp;

    tmp.Rotate90();
    rnd *= tmp;

    {
      Channel h = rnd;
      h.Scale(0, 0.5f);
      buildAndQueueBumpMapFromHeightMap(pavementBump, h);
    }

    tmp.Square();
    tmp.Clamp(0.2f, 0.205f);
    tmp.Scale(0.7f, 0.3f);

    // Speculaire
    queueTexture(pavementSpecular,
		 tmp,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    tmp.Scale(0.7f, 1.f);
    rnd *= tmp;

    // Albedo
    queueTexture(pavement,
		 rnd, rnd, rnd,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  //
  // Herbe
  //
  static void buildGrass()
  {
    Channel t;
    t.Flat(0.95f);

    for (int i = 0; i < 4; ++i)
    {
      t.AddCuts(2.f, 0.01f);
      t.AddCuts(2.f, 0.03f);
      t.AddCuts(4.f, 0.05f);
      t.Scale(0, 0.7f);
      t.GaussianBlur();
    }
    t.Scale(0, 1.f);

    {
      Channel joint;
      joint.AddCuts(5.f, 0.04f);
      joint.AddCuts(20.f, 0.01f);
      joint.GaussianBlur();

      Channel mask;
      mask.Square();
      mask.ClampAndScale(0.15f, 0);

      t.Mask(mask, joint);
    }

    queueTexture(grass,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    t.Pow(2.f);
    t.Scale(0, 0.8f);


    queueTexture(grassSpecular,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    buildAndQueueBumpMapFromHeightMap(grassBump, t);
  }

  //
  // Herbe
  //
  static void buildMoss()
  {
    Channel t;
    t.Flat(0.9f);

    for (int i = 0; i < 3; ++i)
    {
      t.AddCuts(9.f + 2.f * i, 0.012f);
      t.Blur();
      t.Scale(0, 0.7f);
    }

    t.Scale(0, 1.f);
    t.Pow(0.6f);

    {
      Channel joint;
      joint.AddCuts(30.f, 0.008f);
      joint.Blur();

      Channel mask;
      mask.Square();
      mask.ClampAndScale(0.1f, 0);

      t.Mask(mask, joint);
    }
    t.Pow(0.8f);

    queueTexture(moss,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    t.Pow(2.f);
    t *= 0.5f;

    queueTexture(mossSpecular,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    buildAndQueueBumpMapFromHeightMap(mossBump, t);
  }

  static void buildTaxiCar()
  {
    Channel damier;

    damier.Fade(0); damier *= 20.f; damier.Mod();
    damier.Clamp(0.45f, 0.55f);
    damier.Scale(-1.f, 1.f);
    damier.Translate(0, 0.025f);

    {
      Channel tmp = damier;
      tmp.Rotate90();
      damier *= tmp;
    }

    Channel bande;
    bande.Fade(0);
    bande.ClampAndScale(0.75f, 0.755f);
    bande.Translate(0, 0.5f);

    Channel r; r.Flat(1.f);
    Channel g; g.Flat(0.85f);
    Channel b; b.Flat(0);

    r.Mask(bande, damier);
    g.Mask(bande, damier);
    b.Mask(bande, damier);

    queueTexture(taxiCar,
		 r, g, b,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildCarBump()
  {
    Channel h;
    h.Square();
    h.ClampAndScale(0, 0.15f);
    h.Pow(0.6f);

    buildAndQueueBumpMapFromHeightMap(carBump, h);
  }

  static void buildTrafficLights()
  {
    Channel t;
    t.Square();

    Channel tmp;
    tmp.Square(); tmp *= 64.f; tmp.Mod();
    tmp.Scale(-0.2f, 0.4f);
    tmp.GaussianBlur();

    t += tmp;

    t.ClampAndScale(0.3f, 0.6f);
    t.Pow(0.5f);

    tmp = t;
    tmp.ClampAndScale(0.21f, 0.2f); // glow map

    Channel empty; empty.Flat(0.02f);
    Channel off; off = t; off *= 0.25f;
    queueTexture(redLightOn,
		 t, empty, empty, tmp,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
    queueTexture(redLightOff,
		 off, empty, empty,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
    queueTexture(greenLightOn,
		 empty, t, empty, tmp,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
    queueTexture(greenLightOff,
		 empty, off, empty,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildPharmacy()
  {
    Channel t;
    Channel t2;

    t2.Square();
    t2.Scale(-1.f, 20.f);
    t2.Clamp(0.f, 9.f);
    t2.Sin();
    t2.Scale(1.f, 0.f);

    t.Square();
    t.ClampAndScale(0.6f, 0.f);
    t *= t2;

    t.Translate(0.5f, 0.5f);
    Channel black; black.Flat(0.);

    queueTexture(pharmacy,
		 black, t, black,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildStatue()
  {
	Texture::Channel t;
	t.Cells2(24);
	t.AverageScale(4, 2.f);
	t.ClampAndScale(0.7f, 0.95f);
	t.Scale(0.0f, 0.65f);

	Texture::Channel t2;
	t2.Random();
	t2.Scale(0.0f, 0.7f);

	Texture::Channel t3;
	t3.Cells2(48);
	t3.AverageScale(5, 2.f);
	t3.Scale(0.0f, 1.66f);
	t2 += t3;
	t2.GaussianBlur();

	t2.Scale(0.0f, 0.8f);
	t += t2;

	Texture::Channel r = t;
	Texture::Channel g = t;
	Texture::Channel b = t;
	r.Scale(0.9f, 0.8f);
	g.Scale(0.75f, 0.5f);
	b.Scale(0.25f, 0.2f);
	queueTexture(statue, r, g, b, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy, GL_REPEAT);
	t.ClampAndScale(1.f, 0.f);
	queueTexture(statueSpecular, t, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy, GL_REPEAT);
  }

  static void buildWindows1()
  {
    // Mur / fenêtre
    // Cadre / carreau
    // Brique / joint
    // ================================================

    Channel groutOrBrick;
    groutOrBrick.Fade(90); groutOrBrick *= 80.f; groutOrBrick.Mod();

    Channel tmp = groutOrBrick;
    groutOrBrick.Rotate90();
    groutOrBrick.MaximizeTo(tmp);

    groutOrBrick.ClampAndScale(0.1f, 0.2f);

    // ---------------------------

    // 2 x nombre de fenêtres
    Channel wallOrWindow;
    wallOrWindow.Fade(90); wallOrWindow *= 8.f; wallOrWindow.Mod();

    tmp = wallOrWindow;
    wallOrWindow.Rotate90();
    wallOrWindow.MaximizeTo(tmp);

    // ---------------------------

    Channel frameOrWindow;
    frameOrWindow = wallOrWindow;
    frameOrWindow.ClampAndScale(0.5f, 0.51f);

    wallOrWindow.ClampAndScale(0.39f, 0.4f);

    // ---------------------------

    Channel rnd;
    rnd.Random();
    rnd.ClampAndScale(0, 0.2f);
    rnd.Blur();


    // ================================================
    // Couleurs
    {
      // Mur
      Channel r;
      Channel g;
      Channel b;
      {
	// Briques
	Channel br;
	Channel bg;
	Channel bb;
	{
	  Channel t;
	  randomThenGaussian(t);
	  t.VerticalMotionBlur(0.014f);
	  t.HorizontalMotionBlur(0.023f);
	  t.Scale(0, 1.f);
	  t.ClampAndScale(0.05f, 0.4f);

	  Channel t2;
	  t2.Random();
	  t2.Scale(0.f, 0.2f);
	  t += t2;
	  t.Scale(0, 1.f);

	  br = t; br.Scale(0.64f, 0.77f);
	  bg = t; bg.Scale(0.24f, 0.39f);
	  bb = t; bb.Scale(0.17f, 0.22f);

	  Channel rnd;
	  rnd.Random();
	  rnd.Clamp(0, 0.02f);
	  rnd.Blur();
	  rnd.Scale(0.7f, 1.f);
	  br *= rnd;
	  bg *= rnd;
	  bb *= rnd;
	}

	// Joint
	{
	  Channel t;
	  t.Random();

	  t.Blur();
	  t.Pow(0.2f);

	  r = t; r.Scale(0.71f, 0.92f);
	  g = t; g.Scale(0.70f, 0.91f);
	  b = t; b.Scale(0.63f, 0.91f);
	}
	r.Mask(groutOrBrick, br);
	g.Mask(groutOrBrick, bg);
	b.Mask(groutOrBrick, bb);
      }

      // Masque ambient occlusion des joints
      {
	Channel t = groutOrBrick;
	t.Blur();
	t.Scale(1.f, 0);
	t *= t;
	t.Scale(1.f, 0.7f);
	r *= t;
	g *= t;
	b *= t;
      }


      // Version sans fenetres
      {
	queueTexture(wall1,
		     r, g, b,
		     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		     maxAnisotropy,
		     GL_REPEAT);

	// Bump map
	// ========
	{
	  Channel bump = groutOrBrick;
	  bump *= rnd;
	  bump.Scale(0.9f, 1.f);
	  buildAndQueueBumpMapFromHeightMap(wall1Bump, bump);
	}

	// Specular map
	// ============
	{
	  Channel spec = groutOrBrick;
	  spec.Scale(0.3f, 0.4f);
	  queueTexture(wall1Specular,
		       spec,
		       GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		       maxAnisotropy,
		       GL_REPEAT);
	}
      }

      // Toit
      {
	// Carreau
	Channel t; t.Fade(0); t *= 16.f; t.Mod();

	tmp = t;
	tmp.Rotate90();
	t *= tmp;

	t.ClampAndScale(0, 0.8f);
	t.Pow(0.2f);
	t.VerticalMotionBlur(0.01f);
	t.HorizontalMotionBlur(0.01f);

	Channel rr = t; rr.Scale(0.41f, 0.6f);
	Channel rg = t; rg.Scale(0.42f, 0.62f);
	Channel rb = t; rb.Scale(0.38f, 0.56f);

	// ---------------------------

	Channel wallOrRoof;
	wallOrRoof.Square();
	wallOrRoof.ClampAndScale(0.1f, 0.105f);

	// Masque ambient occlusion
	t = wallOrRoof;
	t.VerticalMotionBlur(0.09f);
	t.HorizontalMotionBlur(0.09f);
	t.VerticalMotionBlur(0.02f);
	t.HorizontalMotionBlur(0.02f);
	t.ClampAndScale(1.f, 0.4f);

	t *= t;
	t.Scale(1.f, 0.4f);
	rr *= t;
	rg *= t;
	rb *= t;

	wallOrRoof.Scale(1.f, 0);
	rr.Mask(wallOrRoof, r);
	rg.Mask(wallOrRoof, g);
	rb.Mask(wallOrRoof, b);

	queueTexture(roof1,
		     rr, rg, rb,
		     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		     maxAnisotropy,
		     GL_REPEAT);
      }

      // Fenêtres
      {
	// Cadre
	Channel r2; r2.Flat(0.71f);
	Channel g2; g2.Flat(0.63f);
	Channel b2; b2.Flat(0.55f);

	// Carreau
	Channel wr;
	Channel wg;
	Channel wb;
	{
	  Channel t;
	  t.Perlin(16, 1, 1, 1);					// Perlin 16
	  t.Scale(0, 1.f);

	  Channel fade;
	  fade.Fade(0);
	  fade.Cut(3);
	  fade.Clamp(0.25f, 0.75f);
	  fade.Scale(1.f, 0);

	  t *= fade;

	  wr = t; wr.Scale(0.28f, 0.42f);
	  wg = t; wg.Scale(0.33f, 0.48f);
	  wb = t; wb.Scale(0.45f, 0.61f);
	}

	r2.Mask(frameOrWindow, wr);
	g2.Mask(frameOrWindow, wg);
	b2.Mask(frameOrWindow, wb);

	// Masque ambient occlusion
	{
	  Channel t = wallOrWindow;
	  motionMotion(t, 0.016f, 0.01f);
	  t.ClampAndScale(0.95f, 0.4f);
	  t *= t;
	  t.Scale(1.f, 0.4f);
	  r2 *= t;
	  g2 *= t;
	  b2 *= t;
	}

	r.Mask(wallOrWindow, r2);
	g.Mask(wallOrWindow, g2);
	b.Mask(wallOrWindow, b2);
      }

      queueTexture(windows1,
		   r, g, b,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

    // Bump map
    // ========
    {
      Channel bump = groutOrBrick;
      bump *= rnd;

      bump.Scale(0.9f, 1.f);
      tmp = wallOrWindow;  tmp.Scale(1.f, 0.5f); bump.MaximizeTo(tmp);
      tmp = frameOrWindow; tmp.Scale(1.f, 0);    bump.MaximizeTo(tmp);

      buildAndQueueBumpMapFromHeightMap(windows1Bump, bump);
    }

    // Specular map
    // ============
    {
      Channel spec = groutOrBrick;
      spec.Scale(0.3f, 0.4f);
      tmp = wallOrWindow;  tmp.Scale(0, 0.5f); spec.MinimizeTo(tmp);
      tmp = frameOrWindow;                     spec.MinimizeTo(tmp);

      queueTexture(windows1Specular,
		   spec,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);
    }

  }

  static void buildWalls()
  {
    buffers = new Texture::Channel[numberOfUrbanTextures];
    buildUrban();

    buildAndQueueBumpMapFromHeightMap(windows2Bump, buffers[wallHeight]);
    buildAndQueueBumpMapFromHeightMap(windows4Bump, buffers[wall3Height]);
//     buildAndQueueBumpMapFromHeightMap(roof2Bump, buffers[roof2Height]);
//     buildAndQueueBumpMapFromHeightMap(roof3Bump, buffers[roof3Height]);
    buildAndQueueBumpMapFromHeightMap(pacmanWallBump, buffers[pacWallHeight]);

    queueTexture(windows2,
		 buffers[wall1r], buffers[wall1g], buffers[wall1b],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(windows3,
		 buffers[wall2r], buffers[wall2g], buffers[wall2b],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(windows4,
		 buffers[wall3r], buffers[wall3g], buffers[wall3b],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(pacmanWall,
		 buffers[pacWallr], buffers[pacWallg], buffers[pacWallb],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(windows2Specular,
		 buffers[wallSpecular],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(roof2,
		 buffers[roof2r], buffers[roof2g], buffers[roof2b],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(roof4,
		 buffers[roof3r], buffers[roof3g], buffers[roof3b],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

//     queueTexture(roof2Specular,
// 		 buffers[roof2Specular],
// 		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
// 		 maxAnisotropy,
// 		 GL_REPEAT);

//     queueTexture(roof3Specular,
// 		 buffers[roof2Specular],
// 		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
// 		 maxAnisotropy,
// 		 GL_REPEAT);

    queueTexture(windows4Specular,
		 buffers[wall3Specular],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    queueTexture(pacmanWallSpecular,
		 buffers[pacWallSpecular],
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);

    delete[] buffers;

    // Toit en taule ondulée
    Channel h; h.Fade(0); h *= 32.f; h.Mod();
    h.Pow(0.5f);
    h.Scale(0, 0.8f);
    {
      Channel v; v.Fade(90.f); v *= 2.f; v.Mod();
      Channel v2 = v;
      v2.Clamp(0.95f, 1.f);
      v2.Scale(0, 1.f);
      h.MinimizeTo(v2);
      Channel r = h; r.Scale(0.61f, 0.67f);
      Channel g = h; g.Scale(0.59f, 0.79f);
      Channel b = h; b.Scale(0.54f, 0.79f);

      queueTexture(roof3,
		   r, g, b,
		   GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		   maxAnisotropy,
		   GL_REPEAT);

      v.Scale(0, 0.3f);
      h += v;
      h.Scale(0, 1.f);
      buildAndQueueBumpMapFromHeightMap(roof3Bump, h);
    }
  }

  static void buildContainer()
   {
    Channel t;
    t.Fade(90); t *= 6.f; t.Mod();
    t.Pow(0.2f);
    t.ClampAndScale(0.4f, 0.9f);

    Channel t2;
    t2.Fade(90);
    t2.Scale(1.f, 7.f);
    t2.Mod();
    t2.Pow(3.f);
    t2.ClampAndScale(0.4f, 0.8f);

    t.MinimizeTo(t2);
    t.Scale(0.6f, 1.f);

    // bruit basse frequence
    Channel noise;
    randomThenMotion(noise, 0.019f, 0.019f);

    noise.Scale(-0.05f, 0.05f);
    t += noise;

    // impacts
    Channel concrete;
    concrete.Random();
    concrete.Scale(0.f, 0.93f);
    concrete.AddCuts(0.06f, 0.019f);
    concrete.AddCuts(0.06f, 0.058f);
    concrete.AddCuts(0.06f, 0.078f);
    concrete.GaussianBlur();
    concrete.ClampAndScale(0.3f, 1.f);

    Channel grey;
    grey.Flat(0.f);

    Channel r;
    r = t;
    r *= 0.2f;
    r.Mask(concrete, grey);

    Channel g;
    g = t;
    g *= 0.3f;
    g.Mask(concrete, grey);

    Channel b;
    b = t;
    b *= 0.4f;
    b.Mask(concrete, grey);

    queueTexture(container,
		      r, g, b,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_REPEAT);

    r *= 3.f;
    g *= 2.f;

    queueTexture(container2,
		      r, g, b,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_REPEAT);
  }


  static void buildRampBox()
  {
    Channel t;
    t = *perlin7;

    t.Pow(1.8f);
    t.AverageScale(2, 2.f);
    Channel co;
    co.Conic();
    co.Clamp(0.f, 1.f);
    t *= co;
    t.Scale(0, 1.);

    Channel r = t; r *= 0.36f;
    Channel g = t; g *= 0.05f;
    t *= 0.01f;

    queueTexture(rampBox,
		 r, g, t,
		 GL_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildRampBox2()
  {
    Channel t;
    t = *perlin7;
    t.VerticalFlip();
    t.Pow(0.6f);
    t.AverageScale(3, 2.f);
    Channel co;
    co.Conic();
    co.Clamp(0.f, 1.f);
    t *= co;

    Channel r = t; r *= 0.34f;
    Channel g = t; g *= 0.01f;
    t *= 0.05f;

    queueTexture(rampBox2,
		 r, g, t,
		 GL_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildSpaceBox()
  {
    Channel rnd;
    randomThenGaussian(rnd);

    Channel per;
    per = *perlin7;
    per.Pow(0.2f);
    per.Clamp(0.65f, 0.8f);
    per *= rnd;

    Channel per2;
    per2 = per;
    per.ClampAndScale(0.39f, 0.7f);
    per.GaussianBlur();

    per2.ClampAndScale(0.42f, 0.7f); per2 *= 1.4f;
    per2.GaussianBlur();

    Channel t;
    t = rnd; // bruit.Copy();
    t.GaussianBlur();
//     t.GaussianBlur(); // Provoque une texture uniforme en 256
    t.ClampAndScale(0.36f, 0.2f);
    t.GaussianBlur();
    t.Pow(0.8f);

    rnd.Random();
    rnd.ClampAndScale(0.96f, 1.f); rnd *= 0.15f;
    t += rnd;

    rnd.Random();
    rnd.ClampAndScale(0.99f, 1.f); rnd *= 0.3f;
    t += rnd;

    Channel r = t; r.Scale(0, 0.06f);
    Channel b = t;
    b += per;
    b.Blur();
    b.Scale(0, 0.6f);

    t += per2;
    t.Scale(0, 0.16f);

    queueTexture(spaceBox,
		 r, t, b,
		 GL_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildTransitionBurn()
  {
    Channel t;
    t.Conic();
    Channel noise;
    noise.Random();
    noise.Scale(-0.1f, 0.1f);
    t += noise;
    t.GaussianBlur();
    t.HorizontalWave(4.f, 10);
    t.Scale(0.f, 1.f);

    queueTexture(transitionBurn,
		      t, t, t,
		      GL_LINEAR, GL_LINEAR,
		      maxAnisotropy,
		      GL_REPEAT);
  }

  void forestHeightMap(Channel & t,
		       Channel & r,
		       Channel & g,
		       Channel & b,
		       int size)
  {
    // Heightmap
    // ============================
    t.Perlin(2, 1, 1, 1);					// Perlin 2
    Channel per(size, size);
    per.Perlin(8, 1, 1, 1);					// Perlin 8

    t += per;
    t.Scale(0, 1.f);

    {
      Channel creux(size, size);
      creux.Fade(90.f);
      creux.Scale(-1.f, 1.f);
      creux.Abs();

      {
	Channel bord(size, size);
	bord.Fade(0);
	bord.ClampAndScale(0.8f, 1.f);
	creux.MinimizeTo(bord);
      }
      t *= creux;
    }
    t.Scale(0, 1.f);

    // Herbe
    // =============================
    {
      Channel per2(size, size);
      per.Perlin(1, 1, 1, 1.f);					// Perlin 1
      per2.Perlin(7, 1, 1, 1.f);				// Perlin 7
      per2.Scale(0, 0.25f);
      per += per2;
      per.Scale(0, 1.f);
    }

    Channel hr = per; hr.Scale(0.25f, 0.85f);
    Channel hg = per; hg.Scale(0.51f, 0.98f);
    Channel hb = per; hb.Scale(0.00f, 0.00f);

    // Terre/humus
    // =============================
    {
      Channel per3(size, size);
      per3.Perlin(17, 1, 1, 1.f);				// Perlin 17 - FIXME : utiliser un bruit avec flou
      per3.Scale(0, 0.15f);
      per += per3;
      per.Scale(0, 1.f);
    }

    r = per; r.Scale(0.45f, 0.89f);
    g = per; g.Scale(0.27f, 0.61f);
    b = per; b.Scale(0.10f, 0.16f);

    // Mix final
    // ==============================
    {
      Channel m = t;
      m.ClampAndScale(0, 0.25f);
      m.Pow(0.8f);

      r.Mask(m, hr);
      g.Mask(m, hg);
      b.Mask(m, hb);
    }

    // Faux éclairage
    // ==============================
    {
      Channel l = t;
      l.Bump();
      l.Scale(0, 1);
      l.Pow(0.9f);
      l.Clamp(0, 0.5f);
      l.Scale(0.5f, 1.f);

      r *= l;
      g *= l;
      b *= l;
    }
  }
}
