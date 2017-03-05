//
// Données pour un FBO
//

#ifndef		FBODATA_HH
# define	FBODATA_HH

# include "sys/msys.h"
# include <GL/GL.h>

# include "textureid.hh"
# include "fboid.hh"
# include "shaderid.hh"

namespace FBO
{
  enum TextureUsage
  {
    color = 0,
    color2,
    depth,
    moments,
    numTextureUsages
  };

  void unUse();

  class Unit
  {
  public:
    Unit();
    ~Unit();

    unsigned int id() const;
    void use() const;

    Texture::id renderTexture() const;
    Texture::id renderTexture2() const;
    Texture::id depthTexture() const;

    void attachTexture(Texture::id texId, TextureUsage usage
		       DBGARG(char * description));

    // Les dimensions doivent etre inferieures ou egales aux dimensions des textures.
    void setRenderSize(unsigned width, unsigned height);

  private:
    GLuint _id;
    Texture::id _renderTex[numTextureUsages];
    unsigned int _width;
    unsigned int _height;
  };
}


#include "fbodata.hxx"

#endif // FBODATA_HH
