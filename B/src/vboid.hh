//
// Identifiants de tous les VBOs utilisés
//

#ifndef		VBO_ID_HH
# define	VBO_ID_HH

namespace VBO
{
  typedef enum
    {
    singleCube = 0,
    theCube,

    cubeWalls,
    cubeRoof,

    disjoinedCube,
    shuffledCube,
    resizedCube,
    shearedCube,
    zFightCube,
    rubiksCube,
    sphere,
    treadmillChunk0,
    treadmillChunk1,
    treadmillChunk2,
    treadmillChunk3,
    factoryFrame,

    buildingsWalls0, // Version sans fenêtre
    buildingsWalls1, // Versions avec fenêtres
    buildingsWalls2,
    buildingsWalls3,
    buildingsWalls4,
    buildingsRoofs1, // Toits
    buildingsRoofs2,
    buildingsRoofs3,
    buildingsRoofs4,

    cityGround,
    cityPavement,
    cityStreetH,
    cityStreetV,
    cityStreetO,
    cityBuilding1,
    cityLights,

    forestGround,
    forestTrees,

    redPixel,
    greenPixel,
    bluePixel,
    screen,
//     factoryFloor,
    numberOfVBOs
    } id;
}

#endif		// VBO_ID_HH
