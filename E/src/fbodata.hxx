//
// Données pour un FBO
//

#ifndef		FBODATA_HXX
# define	FBODATA_HXX

namespace FBO
{
  inline
  unsigned int Unit::id() const
  {
    return _id;
  }

  inline
  void Unit::setRenderSize(unsigned width, unsigned height)
  {
    _width = width;
    _height = height;
  }

  inline
  Texture::id Unit::renderTexture() const
  {
    return _renderTex[color];
  }

  inline
  Texture::id Unit::velocityTexture() const
  {
    return _renderTex[velocity];
  }

  inline
  Texture::id Unit::depthTexture() const
  {
    return _renderTex[depth];
  }

}

#endif		// FBODATA_HXX
