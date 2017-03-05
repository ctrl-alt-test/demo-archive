#ifndef PICOC_BINDING_HH_
# define PICOC_BINDING_HH_

#ifndef TEXTURE_CHANNEL_HH
  namespace Texture { class Channel; }
#endif

extern Texture::Channel displayed_texture[3];

// Exemple d'utilisation :
// Picoc::load("data/textures/test.cc");
// Texture t = displayed_texture[0];

namespace Picoc
{
  void init();
  void load(const char *file);
  void loadTexture(const char *file);
}

#endif
