#ifndef FBOID_HH
# define FBOID_HH

namespace FBO
{
  typedef enum
  {
    postProcess = 0,
//     postProcessPong,
    postProcessDownscale2,
    postProcessDownscale4,
    postProcessDownscale8,
    postProcessDownscale8Pong,

    trails,

    numberOfFBOs,
    none
  } id;
}

#endif // FBOID_HH
