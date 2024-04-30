//
// Tous les VBOs utilisés
//

#include "textures.hh"

#include "allTextures.hh"
#include "array.hxx"
#include "demo.hh"
#include "intro.hh"
#include "picoc_binding.hh"
#include "shaderprogram.hh"
#include "spline.hh"
#include "textureid.hh"
#include "texgen/texture.hh"
#include "tweakval.hh"

#include "sys/msys_debug.h"
#include "sys/msys_libc.h"

#include "../stbimage/stbi_DLL.hh"

namespace Texture
{
  Unit * list = NULL;

  void createTextureList()
  {
    list = new Unit[numberOfTextures];
  }

  static float maxMaxAnisotropy = 0;

  struct textureDescription
  {
    id		texId;
    Channel *	r;
    Channel *	g;
    Channel *	b;
    Channel *	a;
    GLint	minFilter;
    GLint	maxFilter;
    GLfloat	maxAnisotropy;
    GLint	tiling;
    bool	compressed;
    IFDBG(const char * description;)

    textureDescription(id texId,
		       const Channel & a,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed
		       DBGARG(const char * description)):
      texId(texId),
      r(NULL), g(NULL), b(NULL), a(new Channel(a)),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
      DBGARG(description(description))
    {}

    textureDescription(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed
		       DBGARG(const char * description)):
      texId(texId),
      r(new Channel(r)), g(new Channel(g)), b(new Channel(b)), a(NULL),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
      DBGARG(description(description))
    {}

    textureDescription(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       const Channel & a,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed
		       DBGARG(const char * description)):
      texId(texId),
      r(new Channel(r)), g(new Channel(g)), b(new Channel(b)), a(new Channel(a)),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
      DBGARG(description(description))
    {}

    void sendTextureToCard()
    {
      Unit & target = list[texId];

      if (NULL == r)
      {
	assert(a != NULL);
	target.setData(*a, compressed);
      }
      else if (NULL == a)
      {
	assert(r != NULL && g != NULL && b != NULL);
	target.setData(*r, *g, *b, compressed);
      }
      else
      {
	assert(r != NULL && g != NULL && b != NULL && a != NULL);
	target.setData(*r, *g, *b, *a, compressed);
      }

      target.setFiltering(minFilter, maxFilter, maxAnisotropy);
      target.setTiling(tiling, tiling);
      target.load();

#if DEBUG
      // On ne veut pas compter les shaders de debug dans le chargement
      if (texId < albedoTest)
#endif // DEBUG
      {
	Loading::update();
      }
    }
  };

  Array<textureDescription> * textureQueue = NULL;

  CRITICAL_SECTION builderMutex;
  CRITICAL_SECTION textureMutex;
  static unsigned int currentBuilderIndex = 0;
  static int currentTextureIndex = 0;
  static unsigned threadsNumber = 2;

  static void callBuilders(void*);
  static void dequeueCurrentTexture(textureDescription* desc);

  // FIXME : destruction des textures


  // ==========================================================================

  static void queueTexture(textureDescription desc)
  {
    EnterCriticalSection(&textureMutex);
    textureQueue->add(desc);
    LeaveCriticalSection(&textureMutex);
  }

  void queueTextureA(id texId,
		     const Channel & a,
		     GLint min, GLint max,
		     bool anisotropic,
		     GLint tiling,
		     bool compressed
		     DBGARG(const char * description))
  {
    queueTexture(textureDescription(texId, a,
				    min, max, anisotropic, tiling,
				    compressed
				    DBGARG(description)));
  }

  void queueTextureRGB(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint min, GLint max,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed
		       DBGARG(const char * description))
  {
    queueTexture(textureDescription(texId, r, g, b,
				    min, max, anisotropic, tiling,
				    compressed
				    DBGARG(description)));
  }

  void queueTextureRGBA(id texId,
			const Channel & r,
			const Channel & g,
			const Channel & b,
			const Channel & a,
			GLint min, GLint max,
			bool anisotropic,
			GLint tiling,
			bool compressed
			DBGARG(const char * description))
  {
    queueTexture(textureDescription(texId, r, g, b, a,
				    min, max, anisotropic, tiling,
				    compressed
				    DBGARG(description)));
  }

  static void dequeueCurrentTexture(textureDescription* desc)
  {
    assert(desc);

    DBG("texture %u: sending to GPU (%s)", desc->texId, desc->description);

    desc->sendTextureToCard();

    delete desc->r;
    delete desc->g;
    delete desc->b;
    delete desc->a;

    desc->r = NULL;
    desc->g = NULL;
    desc->b = NULL;
    desc->a = NULL;
  }

