//
// Tous les VBOs utilisés
//

#include "textures.hh"

#include "sys/msys.h"

#include "array.hh"
#include "bookshelf.hh"
#include "intro.hh"
#include "picoc_binding.hh"
#include "shaderprogram.hh"
#include "textureid.hh"
#include "texgen/texture.hh"
#include "tweakval.h"

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

    textureDescription(id texId,
		       const Channel & a,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed):
      texId(texId),
      r(NULL), g(NULL), b(NULL), a(new Channel(a)),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
    {}

    textureDescription(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed):
      texId(texId),
      r(new Channel(r)), g(new Channel(g)), b(new Channel(b)), a(NULL),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
    {}

    textureDescription(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       const Channel & a,
		       GLint minFilter, GLint maxFilter,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed):
      texId(texId),
      r(new Channel(r)), g(new Channel(g)), b(new Channel(b)), a(new Channel(a)),
      minFilter(minFilter), maxFilter(maxFilter),
      maxAnisotropy(anisotropic ? maxMaxAnisotropy : 1.f), tiling(tiling),
      compressed(compressed)
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
#endif
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
		     bool compressed)
  {
    queueTexture(textureDescription(texId, a,
				    min, max, anisotropic, tiling,
				    compressed));
  }

  void queueTextureRGB(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint min, GLint max,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed)
  {
    queueTexture(textureDescription(texId, r, g, b,
				    min, max, anisotropic, tiling,
				    compressed));
  }

  void queueTextureRGBA(id texId,
			const Channel & r,
			const Channel & g,
			const Channel & b,
			const Channel & a,
			GLint min, GLint max,
			bool anisotropic,
			GLint tiling,
			bool compressed)
  {
    queueTexture(textureDescription(texId, r, g, b, a,
				    min, max, anisotropic, tiling,
				    compressed));
  }

  static void dequeueCurrentTexture(textureDescription* desc)
  {
    assert(desc);

    DBG("texture %u: sending to GPU", desc->texId);

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
					 bool repeat)
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
		     false);
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

#if DEBUG
#include "stb_image.hh"
  // Charge une texture à partir de jpg ou png pour le développement/debug
  void loadTextureFromFile(const char* filename, id texId)
  {
    int x, y, n;
    unsigned char *data = stbi_load(filename, &x, &y, &n, 0); // 4 components per pixel

    assert(x > 0);
    assert(x < 2000);

    Channel r(x, y);
    Channel g(x, y);
    Channel b(x, y);
    Channel a(x, y);

    unsigned char *ptr = data;
    const float inv = 1.f / 255.f;
    for (int pos = 0; pos < x * y; pos++)
      {
	r[pos] = *ptr++ * inv;
	g[pos] = *ptr++ * inv;
	b[pos] = *ptr++ * inv;
	a[pos] = n == 4 ? *ptr++ * inv : 1.f;
      }
    stbi_image_free(data);

    queueTextureRGBA(texId,
		     r, g, b, a,
		     GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		     true, GL_REPEAT, false);
  }
