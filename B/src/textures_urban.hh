#ifndef		TEXTURES_URBAN_HH
# define	TEXTURES_URBAN_HH

namespace Texture
{
  typedef enum
    {
//       gutter,
//       gutterAlpha,

      wall1r,
      wall1g,
      wall1b,
      wall2r,
      wall2g,
      wall2b,
      wallHeight,
      wallSpecular,

      wall3r,
      wall3g,
      wall3b,
      wall3Height,
      wall3Specular,

      roof2r,
      roof2g,
      roof2b,
//       roof2Height,
//       roof2Specular,

      roof3r,
      roof3g,
      roof3b,
//       roof3Height,
//       roof3Specular,

      window1r,
      window1g,
      window1b,
      window2r,
      window2g,
      window2b,
      windowAlpha,
      windowHeight,
      windowSpecular,

      // Groupe de 9 fenêtres
      windowsConf1r,
      windowsConf1g,
      windowsConf1b,
      windowsConfHeight,
      windowsConfSpecular,
      windowsConfAlpha,

      pacWallr,
      pacWallg,
      pacWallb,
      pacWallHeight,
      pacWallSpecular,

      numberOfUrbanTextures
    } textureId;

  extern Texture::Channel *buffers;
  void buildUrban();
}

#endif
