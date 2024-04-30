//
// Tous les shaders utilisés
//

#ifndef		SHADERS_HH
# define	SHADERS_HH

#include "basicTypes.hh"
#include "loading.hh"
#include "shaderid.hh"
#include "shaderprogram.hh"
#include "shaderuniform.hh"

namespace Shader
{
  extern Program * list;

#if DEBUG
  extern const char * Names[];
#endif

  extern ShaderUniform uniforms[numberOfUniforms];

  void loadShaders();

  void setState(id shaderId);

  GLuint loadShader(GLenum type, const char * src DBGARG(compilResult* result));
#if STATIC_SHADERS
  extern const char *shader_src[];
#else
  char * LoadSource(const char * filename);
#endif
}

#endif		// SHADERS_HH