#endif

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
      {flatBump,        127, 127, 255, 255},
      {lightBulb,       255, 243, 176, 0},
      {candle,          230,  20,   5, 255},
      {soufflet,        105,  38,  56, 255},

      {aluminium,       200, 200, 200, 255},
      {brass,           254, 240, 181, 255},
      {bronze,          205, 127,  50, 255},
      {gold,            255, 192,   0, 255},
      {iron,            121, 120, 120, 255}, // FIXME
      {silver,          200, 200, 200, 255},
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

  static int colors[] = {
    23, 10,  3, // Noir
    0,  38, 22, // Vert
    41,  2,  0, // Rouge
    2,   4, 28, // Bleu
    51, 31, 15, // Marron
  };
  static char * fontNames[] = {
    "Georgia",
    "Impact",
    "Palatino Linotype",
    "MS Serif",
    "Times New Roman",
    "Verdana",
  };

  Channel * booksR;
  Channel * booksG;
  Channel * booksB;
  Channel * booksA;
  Channel * aliceJulietteA;
  void buildStickers()
  {
    Shader::Program::unUse();
    glEnable(GL_TEXTURE_2D);

    Unit sticker;// = list[greetingsBooks];
    sticker.setRenderTexture(1024, 1024, false, true,
			     GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
			     8.f
			     DBGARG("greetingBooks"));

    glViewport(0, 0, sticker.width(), sticker.height());
    Camera::orthoProj(sticker.width(), sticker.height());

    const int n = (BookShelf::numberOfGreetingsBooks() + 1)/2;
    for (int i = 0; i < BookShelf::numberOfGreetingsBooks(); ++i)
    {
      const BookShelf::bookDesc & book = BookShelf::greetingsBook(i);

      glClearColor(colors[3 * book.color] / 255.f,
		   colors[3 * book.color + 1] / 255.f,
		   colors[3 * book.color + 2] / 255.f,
		   0.f);
      glClear(GL_COLOR_BUFFER_BIT);

      intro.sticker->selectFont(fontNames[book.fontName], book.fontSize);
      intro.sticker->print(book.title, _TV(20.f), _TV(36.f), 255, 251, 216);

      // Position dans la texture
      int x = _TV(0) + (sticker.width() / 2) * (i/n);
      int y = _TV(0) + msys_ifloorf((float(sticker.height())/n) * (n - 1 - i%n));
      sticker.renderToTexture(x, y,
			      // Zone à copier
			      _TV(0), _TV(0),
			      (sticker.width()/2),
			      (sticker.height()/n));
    }

    // FIXME : on pourrit quand même le loading :-(
    // --------------------------------------
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, intro.xres, intro.yres);
    glClear(GL_COLOR_BUFFER_BIT);
    // --------------------------------------

    unsigned char * r = NULL;
    unsigned char * g = NULL;
    unsigned char * b = NULL;
    unsigned char * a = NULL;
    sticker.dumpFromGL(&r, &g, &b, &a);

    booksR = new Channel(sticker.width(), sticker.height(), r);
    booksG = new Channel(sticker.width(), sticker.height(), g);
    booksB = new Channel(sticker.width(), sticker.height(), b);
    booksA = new Channel(sticker.width(), sticker.height(), a);

    msys_mallocFree(r);
    msys_mallocFree(g);
    msys_mallocFree(b);
    msys_mallocFree(a);


    Unit burnt;
    burnt.setRenderTexture(512, 512, false, true,
			   GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
			   8.f
			   DBGARG("Alice & Juliette"));

    glViewport(0, 0, burnt.width(), burnt.height());
    Camera::orthoProj(burnt.width(), burnt.height());

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    intro.sticker->selectFont(fontNames[BookShelf::palatino], 48, FW_NORMAL, TRUE);
    intro.sticker->print("Alice,", _TV(340.f), _TV(370.f), 255, 255, 255);
    intro.sticker->print("Juliette", _TV(10.f), _TV(370.f), 255, 255, 255);
    intro.sticker->selectFont(fontNames[BookShelf::palatino], 28, FW_NORMAL, TRUE);
    intro.sticker->print("18 novembre 2011", _TV(0.f), _TV(300.f), 255, 255, 255);

    burnt.renderToTexture(0, 0, 0, 0, 512, 512);

    // FIXME : on pourrit quand même le loading :-(
    // --------------------------------------
    glViewport(0, 0, intro.xres, intro.yres);
    glClear(GL_COLOR_BUFFER_BIT);
    // --------------------------------------

    r = NULL;
    g = NULL;
    b = NULL;
    a = NULL;
    burnt.dumpFromGL(&r, &g, &b, &a);
    aliceJulietteA = new Channel(burnt.width(), burnt.height(), a);
    msys_mallocFree(r);
    msys_mallocFree(g);
    msys_mallocFree(b);
    msys_mallocFree(a);
  }

  void buildStickersSafe()
  {
    // Greetings: couleur
    {
      Channel & r = *booksR; r.Rotate90();
      Channel & g = *booksG; g.Rotate90();
      Channel & b = *booksB; b.Rotate90();

      queueTextureRGB(greetingsBooks,
		      r, g, b,
		      GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		      true, GL_REPEAT, true);
    }

    // Greetings: Bump map
    {
      Channel & src = *booksA; src.Rotate90();
      src.Dilate(_TV(0.005f));

      Channel cuir(src.Width(), src.Height());
      cuir.Random();
      cuir.GaussianBlur();
      cuir.Pow(1.5f);
      cuir.Scale(1.f, _TV(0.85f));

      src.Scale(1.f, 0);
      src.Pow(_TV(0.4f));
      src *= cuir;

      buildAndQueueBumpMapFromHeightMap(greetingsBooksBump, src, true);
    }

    // Faire part
    delete booksR;
    delete booksG;
    delete booksB;
    delete booksA;
  }

  void buildFairePart()
  {
    unsigned char * r = NULL;
    unsigned char * g = NULL;
    unsigned char * b = NULL;
    unsigned char * a = NULL;

    Unit & wood = list[wood_MerisierClair];
    wood.dumpFromGL(&r, &g, &b, &a);

    Channel fpR(wood.width(), wood.height(), r);
    Channel fpG(wood.width(), wood.height(), g);
    Channel fpB(wood.width(), wood.height(), b);

    aliceJulietteA->Scale(1.f, 0.4f);
    fpR *= *aliceJulietteA;
    fpG *= *aliceJulietteA;
    fpB *= *aliceJulietteA;

    // Il n'y aura pas de dequeue, donc il faut le faire immédiatement
    textureDescription(wood_MerisierClairAliceJuliette,
		       fpR, fpG, fpB,
		       GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
		       true, GL_REPEAT, false).sendTextureToCard();

    msys_mallocFree(r);
    msys_mallocFree(g);
    msys_mallocFree(b);
    msys_mallocFree(a);
    delete aliceJulietteA;
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
  
// Corps des fonctions génératrices
#define TEXTURE_EXPOSE TEXTURE_BUILDER_BODY
#include "../data/textures/textureList.cc"
#undef TEXTURE_EXPOSE


  typedef void(*textureBuilder)();

  const textureBuilder threadUnsafeBuilders[] =
    {
      buildPlainTextures,
      buildStickers,
    };

  const textureBuilder threadSafeBuilders[] =
    {
// Noms des fonctions génératrices
#define TEXTURE_EXPOSE TEXTURE_BUILDER_NAME
#include "../data/textures/textureList.cc"
#undef TEXTURE_EXPOSE
      buildStickersSafe
    };

#if DEBUG
  const char* textureFilenames[] =
    {
// Fichiers des fonctions
#define TEXTURE_EXPOSE TEXTURE_FILE
#include "../data/textures/textureList.cc"
#undef TEXTURE_EXPOSE
      NULL
    };
#endif

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
      msys_srand(i); // initialisation de la seed pour avoir des résultats reproductibles
      threadSafeBuilders[i]();
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

    DBG("Anisotropie max : %.2f", maxMaxAnisotropy);
    maxMaxAnisotropy = min(maxMaxAnisotropy, 8.f);
    DBG("Anisotropie choisie : %.2f", maxMaxAnisotropy);

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
      OGL_ERROR_CHECK("Texture::loadTextures i = %u / %d", i, numberOfThreadUnsafeBuilders);
    }


    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    threadsNumber = sysinfo.dwNumberOfProcessors;
    DBG("textures: using %u threads", threadsNumber);

    const unsigned int numberOfThreadSafeBuilders = sizeof(threadSafeBuilders) / sizeof(textureBuilder);
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

    // Une texture de plus, qui dépend d'une autre texture
    buildFairePart();

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

    int i;
    for (i = 0; ; i++)
    {
      assert(textureFilenames[i] != NULL); // le fichier n'est pas dans la liste ?!
      if (strcmp(textureFilenames[i], filename) == 0) break;
    }
    msys_srand(i);
    if (_TV(0)) // passer à 1 pour ne pas utiliser picoc
      threadSafeBuilders[i]();
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
#endif
}
