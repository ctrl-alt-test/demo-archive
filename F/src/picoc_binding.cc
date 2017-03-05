// Ce fichier est spécifique par la démo et permet de s'interfacer avec.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

extern "C" {
  #include "../tools/picoc/picoc.h"
}
#include "sys/msys.h"
#include "texgen/texture.hh"

// FIXME: mettre ça dans un namespace (et mettre à jour l'autre texgen)

// taille des textures créées dans picoc
unsigned int texture_size = 512;
  
// la fonction show copie les textures ici
Texture::Channel displayed_texture[3];

#define MAX_TEXTURES 100
Texture::Channel* registered_textures[MAX_TEXTURES];
int numberOfTexturesInPicoc = 0;

// Cette fonction est appelée à chaque fois qu'une texture est créée dans picoc
void register_texture(const char* s, Texture::Channel *t)
{
  assert(numberOfTexturesInPicoc < MAX_TEXTURES);
  registered_textures[numberOfTexturesInPicoc] = t;
  numberOfTexturesInPicoc++;
}

namespace Picoc
{
  void init()
  {
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
      registered_textures[i] = NULL;
    }
  }

  static void reinit()
  {
    for (int i = 0; i < MAX_TEXTURES; i++)
    {
      if (registered_textures[i] != NULL)
	delete registered_textures[i];
      registered_textures[i] = NULL;
    }
    numberOfTexturesInPicoc = 0;
    Initialise();
  }

  // Charge un fichier et l'exécute avec picoc
  void load(const char *file)
  {
    reinit();
    PlatformScanFile(file);
  }

  void loadTexture(const char *file)
  {
    char command[4096] = { 0 };
    strcat(command, "tools\\sed.exe -f src/cpp_to_picoc.sed ");
    strcat(command, "src/picoc_prelude.cc src/textureid.hh ");
    strcat(command, file);
    strcat(command, " > picoc_gen.cc");
    system(command);
    load("picoc_gen.cc");
  }
}
