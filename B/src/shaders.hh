//
// Tous les shaders utilisés
//

#ifndef		SHADERS_HH
# define	SHADERS_HH

#include "loading.hh"
#include "shaderprogram.hh"
#include "shaderid.hh"

#if DEBUG

#include "sys/msys_forbidden.h"

#endif // DEBUG

namespace Shader
{
  extern Program * list;

  void loadFirstShader();
  void loadShaders(Loading::ProgressDelegate * pd, int low, int high);

  void broadcast1f(uniforms u, float x);
  void broadcast3f(uniforms u, float x, float y, float z);
  void broadcast1i(uniforms u, int x);

  void unicast1i(id shaderId, uniforms u, int x);
  void unicast1f(id shaderId, uniforms u, float x);

  float calcFade(date begin, date end, date d, bool fadein);

#if DEBUG

  // Chargement des shaders depuis des fichiers (pour le dev)
  char * LoadSource(const char * filename);
  void LoadProgram(Program & program, const char * vsname, const char * psname, FILE * out);

#endif // DEBUG

}

#endif		// SHADERS_HH
