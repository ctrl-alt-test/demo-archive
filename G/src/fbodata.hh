//
// Données pour un FBO
//

#ifndef		FBODATA_HH
# define	FBODATA_HH

#include "array.hh"
#include "basicTypes.hh"
#include "fboid.hh"
#include "textureid.hh"

namespace FBO
{
  enum TextureUsage
  {
    color = 0,
    color2,
    depth,
    moments,
    coc,
    compute,
    numTextureUsages
  };

  void unUse();

  class Unit
  {
  public:
    Unit();
    ~Unit();

    unsigned int id() const;
    void use(bool writeOnlyFirstBuffer = false) const;

    Texture::id renderTexture() const;
    Texture::id renderTexture2() const;
    Texture::id depthTexture() const;
    Texture::id cocTexture() const;

    void attachTexture(Texture::id texId, TextureUsage usage, GLenum clamping
		       DBGARG(char * description));

    // Les dimensions doivent etre inferieures ou egales aux dimensions des textures.
    void setRenderSize(unsigned width, unsigned height);

  private:
    GLuint _id;
    Texture::id _renderTex[numTextureUsages];
    unsigned int _width;
    unsigned int _height;

    Array<GLenum> _colorBuffers;
  };
}

#endif // FBODATA_HH
