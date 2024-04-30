//
// Identifiants de toutes les textures utilisées
//

#ifndef		TEXTURE_ID_HH
# define	TEXTURE_ID_HH

namespace Texture
{
  enum id : unsigned char
    {
      none = 0,
      black,
      flatBump,
      lightBulb,

      silver,
      hillsHM,

      font,

//       defaultBump,
//       defaultSpecular,

      gear,
      gearBump,

      // Titre
      titleScreen,
      titleScreenGlow,

      // Post
      roundBokeh,
      grainMap,

      // storyboard
      storyboardStart,
      storyboardEnd = storyboardStart + 26,

      // Textures dynamiques

//       reflectionMap,
//       ambientMap,

      colorBuffer,
      velocityMap,
      depthMap,
      fullSizePong1,
      fullSizePong2,
      circleOfConfusion,
      glowBuffer,
      hStreakBuffer1,
      hStreakBuffer2,
      vStreakBuffer1,
      vStreakBuffer2,

      shadowMap,
      varianceShadowMap,
      varianceShadowMapPong,

      postProcessDownscale2,
      postProcessDownscale4,
      downScaleRender,
      downScalePong1,
      downScalePong2,

      fftMap,
      seagulAnimationsMap,
      particlesPositionsPing,
      particlesPositionsPong,

      // Les textures de test doivent être en dernier :
      // ça permet de faire une comparaison d'id
#ifdef DEBUG

      albedoTest,
      testBump,
      testBump2,
      testSpecular,
//       dummyRelief,
//       dummyBumpRelief,

#endif // DEBUG

      numberOfTextures
    };
}

#endif		// TEXTURE_ID_HH
