#include "allTextures.hh"

#include "texgen/texture.hh"
#include "textures.hh"

#include "sys/msys.h"

namespace AllTextures
{
  using namespace Texture;

  // Corps des fonctions génératrices
  #define TEXTURE_LIST "../data/textures/textureList.cc"
  #define TEXTURE_EXPOSE TEXTURE_BUILDER_BODY
  #include TEXTURE_LIST
  #undef TEXTURE_EXPOSE

  const callback textureBuilders[] =
    {
      // Noms des fonctions génératrices
      #define TEXTURE_EXPOSE TEXTURE_BUILDER_NAME
      #include TEXTURE_LIST
      #undef TEXTURE_EXPOSE
    };
  const int textureBuildersLength = ARRAY_LEN(textureBuilders);

  #if DEBUG
    const char* textureFilenames[] =
      {
  // Fichiers des fonctions
  #define TEXTURE_EXPOSE TEXTURE_FILE
  #include TEXTURE_LIST
  #undef TEXTURE_EXPOSE
        NULL
      };
  #endif
}
