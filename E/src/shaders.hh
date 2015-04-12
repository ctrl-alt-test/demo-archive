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

  //
  // Cette liste est en partie dupliquée dans :
  // - shaderid.hh
  // - shaders.cc (deux fois)
  //
  typedef struct
  {
    float textureFade;       // Poids entre couleur et couleur secondaire
    float trans;             // transition par texture
    float cameraMatrix[16];  // Matrice de la caméra

    float glowness;          // importance du glow (~contraste)
    float trail1Color[3];
    float trail2Color[3];
    float trail3Color[3];
    float dof;               // profondeur de champ, entre 0. et 1.
    float fade;              // permet de faire un fondu
    float fadeLuminance;     // fondu noir ou fondu blanc ?
    float zNear;             // near clipping
    float zFar;              // far clipping
    float screenCX;          // screen center
    float screenCY;
    int   time;
  } State;

  extern State state;

  void loadShaders(Loading::ProgressDelegate * pd, int low, int high);

  void setUniform1i(id shaderId, uniforms u, int x);
  void setUniform1f(id shaderId, uniforms u, float x);
  void setUniform3f(id shaderId, uniforms u, float x, float y, float z);
  void setUniform4fv(id shaderId, uniforms u, const float m[16]);

  void setState(unsigned int programId);
}

#endif		// SHADERS_HH
