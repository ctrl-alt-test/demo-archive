//
// Identifiants de tous les VBOs utilisés
//

#ifndef		VBO_ID_HH
# define	VBO_ID_HH

namespace VBO
{
  enum id : unsigned char
    {
      singleCube = 0,

      test,
      test2,
      testhm, // heightmap, à virer

      room,
      floor,
      plinthe,
      table,
      bin,
      window,
      blackBackground,

      // FIXME: ces meshs pavés peuvent être virés si le scale marche
      door,
      doorBorder,
      doorHinge,
      doorLock,
      doorKey,
      doorHandle,
      painting,
      paper,
      boatFlag,
      ceil,

      monkey,
      shelf,
      bookShelf,
      greetingsBooks,
      desk,
      toolsWoodPart,
      toolsMetalPart,
      stool,
      luxo,
      lightBulb,
      chimney,
      chimneyTop,
      chimneyInside,
      book,
      armchair,

      // Horloge
      clockBody,
      clockFace,
      clockPendulum,
      clockHandH,
      clockHandM,
      clockHandS,

      // Jouets et déco
      toysPart1,
      toysPart2,
      toysPart3,
      pen,
      bibelot,
      candle,
      candleHolder,
      cup,

      // Marionnettes
      puppetBody,
      puppetHead,
      puppetHat,
      puppetShoulder,
      puppetArm,
      puppetForearm,
      puppetLeg,
      puppetFoot,
      hihatstick,
      xylostick,
      hihatBody,
      hihatHead,

      // Échiquier
      chessPiecesW,
      chessPiecesB,
      chessBoard,
      chessBorder,

      // Tambour
      drumMain,
      drumSkin,
      drumStick,
      drumMetal,
      drumSupport,

      rail,
      tunnel,
      wagon,
      wagonMetal,
      locoMetal,
      wagonWheels,
      locomotive,
      sign,
      wagonAttach,

      // Caroussel
      carouselBody,
      carouselHead,
      carouselBell,
      cyclogratte,
      rope,

      // Boite à musique
      mbPic,
      mbBody,
      mbBox,
      mbKey,
      mbGear,
      mbMechanism,
      mbSmallCylinder,
      mbCover,
      mbDoorKey,
      mbKeyHole,
      mbTriangle,

      // Lousianne
      flute,
      fluteOrnaments,
      fluteKey,
      soufflet,
      plateau,
      lousianneMetal,
      lousianneWheels,

      // Orgalame
      olBase,
      olBlade,
      olPipes,
      olKey,

      // Pas content
      pcBase,
      pcPipes,
      pcPipes2,
      pcSlider,

      numberOfVBOs
    };
}

#endif		// VBO_ID_HH
