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
      candle,

      aluminium,
      brass,
      bronze,
      gold,
      iron,
      silver,

      font,

//       defaultBump,
//       defaultSpecular,

      // Divers pièce
      armchair,
      armchairBump,
      brick,
      ceilBump,
      clock,
      doorBump,
      painting,
      paintingBump,
      paintingV,
      sky,
      wall,
      wallBump,

      // Bureau
      paper,
      pen,
      penBump,
      mbCylinder,
      mbCylinderBump,

      // Bibliothèque
      book,
      bookBump,
      greetingsBooks,
      greetingsBooksBump,
      flute,
      fluteOrnaments,
      soufflet,
      souffletBump,

      // Etagère
      puppetHead,
      puppetBody,
      puppetBump,
      flag,
      trainSign,
      railBump,
	  orgalameBump,

      // Bois
      wood,
      wood_Pauvre,
      wood_TendreFrais,
      wood_TendreSec,
      wood_Lambris,
      wood_MerisierClair,
      wood_MerisierFonce,
      wood_Chene,
      wood_Noyer,
      wood_MerisierClairAliceJuliette,
//       wood6,
      wood_NoyerB,
//       wood8,
      woodBump1,
      woodBump2,
      woodBump3,

      chessBoard,
      chessBoardBump,

      felix,

      // Post
      irisBokeh,
      grainMap,


      // Textures dynamiques

//       reflectionMap,
//       ambientMap,

      postProcessRender,
      velocityMap,
      depthMap,
      postProcessPong,
      postProcessPong2,

      shadowMap,
      varianceShadowMap,
      varianceShadowMapPong,

      postProcessDownscale2,
      postProcessDownscale4,
      postProcessDownscale8,
      postProcessDownscale8Pong,


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
