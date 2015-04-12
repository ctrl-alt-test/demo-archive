//
// Identifiants de toutes les animations utilisées
//

#ifndef		ANIM_ID_HH
# define	ANIM_ID_HH

namespace Anim
{
  typedef enum
    {
      none = 0,

      // Usine
      rotorPistonCycle,
      fall,
      moveToTreadmill,
      arrivalCubes,
      fastTreadmill,
      fakeBeamCubes,
      grab1,
      grab2,
      grabValidated,
      grabbed1,
      grabbed2,
      grabbedValidated,
      beforeTest,
      slowTreadmill,
      validatorCycle,
      goToPit,
      pitTrack,
      pitObject,

      // Canon Zeus
      zeusHeart,
      zeusSatellite,
      zeusArm1,
      zeusArm2,
      zeusInner,
      zeusFinger,
      zeusVortex1,
      zeusVortex2,
      zeusVortex3,
      zeusVortex4,
      zeusVortex5,

      lightningForward,
      lightningBackward,
      sphereDie,
      dance1,
      dance2,

      // Ville
      smoke,
      wagon,
      wagonCube1,
      wagonCube2,
      wagonCube3,
      wagonCube4,
      car,
      car2,
      rails,
      growingBuilding,
      tetris,
      cityWalk1,

      // Pong
      pongLeftPaddle,
      pongRightPaddle,
      pongBall,

      // Starfield
      conwayGrow,
      conwayFromTop,
      conwayFromBottom,
      conwayFromLeft,
      conwayFromRight,
      conwayDie,
      conwayCube,

      // Ramp
      rampTransition,
      rampNode,
      rampCube,

      // Forêt
      arrivingForestCube,
      growingTree,
      leaf,
      forestWalk,
      forestJumpingCube,

      // Rope
      ropeNode,
      ropeCube,

      // Écran
      joinScreen,

      numberOfAnims
    } id;
}

#endif		// ANIM_ID_HH
