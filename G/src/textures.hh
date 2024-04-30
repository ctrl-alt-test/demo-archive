//
// Toutes les textures utilisées
//

#ifndef		TEXTURES_HH
# define	TEXTURES_HH

#include "loading.hh"
#include "textureunit.hh"
#include "textureid.hh"

namespace Texture
{
  extern Unit * list;

  void createTextureList();
  void loadTextures();

  void buildStickers();
  void loadTextureFromFile(const char * filename, id texId, bool pixelized = false);
  void loadTextureFromSTBIData(const unsigned char * data, int x, int y, int n, id texId, bool pixelized DBGARG(const char * name));

#if DEBUG
  const char* textureFilenames[];
  void reloadSingleTexture(const char *filename);
#endif

  void queueTextureRGB(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint min, GLint max,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed
		       DBGARG(const char * description));
  void queueTextureRGBA(id texId,
			const Channel & r,
			const Channel & g,
			const Channel & b,
			const Channel & a,
			GLint min, GLint max,
			bool anisotropic,
			GLint tiling,
			bool compressed
			DBGARG(const char * description));
  void queueTextureA(id texId,
		     const Channel & a,
		     GLint min, GLint max,
		     bool anisotropic,
		     GLint tiling,
		     bool compressed
		     DBGARG(const char * description));
  void buildAndQueueBumpMapFromHeightMap(id texId, Channel & h,
					 bool repeat = true
					 DBGARG(const char * description = "bump"));

  // FIXME: à virer
  void drawSpline(Channel & tex, float * splineData, int splineDataLen,
		  const float * m);
  void buildStickersSafe();
}

#define TEXTURE_NAMES             1
#define TEXTURE_BUILDER_NAME      2
#define TEXTURE_BUILDER_BODY      3
#define TEXTURE_FILE              4

#endif		// TEXTURES_HH
