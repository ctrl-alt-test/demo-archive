//
// Texture unit
//

#ifndef		TEXTURE_USAGE_HH
# define	TEXTURE_USAGE_HH

namespace Texture
{
  enum usage
    {
      // Nommage pour le forward rendering
      albedo   = 0,
      albedo2  = 1,
      normal   = 2,
      material = 3,
      shadow   = 4,
      fft      = 5,

      // Nommage pour le post processing
      color    = 0,
      velocity = 1,
      glow     = 2,
      hStreak  = 3,
      vStreak  = 4,
      depth    = 5,

      data     = 6,
      data2    = 7,

      numberOfUsages // Ne pas dépasser 8 (question de hardware)
    };
}

#endif // TEXTURE_USAGE_HH
