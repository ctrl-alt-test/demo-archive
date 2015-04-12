//
// Tous les VBOs utilisés
//

#include "textures.hh"

#include "sys/msys.h"

#include "array.hh"
#include "textureid.hh"
#include "trails.hh"
#include "texgen/texture.hh"

namespace Texture
{
  Unit * list = NULL;


#ifdef DEBUG

  static void buildTestTexture();
  static void buildBumpTestTexture();
  static void buildSpecularTestTexture();

#endif // DEBUG

  static void buildPlainTextures();
//   static void buildDefaultBump();
  static void buildAmbientMap();

  static void buildLightPatterns();

//   static void buildDummyRelief();
  static void buildBuildingLightPatterns();

  /*
  static void buildTire();
  static void buildEmboutissage1Bump();
  static void buildEmboutissage2Bump();
  static void buildEmboutissage3Bump();
  */

  static void buildGroundNoise();

  static void setupRenderTextures();
  static void setupRenderDepthTextures();

  static unsigned int getCurrentBuilderIndex();

  typedef void(*textureBuilder)();

  const textureBuilder threadUnsafeBuilders[] =
    {
      buildPlainTextures,
    };

  const textureBuilder threadSafeBuilders[] =
    {
#if DEBUG

      buildTestTexture,
      buildBumpTestTexture,
      buildSpecularTestTexture,

#endif // DEBUG

#if (!DEBUG_TRAILS)

//       buildDefaultBump,
      buildAmbientMap,
      buildLightPatterns,

      buildBuildingLightPatterns,

      buildGroundNoise,

#endif // DEBUG_TRAILS

      setupRenderTextures,
      setupRenderDepthTextures
    };


  Channel * perlin7 = NULL;

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
    START_TIME_EVAL;

    assert(list != NULL);

    textureQueue = new Array<textureDescription>(numberOfTextures);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

    DBG("Anisotropie max : %.2f", maxAnisotropy);
    maxAnisotropy = min(maxAnisotropy, 8.f);
    DBG("Anisotropie choisie : %.2f", maxAnisotropy);

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
      OGL_ERROR_CHECK("Texture::loadTextures i = %u / %d", i, numberOfThreadUnsafeBuilders);
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

    END_TIME_EVAL("Textures generation");
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
    setupTextureFormatForRendering(postProcessRender, GL_RGBA);
    setupTextureFormatForRendering(postProcessPong, GL_RGBA);

    // FIXME : du RGB devrait suffire pour la velocity map.
    //  ==>  Modifier et tester.
    setupTextureFormatForRendering(postProcessVelocity, GL_RGBA);

    setupTextureFormatForRendering(postProcessDownscale2, GL_RGBA);
    setupTextureFormatForRendering(postProcessDownscale4, GL_RGBA);
    setupTextureFormatForRendering(postProcessDownscale8, GL_RGBA);
    setupTextureFormatForRendering(postProcessDownscale8Pong, GL_RGBA);

