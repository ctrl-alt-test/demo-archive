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
  enum id : unsigned char
    {
      none = 0,
      loading,

//       default,
      createVSM,
      parallax,
      anisotropic,
      string,
      lousianne,
      syncTest,
      floor,
      lightSource,
      xylophone,
      drum,

      noPost,
//       holga,
//       television,

      text,
      hBlurPass,
      vBlurPass,

      bokehFstPass,
      bokehSndPass,

      finalPass,
      antialias,
      lensOrbs,

      // Les shaders de test doivent être en dernier :
      // ça permet de faire une comparaison d'id
#if !STATIC_SHADERS
      debugNonLinear,
      debugNormals,
      debugLight,
      debugWhiteLight,
//       debugZBuffer,
#endif // DEBUG

      numberOfShaders
    };

  //
  // Cette liste est en partie dupliquée dans :
  // - shaders.cc (deux fois)
  // - shaders.hh
  //
  enum uniforms
    {
      // Constant sur la durée de la démo
      albedoMap = 0, // Couleur du matériau
      albedo2Map,    // Deuxième couleur
      normalMap,     // Bump
      materialMap,   // Nature du matériau (spéculaire, etc.)
//       ambientMap,    // Ambient map
      shadowMap,     // Ombre

      // Changeant à chaque frame
      textureFade,   // Poids entre couleur et couleur secondaire
      trans,         // transition par texture
      cameraMatrix,  // Matrice de la caméra

      glowness,      // Importance du glow (~contraste)
      focus,         // Distance de focus, entre 0. et 1.
      skip,          // Gros hack pour le bokeh : 0. ou >1.
      fade,          // permet de faire un fondu
      fadeLuminance, // fondu noir ou fondu blanc ?
      zNear,         // near clipping
      zFar,          // far clipping
      invResolution, // inverse screen resolution
      center,        // screen center
      time,

      // Spécifique à chaque objet
      objectId,
      shininess,     // Exposant dans le calcul du spéculaire
      oldModelView,  // Matrice modelview précédente
      textStep,      // Fondu du texte

      numberOfUniforms
    };

  enum vertexAttrib
    {
      vTan = 1,      // Tangeante
      numberOfAttributes
    };
}

#endif		// SHADER_ID_HH
