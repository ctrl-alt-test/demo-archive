#ifndef FBOID_HH
# define FBOID_HH

namespace FBO
{
  enum id
  {
    postProcess = 0,
    postProcessPong,

    postProcessDownscale2,
    postProcessDownscale4,
    postProcessDownscale8,
    postProcessDownscale8Pong,

    shadowMap,
    shadowMapPong,

    numberOfFBOs,
    none
  };
}

#endif // FBOID_HH
