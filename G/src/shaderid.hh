//
// Identifiants de tous les shaders utilisés
//

#ifndef		SHADER_ID_HH
# define	SHADER_ID_HH

#if DEBUG
# define STATIC_SHADERS 0
#else
# define STATIC_SHADERS 1 // shaders en dur pour la release
#endif

namespace Shader
{
#define SHADER_ID(x) x
  enum id : unsigned char
    {
      none = 0,
#include "shaderid.def"
      numberOfShaders
    };
#undef SHADER_ID

  //
  // Cette liste impacte le tableau uniformDefinitions déclaré dans shaders.cc
  //
  enum uniformId
    {
      // Texture units
      tex0 = 0, // albedo   / color
      tex1,     // albedo2  / velocity
      tex2,     // normal   / glow
      tex3,     // material / hStreak
      tex4,     // shadow   / vStreak
      tex5,     // fft      / depth
      tex6,     // data     / data
      tex7,     // data2    / other data

      // Light parameters
      ambient,       // Ambient color
      l0Color,       // Light 0 color
      l0Position,    // Light 0 position
      l0Attenuation, // Light 0 attenuation
      l1Color,       // Light 1 color
      l1Position,    // Light 1 position
      l1Attenuation, // Light 1 attenuation

      // Wireframe
      retroEdgeThick,// x: edge thickness; y: edge flatness
      retroFaceColor,// Face color in retro mode
      retroEdgeColor,// Edge color in retro mode
      retroParams,   // x, y: face/edge retro amount ; z, w: face/edge shading amount

      // Lens effects
      lensFishEye,   // Déformation de la lentille
      lensGlowPower, // Taux de réduction du glow
      lensStreakPower,//Taux de réduction du streak horizontal et vertical
      lensOrbsSize,  // Lens orbs size (big min/max, small min/max)
      lensOrbsColor, // Lens orbs color
      lensFlareColor,// Lens flare color
      lensGlowColor, // Glow color
      lensStreakColor,//Streak color
      focus,         // Distance de focus, entre 0. et 1.
      skip,          // Gros hack pour le bokeh : 0. ou >1.

      // Other post processing params
      vignetting,    // Intensité du vignetting (0: aucun, 0.4: bonne caméra, 60°) et épaisseur du contour
      grain,         // Intensité et fréquence du grain
      fade,          // permet de faire un fondu
      fadeLuminance, // fondu noir vs fondu blanc

      resolution,    // buffer resolution
      invResolution, // inverse buffer resolution
      center,        // screen center
      pass,          // shader pass

      // Camera and screen
      cameraMatrix,  // Matrice de la caméra
      zNear,         // near clipping
      zFar,          // far clipping

      time,

      // G specific: Y position of the camera
      yPos,
      oldYPos,

      // Sky / fog
      skyColor,      // Couleur du ciel en haut
      fogColor,      // Couleur du ciel à l'horizon
      fogDensity,

      // Spécifique à chaque objet
      objectId,
      birthDate,
      shininess,     // Exposant dans le calcul du spéculaire
      trans,         // transition
      oldModelView,  // Matrice modelview précédente
      textStep,      // Fondu du texte

      numberOfUniforms
    };
}

#endif		// SHADER_ID_HH