    setupTextureFormatForRendering(trails, GL_RGBA);
  }

  static void setupRenderDepthTextures()
  {
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
    Channel r(h);
    Channel g(h);

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
    Channel b(h.Width(), h.Height());
    b.Flat(2.);
    b -= r;
    b -= g;

    queueTexture(texId,
		 r, g, b, h,
 		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy,
		 repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  }

  //
  // Ambient occlusion
  //
  static void buildAmbientOcclusionMapFromHeightMap(const Channel & t, Channel & l)
  {
    Channel m0(t);
    Channel m1(t.Width(), t.Height());
    Channel sub(t.Width(), t.Height());

    l.Flat(1.f);

    int d = 2;
    for (int i = 0; i != 5; ++i)
    {
      // Filtre convolutif
      //    -1
      // -1  4  -1
      //    -1
      sub = m0; sub *= 4.f;
      m1 = m0; m1.Translatei( d,  0); sub -= m1;
      m1 = m0; m1.Translatei(-d,  0); sub -= m1;
      m1 = m0; m1.Translatei( 0,  d); sub -= m1;
      m1 = m0; m1.Translatei( 0, -d); sub -= m1;

      sub.Scale(1.f - 1.f / pow(2.f, i), 1.f);
      l *= sub;
      m0.GaussianBlur();
      d *= 2;
    }
    l.Scale(0., 1.f);

    // Valeurs trouvées expérimentalement
    l.Pow(0.3f);
    l.Scale(0.25f, 1.25f);
    l.Clamp(0, 1.f);
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

  //
  // Textures de test (ne seront pas compilées en RELEASE)
  //

  static void buildTestTexture()
  {
    Channel r;
    Channel g;
    Channel b;
    Channel a;
    r.Gradient(1.1f, 0.3f, 0.2f, 0.6f);
    g.Fade(0);
    b.Fade(90.f);

    Channel grid;
    grid.Fade(0); grid *= 32.f; grid.Mod();
    Channel tmp = grid;
    tmp.Rotate90();
    grid.MinimizeTo(tmp);
    grid.ClampAndScale(0.9f, 0.95f);
    r.MinimizeTo(grid);
    g.MinimizeTo(grid);
    b.MinimizeTo(grid);

    a.Fade(0); a *= 6.f; a.Mod();
    tmp = a;
    tmp.Rotate90();
    a.MaximizeTo(tmp);
    a.ClampAndScale(1.f, 0.7f);

    queueTexture(test,
		 r, g, b, a,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  static void buildBumpTestTexture()
  {
    Channel m;
    m.Conic(); m *= 2.f; m.Mod();
    m.ClampAndScale(0.9f, 0.8f);

    Channel c;
    c.Square();
    c.ClampAndScale(0.9f, 0.95f);
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

    m += c;
    m.Scale(0, 1.);

    buildAndQueueBumpMapFromHeightMap(testBump, m);


    Channel h;
    h.Fade(0); h *= 20.f; h.Mod();
    h.ClampAndScale(0.1f, 0.2f);
    {
      Channel v = h;
      v.Rotate90();
      h *= v;
    }
    h.Scale(0, 1.f);

    buildAndQueueBumpMapFromHeightMap(testBump2, h);
  }

  static void buildSpecularTestTexture()
  {
    Channel t;
    t.Fade(0);

    Channel grid;
    grid.Fade(0); grid *= 8.f; grid.Mod();
    Channel tmp = grid;
    tmp.Rotate90();
    grid.MaximizeTo(tmp);
    grid.ClampAndScale(0.4f, 0.45f);

    t *= grid;

    queueTexture(testSpecular,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
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
      {black,	          0,   0,   0, 255},
    };

  static void buildPlainTextures()
  {
    const unsigned int numberOfPlainTextures = sizeof(plainTextureDescs) / sizeof(plainTextureDescription);
    for (unsigned int i = 0; i < numberOfPlainTextures; ++i)
    {
      buildPlainColor(plainTextureDescs[i]);
    }
//     buildPlainColor(defaultSpecular, 0);
  }

  /*
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

    buildAndQueueBumpMapFromHeightMap(defaultBump, t);
  }
  */

  //
  // Ambient map style spherical harmonics
  // Du bleu d'un côté, du beige de l'autre, du blanc en haut et en bas
  //
  // Blanc de base : FFFBF4 - 255, 251, 244 - 1.00, 0.98, 0.96
  // Bleu :          A7B6D5 - 167, 182, 213 - 0.65, 0.71, 0.83
  // Orange :        FFEAC3 - 255, 234, 195 - 1.00, 0.92, 0.76
  static void buildAmbientMap()
  {
    const float intensity = 0.5f;

    const float white_r = 255.f/255.f * intensity;
    const float white_g = 251.f/255.f * intensity;
    const float white_b = 244.f/255.f * intensity;

    const float blue_r = 0.5f * (1.f + 167.f/255.f) * intensity;
    const float blue_g = 0.5f * (1.f + 182.f/255.f) * intensity;
    const float blue_b = 0.5f * (1.f + 213.f/255.f) * intensity;

    const float beige_r = 0.5f * (1.f + 255.f/255.f) * intensity;
    const float beige_g = 0.5f * (1.f + 234.f/255.f) * intensity;
    const float beige_b = 0.5f * (1.f + 195.f/255.f) * intensity;

    Channel h(256, 256); h.Cosinus(1, 0);
    Channel r(h); r.Scale(blue_r, beige_r);
    Channel g(h); g.Scale(blue_g, beige_g);
    Channel b(h); b.Scale(blue_b, beige_b);

    Channel v(256, 256); v.Cosinus(0, 1);
    Channel wr(256, 256); wr.Flat(white_r); r.Mask(v, wr);
    Channel wg(256, 256); wg.Flat(white_g); g.Mask(v, wg);
    Channel wb(256, 256); wb.Flat(white_b); b.Mask(v, wb);

    // Pas de mipmapping car les interpolations créent des artefacts
    // très laids
    queueTexture(ambientMap,
		 r, g, b,
		 GL_LINEAR, GL_LINEAR, 1.f, GL_REPEAT);
  }

  static void buildLightPatterns()
  {
    const unsigned int size = 2048;

    float widthext = 0.03f;

    Channel h(size, size); h.Fade(90); h.Scale(0, 2.f); h.Mod();
    Channel v(size, size); v = h; v.Rotate90();

    // ==========================================
    // Motif de base

    Channel t1(h); t1.ClampAndScale(1.f - widthext, 1.f);
    Channel t2(v); t2.ClampAndScale(0.5f - widthext, 0.5f);

    Channel t(t1);
    t.Translate(-0.2f, 0);

    t1.MaximizeTo(t2);

    t1.Translate(0.4f, -0.1f); t += t1;
    t1.Translate(0.25f, 0); t += t1;
    t1.Translate(0.25f, 0); t += t1;
    t1.Translate(0.25f, 0); t += t1;

    t2 = v; t2.ClampAndScale(0.75f - widthext, 0.75f);
    t1.MaximizeTo(t2);

    t1.Translate(-0.1f, 0.25f);
    t2 = t1;
    t2.Translate(0, 0.5f);
    t1 += t2;

    t += t1;
    t1.Translate(-0.5f, 0.25f);
    t += t1;

    // ==========================================
    // Heightmap
    Channel tile(size / 4, size / 4);
    {
      float x1, y1, x2, y2;
      int size = 18;
      for (unsigned int i = 0; i < 200; ++i)
      {
	x1 = msys_rand() % size * (1.f / size);
	y1 = msys_rand() % size * (1.f / size);
	if (msys_rand() % 2 == 0)
	{
	  x2 = x1 + 1.f/size;
	  y2 = y1;
	}
	else
	{
	  y2 = y1 + 1.f/size;
	  x2 = x1;
	}
	tile.Line(x1, y1, x2, y2);
      }

      Channel h2(tile);
      tile.VerticalMotionBlur(0.005f);
      tile.HorizontalMotionBlur(0.01f);
      tile += h2;
      tile.VerticalMotionBlur(0.01f);
      tile.HorizontalMotionBlur(0.01f);

      tile.Scale(0.f, 2.8f); tile.Mod(); tile.ClampAndScale(0.1f, 0.4f);
      tile.GaussianBlur();

      tile.AverageScale(4, 1.9f);
      tile.Scale(0, 1.f);
      tile.Pow(1.2f);
      tile.Scale(0.5f, 1.f);
    }

    Channel height(size, size);
    for (unsigned int j = 0; j < 4; ++j)
      for (unsigned int i = 0; i < 4; ++i)
      {
	height.subCopy(tile,
		       0, 0,
		       i * tile.Width(), j * tile.Height(),
		       tile.Width(), tile.Height());
      }

    // Rainures
    t1 = t; t1.Clamp(0.f, 0.2f); t1.Scale(1.f, 0.1f);
    t2 = t; t2.Clamp(0.5f, 0.51f); t2.Scale(0.1f, 0);

    t1 += t2;

    height.MaximizeTo(t1);

    //
    // FIXME : ajouter le bruit
    //

    buildAndQueueBumpMapFromHeightMap(bumpPatterns, height);

    // ==========================================
    // Rayons
    t1.Fade(90); t1 *= 8.f; t1.Mod();
    t1.ClampAndScale(0.66f, 0.661f);

    Channel r(t);
    r.ClampAndScale(0.75f, 0.755f); // Un quart de la rainure

    Channel g(r);
    r *= t1;

    t1.Scale(1.f, 0.f);
    g *= t1;

    t1.Flat(0); // Dummy

    Channel l(size, size);
    buildAmbientOcclusionMapFromHeightMap(height, l);

    queueTexture(lightPatterns,
		 r, g, t1, l,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy, GL_REPEAT);

    // ==========================================
    // Specular map
    r.MinimizeTo(g);
//     r.MinimizeTo(b);

    queueTexture(specularPatterns,
		 r,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  /*
  static void buildDummyRelief()
  {
    // ==========================================
    // Heightmap
    Channel t;
    t.Flat(0.f);

    float x1, y1, x2, y2;
    int size = 18;
    for (unsigned int i = 0; i < 200; ++i)
    {
      x1 = msys_rand() % size * (1.f / size);
      y1 = msys_rand() % size * (1.f / size);
      if (msys_rand() % 2 == 0)
      {
	x2 = x1 + 1.f/size;
	y2 = y1;
      }
      else
      {
	y2 = y1 + 1.f/size;
	x2 = x1;
      }
      t.Line(x1, y1, x2, y2);
    }

    Channel t2 = t;
    t.VerticalMotionBlur(0.005f);
    t.HorizontalMotionBlur(0.01f);
    t += t2;
    t.VerticalMotionBlur(0.01f);
    t.HorizontalMotionBlur(0.01f);

    t.Scale(0.f, 2.8f);
    t.Mod();
    t.ClampAndScale(0.1f, 0.4f);

    t.GaussianBlur();

    t.AverageScale(4, 1.9f);
    t.Scale(0, 1.f);
    t.Pow(1.2f);
    t.Scale(0.5f, 1.f);

    t2.Conic(); t2 *= 2.f; t2.Mod();
    t2.ClampAndScale(0.9f, 0.8f);
    t.MaximizeTo(t2);

    buildAndQueueBumpMapFromHeightMap(dummyBumpRelief, t);

    // ==========================================
    // Light map
    Channel l;
    buildAmbientOcclusionMapFromHeightMap(t, l);

    t.Flat(0);

    queueTexture(dummyRelief,
		 t, t, t, l,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy, GL_REPEAT);
  }
  */

  static void buildBuildingLightPatterns()
  {
    float height = 0.05f;

    const unsigned int size = 1024;

    Channel h(size, size); h.Fade(90); h.Scale(0, 2.f); h.Mod();
    Channel v = h; v.Rotate90();

    // ==========================================
    Channel t(h.Width(), h.Height());
    Channel t2(t.Width(), t.Height());

    Channel r(t.Width(), t.Height());
    Channel g(t.Width(), t.Height());
    Channel b(t.Width(), t.Height());

    for (unsigned int i = 0; i < 8; ++i)
    {
      const float x = msys_sfrand();
      const float y = msys_sfrand();
      float width = 0.05f + 0.05f * msys_frand();
      float length = width * (1.f + 2.f * msys_frand());
      if (msys_rand() % 2 == 0)
      {
	float swap = length;
	length = width;
	width = swap;
      }
      t = h; t.ClampAndScale(1.f - width, 1.f - width + height);
      t2 = v; t2.ClampAndScale(1.f - length, 1.f - length + height);

      t.MaximizeTo(t2);
      t.Translate(x, y);

      const int choose = msys_rand() % 3;
      Channel & target = (0 == choose ? r : (1 == choose ? g : b));
      target.MinimizeTo(t);
    }

    // ==========================================
    // Heightmap
    t.Flat(0);

    t2.Random();
    t2.GaussianBlur();
    t2.HorizontalMotionBlur(0.05f);
    t2.VerticalMotionBlur(0.05f);
    t2.Scale(0, 1.f);
    t += t2;

    t2.Random();
    t2.HorizontalMotionBlur(0.001f);
    t2.VerticalMotionBlur(0.001f);
    t2.Scale(0, 0.05f);
    t += t2;

    t2.Random();
    t2.GaussianBlur();
    t2.Scale(0.f, 1.f);
    t2.ClampAndScale(0.4f, 1.f);
    t2.Pow(1.2f);
    t *= t2;

    t.Scale(0.8f, 1.f);

    t2.Flat(0);
    t2.MinimizeTo(r);
    t2.MinimizeTo(g);
    t2.MinimizeTo(b);
    t2.ClampAndScale(0.5f, 0);
    t2.Pow(2.f);

    t.MaximizeTo(t2);

    buildAndQueueBumpMapFromHeightMap(buildingBumpPatterns, t);

    // ==========================================
    // Light map
    Channel l(t.Width(), t.Height());
    buildAmbientOcclusionMapFromHeightMap(t, l);

    r.ClampAndScale(0.4f, 0.5f);
    g.ClampAndScale(0.4f, 0.5f);
    b.ClampAndScale(0.4f, 0.5f);

    queueTexture(buildingLightPatterns,
		 r, g, b, l,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy, GL_REPEAT);

    // ==========================================
    // Specular map
    t.Flat(0);
    t.MinimizeTo(r);
    t.MinimizeTo(g);
    t.MinimizeTo(b);

    queueTexture(buildingSpecularPatterns,
		 t,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy,
		 GL_REPEAT);
  }

  /*
  static void buildTire()
  {
    float angle = 0.f;
    float lines = 16.f;
    float fringes = 256.f;
    float zigzag = 64.f;
    float valley = 0.2f;
    float hill = 0.4f;

    Channel diag1;
    diag1.Fade(90.f);

    Channel lr;
    lr.Fade(angle); lr *= zigzag; lr.Mod();
    lr.Scale(0, 0.02f);

    Channel pattern = diag1;
    pattern += lr;

    pattern *= 2.f * lines; pattern.Mod();
    pattern.ClampAndScale(valley, hill);

    Channel fringe;
    fringe.Fade(0); fringe *= fringes; fringe.Mod();
    fringe.ClampAndScale(0.4f, 0.8f);

    Channel fringeOrBorder;
    fringeOrBorder.Fade(90); fringeOrBorder.Scale(0, 4.f); fringeOrBorder.Mod();
    fringeOrBorder.ClampAndScale(0.94f, 0.95f);

    fringe *= fringeOrBorder;

    Channel border;
    border.Fade(90); border *= 32.f; border.Mod();
    border.ClampAndScale(0.9f, 0.95f);
    fringe += border;

    fringe.Scale(0, 0.1f);

    Channel shapeOrBorder;
    shapeOrBorder.Fade(90.f); shapeOrBorder *= 2.f; shapeOrBorder.Mod();
    shapeOrBorder.ClampAndScale(0.75f, 0.74f);

    Channel shape = pattern;
    shape.Mask(shapeOrBorder, fringe);

    buildAndQueueBumpMapFromHeightMap(tireBump, shape);


    Channel noise;
    noise.Random();
    noise.Pow(2.f);
    noise.Scale(0, 0.2f);
    noise *= shapeOrBorder;

    queueTexture(tireSpecular,
		 noise,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy, GL_REPEAT);


    shape.Scale(0.35f, 0.7f);

    queueTexture(tire,
		 shape, shape, shape,
		 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		 maxAnisotropy, GL_REPEAT);
  }

  static void buildEmboutissage1Bump()
  {
    //
    // Test de relief d'emboutissage 1
    //
    // http://upload.wikimedia.org/wikipedia/commons/a/a7/Kibo_PM_and_ELM-PS.jpg

    Channel h;
    h.Fade(0); h *= 2; h.Mod();
    h.Clamp(0.1f, 0.6f);

    h.Scale(0, 4.f); h.Mod();
    h.ClampAndScale(0.75f, 1.f);

    Channel tmp;
    tmp.Fade(90);
    tmp.Clamp(0.04f, 0.96f);
    tmp.Scale(0, 6.f); tmp.Mod();
    tmp.ClampAndScale(0.9f, 1.f);

    h.MinimizeTo(tmp);

    tmp.Square();
    tmp.ClampAndScale(0.05f, 0.1f);

    h.MaximizeTo(tmp);
    h.Pow(0.7f);
    h.GaussianBlur();


    Channel c;
    c.Conic();

    c.ClampAndScale(0.98f, 0.995f);
    c.Pow(0.5f);
    c.Scale(0, 0.2f);
    c.Translate(-0.445f, -0.445f);

    Channel cc = c;
    for (int i = 0; i < 18; ++i)
    {
      c.Translate(0.05f, 0);
      cc += c;
    }
    c = cc;
    cc.Rotate90(); c += cc;
    cc.Rotate90(); c += cc;
    cc.Rotate90(); c += cc;

    h.MinimizeTo(c);

    buildAndQueueBumpMapFromHeightMap(emboutissage1Bump, h);
  }

  static void buildEmboutissage2Bump()
  {
    //
    // Test de relief d'emboutissage 2
    //
    // http://upload.wikimedia.org/wikipedia/commons/0/0c/ISS_Quest_airlock.jpg

    Channel t;
    t.Fade(0);
    t.Clamp(0.1f, 0.9f);
    t.Scale(0, 2.f); t.Mod();
    t.ClampAndScale(0.1f, 0.2f);

    Channel tmp;
    tmp.Fade(90);
    tmp.Clamp(0.125f, 0.875f);
    tmp.Scale(0, 4.f); tmp.Mod();
    tmp.ClampAndScale(0.05f, 0.25f);
    t.MaximizeTo(tmp);
    motionMotion(t, 0.01f, 0.01f);
    t.ClampAndScale(0.3f, 0.7f);

    tmp.Square();
    tmp.ClampAndScale(0.2f, 0.25f);
    motionMotion(tmp, 0.01f, 0.01f);
    tmp.ClampAndScale(0.3f, 0.7f);
    t += tmp;

    tmp.Square();
    tmp.ClampAndScale(0.1f, 0.15f);
    motionMotion(tmp, 0.01f, 0.01f);
    tmp.ClampAndScale(0.3f, 0.7f);
    t += tmp;

    t.Scale(0, 1.f);

    buildAndQueueBumpMapFromHeightMap(emboutissage2Bump, t);
  }

  static void buildEmboutissage3Bump()
  {
    //
    // Test de relief d'emboutissage 3
    //
    // http://upload.wikimedia.org/wikipedia/commons/0/0c/ISS_Quest_airlock.jpg

    Channel t;
    t.Square();
    t.ClampAndScale(1.f, 0.2f);

    Channel tmp;
    tmp.Fade(45);
    tmp.Clamp(0.5f, 1.f);
    tmp.Scale(0, 2.f); tmp.Mod();
    tmp.Scale(0.28f, 1.f);

    t.MinimizeTo(tmp);
    tmp.HorizontalFlip();
    t.MinimizeTo(tmp);

    t.ClampAndScale(0.85f, 0.95f);

    tmp.Square();
    tmp.ClampAndScale(0.1f, 0.2f);
    t.MaximizeTo(tmp);

    motionMotion(t, 0.02f, 0.02f);

    t.ClampAndScale(0.3f, 0.7f);

    buildAndQueueBumpMapFromHeightMap(emboutissage3Bump, t);
  }
  */

  static void buildGroundNoise()
  {
    Texture::Channel t;
    t.Cells(15);

    Texture::Channel t2;
    t2.Random();
    t2.VerticalMotionBlur(0.01f);
    t2.GaussianBlur();
    t2.Scale(0.f, 0.5f);

    t += t2;
    t.AverageScale(5, 1.5f);

    t.Scale(0.f, 1.f);
    t.ClampAndScale(0.2f, 0.7f);
    queueTexture(groundNoise, t, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, maxAnisotropy, GL_REPEAT);
  }
}
