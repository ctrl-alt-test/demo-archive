// ============================================================================
//
// Test du generateur de textures
//
// ============================================================================

#include	<iostream>
#include	<cstdlib>
#include	<map>
#include	"noise.hh"
#include	"test.hh"
#include	"texture.hh"

Texture::Channel displayed_texture[3];
int texture_size = 256;

#include <iostream>
#include <string>

//int yyparse();
//extern FILE *yyin;

//void parse_string(char *);

extern "C" {
#include "../../tools/picoc/picoc.h"
}

namespace PicocTex
{
  extern struct LibraryFunction funcList[];
}

std::map<const char*, Texture::Channel*> textures_list;
void register_texture(const char *name, Texture::Channel *t)
{
  textures_list[name] = t;
}

void do_picoc(char *file)
{
  Initialise(PicocTex::funcList);
  PlatformScanFile(file);
  Cleanup();
}

int	main(int argc, char* argv[])
{
  //yyin = NULL;
  // Use parse_string to use a string as input.
  // parse_string ("a=2; print_int(plus(4, a));");
  //yyparse ();
  if (argc < 3) {
    std::cout << "Usage: ./texgen script output" << std::endl;
    return 42;
  }
  do_picoc(argv[1]);
  //std::cout << "End." << std::endl;
  //scanf("\n");
  //  TestTexture ();
  std::cout << texture_size << std::endl;
  std::string name = argv[2];
  RAW(displayed_texture[0], displayed_texture[1], displayed_texture[2], name + ".0.raw");

  for (std::map<const char*, Texture::Channel*>::iterator it = textures_list.begin(); it != textures_list.end(); it++)
    RAW(*it->second, name + "." + it->first + ".raw");
}

// ============================================================================
