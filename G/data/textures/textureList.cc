//
// Liste des textures
//
// Ce fichier va être inclu à différents endroits, avec des macro
// différentes pour récupérer une information différente selon le
// contexte
//

#include "../src/sys/msys_debug.h"

#if DEBUG

// Textures de tests
#include "../data/textures/albedoTest.cc"
#include "../data/textures/bumpTest.cc"
#include "../data/textures/specularTest.cc"

#endif

#define IMAGE_DIR "data/"

#include "../data/textures/gears.cc"
#include "../data/textures/grain.cc"
#include "../data/textures/hillsHM.cc"
#include "../data/textures/roundBokeh.cc"
//#include "../data/textures/storyboard.cc"
#include "../data/textures/titleScreen.cc"
