// Ce fichier est spécifique par la démo et permet de s'interfacer avec.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <windows.h>

extern "C" {
  #include "../../tools/picoc/picoc.h"
}
#include "texgen/texture.hh"
#include "picoc_binding.hh"

#include "sys/msys_debug.h"

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

void PlatformLibraryInit()
{
    struct ParseState Parser;
    char *Identifier;
    struct ValueType *ParsedType;
    void *Tokens;
    const char *IntrinsicName = TableStrRegister("platform library");
    // Fake definition - l'utilisateur doit passer par les fonctions
    const char *StructDefinition = "struct Texture{float foo;};";

    /* define an example structure */
    Tokens = LexAnalyse(IntrinsicName, StructDefinition, strlen(StructDefinition), NULL);
    LexInitParser(&Parser, StructDefinition, Tokens, IntrinsicName, TRUE);
    TypeParse(&Parser, &ParsedType, &Identifier);
    //HeapFree(Tokens);
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

  // Charge un fichier et l'exécute avec picoc
  static void load(const char *file, LibraryFunction * funcList)
  {
    Initialise(funcList);
    PlatformScanFile(file);
  }

  void loadTexture(const char *file)
  {
    char command[4096] = { 0 };
    strcat(command, "..\\tools\\sed.exe -f src/cpp_to_picoc.sed ");
    strcat(command, "src/picoc_prelude.cc src/textureid.hh ");
    strcat(command, file);
    strcat(command, " > picoc_gen.cc");
    system(command);

    for (int i = 0; i < MAX_TEXTURES; i++)
    {
      if (registered_textures[i] != NULL)
	delete registered_textures[i];
      registered_textures[i] = NULL;
    }
    numberOfTexturesInPicoc = 0;

    load("picoc_gen.cc", PicocTex::funcList);
  }
}
