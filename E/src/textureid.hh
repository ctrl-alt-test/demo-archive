//
// Identifiants de toutes les textures utilisées
//

#ifndef		TEXTURE_ID_HH
# define	TEXTURE_ID_HH

namespace Texture
{
  typedef enum
    {
      none = 0,
      black,
      font,

//       defaultBump,
//       defaultSpecular,

#ifdef DEBUG

      test,
      testBump,
      testBump2,
      testSpecular,

#endif // DEBUG

      ambientMap,

//       dummyRelief,
//       dummyBumpRelief,

      lightPatterns,
      bumpPatterns,
      specularPatterns,

      buildingLightPatterns,
      buildingBumpPatterns,
      buildingSpecularPatterns,

      groundNoise,

      postProcessRender,
      postProcessPong,
      postProcessVelocity,
      postProcessDepth,
      postProcessDownscale2,
      postProcessDownscale4,
      postProcessDownscale8,
      postProcessDownscale8Pong,

      trails,

      numberOfTextures
    } id;
}

#endif		// TEXTURE_ID_HH
