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

#if DEBUG
  void buildStickers();

  const char* textureFilenames[];
  void reloadSingleTexture(const char *filename);

  void queueTextureRGB(id texId,
		       const Channel & r,
		       const Channel & g,
		       const Channel & b,
		       GLint min, GLint max,
		       bool anisotropic,
		       GLint tiling,
		       bool compressed);
  void queueTextureRGBA(id texId,
			const Channel & r,
			const Channel & g,
			const Channel & b,
			const Channel & a,
			GLint min, GLint max,
			bool anisotropic,
			GLint tiling,
			bool compressed);
  void queueTextureA(id texId,
		     const Channel & a,
		     GLint min, GLint max,
		     bool anisotropic,
		     GLint tiling,
		     bool compressed);
  void buildAndQueueBumpMapFromHeightMap(id texId, Channel & h,
					 bool repeat = true);
#endif
}

#define TEXTURE_NAMES             1
#define TEXTURE_BUILDER_NAME      2
#define TEXTURE_BUILDER_BODY      3
#define TEXTURE_FILE              4

#endif		// TEXTURES_HH
