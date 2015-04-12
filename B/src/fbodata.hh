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
  typedef enum
  {
    color = 0,
    depth,
    numTextureUsages
  } TextureUsage;

  void unUse();

  class Unit
  {
  public:
    Unit();
    ~Unit();

    unsigned int id() const;
    void setListId(FBO::id listId);
    void use() const;

    Texture::id renderTexture() const;
    Texture::id depthTexture() const;

    void attachTexture(Texture::id texId, TextureUsage usage);

    // Les dimensions doivent etre inferieures ou egales aux dimensions des textures.
    void setRenderSize(unsigned width, unsigned height);

  private:
    GLuint _id;
    FBO::id _listId;
    Texture::id _renderTex[numTextureUsages];
    unsigned _width;
    unsigned _height;
  };
}


#include "fbodata.hxx"

#endif // FBODATA_HH
