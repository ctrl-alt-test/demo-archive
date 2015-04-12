//
// Toutes les textures utilisées
//

#ifndef		TEXTURES_HH
# define	TEXTURES_HH

#include "loading.hh"
#include "textureunit.hh"

namespace Texture
{
  extern Unit * list;
  Texture::Channel *getPerlin7();

  void createTextureList();
  void loadTextures(Loading::ProgressDelegate * pd, int low, int high);
  void forestHeightMap(Texture::Channel & t,
		       Texture::Channel & r,
		       Texture::Channel & g,
		       Texture::Channel & b,
		       int size);
}

#endif		// TEXTURES_HH
