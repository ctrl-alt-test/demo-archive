//
// Données pour un FBO
//

#ifndef		FBODATA_HXX
# define	FBODATA_HXX

#include "fbodata.hh"

namespace FBO
{
  inline unsigned int Unit::id() const { return _id; }

  inline
  void Unit::setRenderSize(unsigned width, unsigned height)
  {
    _width = width;
    _height = height;
  }

  inline Texture::id Unit::renderTexture() const { return _renderTex[color]; }

  inline Texture::id Unit::renderTexture2() const { return _renderTex[color2]; }

  inline Texture::id Unit::depthTexture() const { return _renderTex[depth]; }

  inline Texture::id Unit::cocTexture() const { return _renderTex[coc]; }
}

#endif		// FBODATA_HXX
