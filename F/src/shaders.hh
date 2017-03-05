//
// Tous les shaders utilisés
//

#ifndef		SHADERS_HH
# define	SHADERS_HH

#include "loading.hh"
#include "shaderprogram.hh"
#include "shaderid.hh"

namespace Shader
{
  extern Program * list;

  //
  // Cette liste est en partie dupliquée dans :
  // - shaderid.hh
  // - shaders.cc (deux fois)
  //
  struct State
  {
    float textureFade;       // Poids entre couleur et couleur secondaire
    float trans;             // transition par texture
    float cameraMatrix[16];  // Matrice de la caméra

    float glowness;          // importance du glow (~contraste)
    float focus;             // Distance de focus, entre 0. et 1.
    float skip;              // Gros hack pour le bokeh : 0. ou >1.
    float fade;              // permet de faire un fondu
    float fadeLuminance;     // fondu noir ou fondu blanc ?
    float zNear;             // near clipping
    float zFar;              // far clipping
    float invResolution[2];  // inverse screen resolution
    float center[2];         // screen center
    int   time;
  };

  extern State state;

  void loadShaders();

  void setUniform1i(id shaderId, uniforms u, int x);
  void setUniform1f(id shaderId, uniforms u, float x);
  void setUniform2f(id shaderId, uniforms u, float x, float y);
  void setUniform3f(id shaderId, uniforms u, float x, float y, float z);
  void setUniformM4fv(id shaderId, uniforms u, const float m[16]);

  void setState(unsigned int programId);

  GLuint loadShader(GLenum type, const char * src);
#if STATIC_SHADERS
  extern const char *shader_src[];
#else
  char * LoadSource(const char * filename);
#endif
}

#endif		// SHADERS_HH