  //
  // Bump mapping
  //
  void buildAndQueueBumpMapFromHeightMap(id texId, Channel & h,
					 bool repeat
					 DBGARG(const char * description))
  {
    Channel r(h.Width(), h.Height());
    Channel g(h.Width(), h.Height());
    Channel b(h.Width(), h.Height());

    for (unsigned int j = 0; j < h.Height(); ++j)
      for (unsigned int i = 0; i < h.Width(); ++i)
      {
	const float dx = h.Pixel(i - 1, j) - h.Pixel(i, j);
	const float dy = h.Pixel(i, j + 1) - h.Pixel(i, j);
	const float dz = msys_sqrtf(1.f - dx * dx - dy * dy);
	r.Pixel(i, j) = 0.5f + dx;
	g.Pixel(i, j) = 0.5f + dy;
	b.Pixel(i, j) = dz;
      }

    queueTextureRGBA(texId,
		     r, g, b, h,
		     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		     true, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE,
		     false
		     DBGARG(description));
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
    l.Scale(0, 1.f);

    // Valeurs trouvées expérimentalement
    l.Pow(0.3f);
    l.Scale(0.25f, 1.25f);
    l.Clamp(0, 1.f);
  }



  void loadTextureFromFile(const char* filename, id texId, bool pixelized)
  {
    int x, y, n;
    unsigned char * data = STB::STBI::Load(filename, &x, &y, &n, 0);
    assert(data != NULL);
    loadTextureFromSTBIData(data, x, y, n, texId, pixelized DBGARG(filename));
    STB::STBI::Free(data);
  }

  void loadTextureFromSTBIData(const unsigned char * data, int x, int y, int n, id texId, bool pixelized DBGARG(const char * name))
  {
    assert(x > 0);
    assert(x < 2000);
    assert(n == 3 || n == 4);

    Channel r(x, y);
    Channel g(x, y);
    Channel b(x, y);
    Channel a(x, y);

    const unsigned char *ptr = data;
    const float inv = 1.f / 255.f;
    for (int pos = 0; pos < x * y; pos++)
      {
        // FIXME: ce n'est pas la bonne façon de gérer le gamma.
        // Idéalement, il faut charger la texture telle quelle
        // (sans le pow 2.2) avec le format GL_SRGB au lieu de
        // GL_RGBA.
        // En appliquant le gamma à la main ici, on perd en
        // précision dans les sombres.
	r[pos] = msys_powf(*ptr++ * inv, 2.2f);
	g[pos] = msys_powf(*ptr++ * inv, 2.2f);
	b[pos] = msys_powf(*ptr++ * inv, 2.2f);
	a[pos] = n == 4 ? *ptr++ * inv : 1.f;
      }

    queueTextureRGBA(texId,
		     r, g, b, a,
		     GL_LINEAR_MIPMAP_LINEAR, pixelized ? GL_NEAREST : GL_LINEAR,
		     true, GL_REPEAT,
		     false
		     DBGARG(name));
  }

  // ==========================================================================

  struct plainTextureDescription
  {
    id			textureId;
    unsigned char	r;
    unsigned char	g;
    unsigned char	b;
    unsigned char	a;
  };

  static void buildPlainColor(const plainTextureDescription & desc)
  {
    Unit & target = list[desc.textureId];
    const unsigned char col[] = { desc.r, desc.g, desc.b, desc.a };

    target.setData(1, 1, GL_RGBA, col); // TODO optim: remplacer col par &desc.r
    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP, GL_CLAMP);
    target.load(false);
  }

//   static void buildPlainColor(Texture::id id,
// 			      unsigned char a)
//   {
//     Unit & target = list[id];

