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

  void createTextureList();
  void loadTextures(Loading::ProgressDelegate * pd, int low, int high);
}

#endif		// TEXTURES_HH
