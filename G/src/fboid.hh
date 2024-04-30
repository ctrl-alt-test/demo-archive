#ifndef FBOID_HH
# define FBOID_HH

namespace FBO
{
  enum id
  {
    baseRender = 0,
    fullSizePong,

    postProcessDownscale2,
    postProcessDownscale4,
    downScaleRender,
    downScalePong,
    glow,
    hStreak,
    vStreak,

    shadowMap,
    shadowMapPong,

    particlesPositionsPing,
    particlesPositionsPong,

    numberOfFBOs,
    none
  };
}

#endif // FBOID_HH
