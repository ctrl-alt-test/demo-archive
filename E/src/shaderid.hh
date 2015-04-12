//
// Identifiants de tous les shaders utilisés
//

#ifndef		SHADER_ID_HH
# define	SHADER_ID_HH

namespace Shader
{
  typedef enum
    {
      none = 0,
//       default,
      parallax,
      ground,

      noPost,
//       holga,
//       television,

      text,
      hBlurPass,
      vBlurPass,

      finalPass,
/*
#if DEBUG
      debugZ,
#endif
*/
      numberOfShaders
    } id;

  //
  // Cette liste est en partie dupliquée dans :
  // - shaders.cc (deux fois)
  // - shaders.hh
  //
  typedef enum
    {
      // Constant sur la durée de la démo
      colorMap = 0,  // Couleur
      sndColorMap,   // Couleur secondaire
      normalMap,     // bump
      specularMap,   // specular
      ambientMap,    // Ambient map
      dynamicMap,    // Couleur dynamique

      // Changeant à chaque frame
      textureFade,   // Poids entre couleur et couleur secondaire
      trans,         // transition par texture
      cameraMatrix,  // Matrice de la caméra

      glowness,      // Importance du glow (~contraste)
      trail1Color,
      trail2Color,
      trail3Color,
      dof,           // profondeur de champ, entre 0. et 1.
      fade,          // permet de faire un fondu
      fadeLuminance, // fondu noir ou fondu blanc ?
      zNear,         // near clipping
      zFar,          // far clipping
      screenCX,      // screen center
      screenCY,
      time,

      // Spécifique à chaque objet
      objectId,
      oldModelView,  // Matrice modelview précédente
      textStep,      // Fondu du texte

      numberOfUniforms
    } uniforms;
}

#endif		// SHADER_ID_HH