//     target.setData(1, 1, GL_ALPHA, &a);
//     target.setFiltering(GL_NEAREST, GL_NEAREST);
//     target.setTiling(GL_CLAMP, GL_CLAMP);
//     target.load(false);
//   }

  static void buildPlainTextures()
  {
    const plainTextureDescription plainTextureDescs[] =
    {
      {none,	      _TV(235), _TV(235), _TV(224), _TV(255)}, // 1-Eigengrau :)
      {black,	      _TV( 20), _TV( 20), _TV( 29), _TV(255)}, // Eigengrau
      {flatBump,      _TV(127), _TV(127), _TV(255), _TV(255)},
      {lightBulb,     _TV(255), _TV(243), _TV(176), _TV(  0)},

      {silver,        _TV(201), _TV(192), _TV(187), _TV(255)},
    };

    const unsigned int numberOfPlainTextures = sizeof(plainTextureDescs) / sizeof(plainTextureDescription);
    for (unsigned int i = 0; i < numberOfPlainTextures; ++i)
    {
      buildPlainColor(plainTextureDescs[i]);
    }
  }

  static void prepareFFTTexture()
  {
    Unit & target = list[Texture::fftMap];

    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    target.setDynamicTexture(256, 256, GL_RED, GL_R32F, GL_FLOAT DBGARG("fftMap"));
  }

  static void prepareSeagulAnimationsTexture()
  {
    Unit & target = list[Texture::seagulAnimationsMap];

    target.setFiltering(GL_NEAREST, GL_NEAREST);
    target.setTiling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    target.setDynamicTexture(16/4, 1000 * 4, GL_RGBA, GL_RGBA32F, GL_FLOAT DBGARG("seagulAnimationsMap"));
  }

  // FIXME: déplacer dans l'objet Channel
  void drawSpline(Channel & tex, float * splineData, int splineDataLen,
		  const float * m)
  {
    for (int i = 0; i < splineDataLen; i += 3)
      splineData[i] = (float)i / (splineDataLen - 3);

    const int steps = 200;
    for (int i = 1; i < steps; i++)
    {
      float t0 = (float) (i-1) / (steps - 1);
      float t1 = (float) i / (steps - 1);
      float p1[2];
      spline(splineData, splineDataLen / 3, 2, t0, p1);
      float p2[2];
      spline(splineData, splineDataLen / 3, 2, t1, p2);

      float p1x = m[0] * p1[0] + m[1] * p1[1] + m[2];
      float p1y = m[3] * p1[0] + m[4] * p1[1] + m[5];

      float p2x = m[0] * p2[0] + m[1] * p2[1] + m[2];
      float p2y = m[3] * p2[0] + m[4] * p2[1] + m[5];
      tex.Line(p1x, p1y, p2x, p2y);
    }
  }

  const callback threadUnsafeBuilders[] =
    {
      buildPlainTextures,
      prepareFFTTexture,
      prepareSeagulAnimationsTexture,
    };

  static void callBuilders(void*)
  {
    unsigned int i;
    const unsigned int numberOfThreadSafeBuilders = AllTextures::textureBuildersLength;

    while (true)
    {
      EnterCriticalSection(&builderMutex);
      i = currentBuilderIndex++;
      LeaveCriticalSection(&builderMutex);

      if (i >= numberOfThreadSafeBuilders)
	break;

      DBG("texture %u: begin", i);
      const long int t1 = GetTickCount();
      AllTextures::textureBuilders[i]();
      const long int t2 = GetTickCount();
      DBG("texture %u: end (%d ms)", i, t2 - t1);
    }
  }

  void loadTextures()
  {
    START_TIME_EVAL;

    assert(list != NULL);
    currentTextureIndex = 0;
    currentBuilderIndex = 0;

    // Initialisation des unités de texture (tableau statique)
    Texture::Unit::unUse();

    textureQueue = new Array<textureDescription>(numberOfTextures);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy);

    DBG("Anisotropy max: %.2f", maxMaxAnisotropy);
    maxMaxAnisotropy = msys_min(maxMaxAnisotropy, 8.f);
    DBG("Actual anisotropy: %.2f", maxMaxAnisotropy);

    //
    // Création des données des textures qui ne peuvent être que dans
    // le thread principal
    //
    const unsigned numberOfThreadUnsafeBuilders = ARRAY_LEN(threadUnsafeBuilders);
    for (unsigned int i = 0; i < numberOfThreadUnsafeBuilders; ++i)
    {
      DBG("Loading texture %u", i);
      const long int t1 = GetTickCount();
      threadUnsafeBuilders[i]();
      const long int t2 = GetTickCount();
      DBG("-> %d ms", t2 - t1);
      OGL_ERROR_CHECK("Texture::loadTextures i = %u / %d", i, numberOfThreadUnsafeBuilders);
    }


    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    threadsNumber = sysinfo.dwNumberOfProcessors;
    DBG("textures: using %u threads", threadsNumber);

    const unsigned int numberOfThreadSafeBuilders = AllTextures::textureBuildersLength;
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
      builderIndex = currentBuilderIndex;

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

    END_TIME_EVAL("Textures generation");
  }

#if DEBUG
//  typedef void(*textureBuilder)();
  void reloadSingleTexture(const char *filename)
  {
    // Ce n'est pas threadé ici, mais on appelle des fonctions qui utilisent le lock
    InitializeCriticalSection(&textureMutex);
    textureQueue = new Array<textureDescription>(numberOfTextures);

    if (strcmp(filename, "src/textures.cc") == 0) {
      buildPlainTextures();
      return;
    }

    int i;
    for (i = 0; ; i++)
    {
      assert(AllTextures::textureFilenames[i] != NULL); // le fichier n'est pas dans la liste ?!
      const char * file = strstr(AllTextures::textureFilenames[i], "data");
      if (strcmp(file, filename) == 0) break;
    }

    if (_TV(0)) // passer à 1 pour ne pas utiliser picoc
      AllTextures::textureBuilders[i]();
    else
      try {
        Picoc::loadTexture(filename);
      }
      catch (char *) { return; }

    textureDescription * desc = NULL;
    while (textureQueue->size > 0)
    {
      desc = & textureQueue->last();
      dequeueCurrentTexture(desc);
      textureQueue->pop();
    }
    DeleteCriticalSection(&textureMutex);
    delete textureQueue;
  }
#endif // DEBUG
}
