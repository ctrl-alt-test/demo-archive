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
      font,
      black,
      darkGray,
      defaultBump,
      defaultSpecular,

// #ifdef DEBUG

      test,
      testBump,

// #endif // DEBUG

      /*
      // "DEMO"
      demo,
      demoBump,
      */

      // Bandes noires et jaunes
      dangerStripes,		// FIXME
//       dangerStripesBump,	// TODO !!
      dangerStripesSpecular,	// TODO !

      // Béton brut
      concrete,
      concreteBump,		// TODO !!
      concreteSpecular,		// TODO !

      // Anti dérapant
      antiSlip,			// FIXME
      antiSlipBump,		// FIXME
      antiSlipSpecular,

      // Cube manipulateur
      manipulator,
      manipulatorBump,
      manipulatorSpecular,

      // Cube validateur
      validator,
      validatorOK,
      validatorKO,
      validatorBump,
      validatorSpecular,	// TODO !

      // Canon
      highParticle,
      thunderBolt,

      // Ciel
      citySky,
      citySkyRoof,
      forestSky,
      forestSkyRoof,
      spaceBox,
      rampBox,
      rampBox2,

      // Usine
      smoke,

      // Autres
      rubiks,
      rubiksBump,
      rubiksSpecular,

      // Companion Cube
      storageCube,
      companionCube,
      companionCubeBump,
      companionCubeSpecular,

      // Cube Mario
      mario,

      // Caisses d'entrepôt
      crate,
      crateBump,		// FIXME ? (ressort pas assez)
      crateSpecular,
      crateBlackMesa,
      crateBlackMesaSpecular,

      // Caisses ACME
      crateACME,		// TODO !!
      crateACMESpecular,	// TODO !

      // ???? on garde ou pas ?
//       ironFloor,
//       noise,

      // Utilisé ?
//       pixel,

      // Routes
      roadH, // horizontale
      roadHBump,
      roadHSpecular,		// FIXME (les trois spéculaires n'ont pas les mêmes valeurs)
      roadV, // verticale
      roadVBump,
      roadVSpecular,		// FIXME
      roadO, // autre (virage...)
      roadOBump,
      roadOSpecular,		// FIXME
      pavement,
      pavementBump,
      pavementSpecular,

      // Couleur des voitures
      whiteCar,
      redCar,
      blueCar,
      blackCar,
      greyCar,
      taxiCar,
      carBump,

      // Feux
      redLightOn,
      redLightOff,
      yellowLightOff,
      greenLightOn,
      greenLightOff,

      pharmacy,
      glowingSodium,
      statue,
      statueBump,
      statueSpecular,

      // Herbe
      grass,
      grassBump,		// TODO !!
      grassSpecular,		// TODO !

      // Forêt
      moss,
      mossBump,			// TODO !!
      mossSpecular,		// TODO !
      fuchsia,
      rosace,
      // FIXME : bump et spec pour la texture d'arbre

      // Train
      container,
      container2,

      // Immeubles
      wall1,
      wall1Bump,
      wall1Specular,

      windows1,			// TODO !!!
      windows2,			// TODO !!!
      windows3,
      windows4,
      windows1Bump,
      windows2Bump,		// TODO !!
//       windows3Bump,
      windows4Bump,
      windows1Specular,
      windows2Specular,		// TODO !
//       windows3Specular,
      windows4Specular,

      pacmanWall,
      pacmanWallBump,
      pacmanWallSpecular,

      roof1,
      roof2,
      roof3,
      roof4,
      roof1Bump,		// TODO !!
      roof2Bump,		// TODO !!
      roof3Bump,
      roof4Bump,		// TODO !!
//       roof1Specular,		// TODO !
//       roof2Specular,		// TODO !
//       roof3Specular,
//       roof4Specular,		// TODO !

      // Textures de transition
      transitionBurn,

      // Render to texture
      //live1,
      //depthLive1,

      screenCubeRender,
      screenCubeDepth,

      postProcessRender,
      postProcessDepth,

      numberOfTextures
    } id;
}

#endif		// TEXTURE_ID_HH
