//
// Texture unit
//

#ifndef		TEXTURE_UNIT_HXX
# define	TEXTURE_UNIT_HXX

#include "textureunit.hh"

namespace Texture
{
  inline GLuint Unit::id() const { return _id; }

  inline GLuint Unit::width() const { return _width; }

  inline GLuint Unit::height() const { return _height; }

  inline GLuint Unit::renderWidth() const { return _renderWidth; }

  inline GLuint Unit::renderHeight() const { return _renderHeight; }

  inline
  unsigned char Unit::operator [](unsigned int i) const
  {
    return _data[i];
  }

  inline const unsigned char * Unit::data() const { return _data; }

  inline
  void Unit::setFiltering(GLint min, GLint max, GLfloat maxAnisotropy)
  {
    _minFiltering = min;
    _maxFiltering = max;
    _maxAnisotropy = maxAnisotropy;
  }

  inline
  void Unit::setTiling(GLint horizontal, GLint vertical)
  {
    _horizontalTiling = horizontal;
    _verticalTiling = vertical;
  }

  inline void Unit::setFormat(GLenum format) { _format = format; }
}

#endif		// TEXTURE_UNIT_HXX
