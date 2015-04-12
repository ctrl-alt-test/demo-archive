//
// Texture unit
//

#include "textureunit.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "sys/glext.h"

//
// void glGenTextures(GLsizei n,  GLuint * textures)
// void glBindTexture(GLenum target, GLuint texture)
// void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
//                   GLsizei width, GLsizei height, GLint border, GLenum format,
//                   GLenum type, const GLvoid * data)
// GLint gluBuild2DMipmaps(GLenum target, GLint internalFormat,
//                         GLsizei width, GLsizei height, GLenum format,
//                         GLenum type, const void * data)
// void glTexEnvf(GLenum target, GLenum pname, GLfloat param)
// void glTexEnvi(GLenum target, GLenum pname, GLint param)
// void glTexParameterf(GLenum target, GLenum pname, GLfloat param)
// void glTexParameteri( GLenum target, GLenum pname, GLint param)
// void glDeleteTextures(GLsizei n, const GLuint * textures)
//

namespace Texture
{
  static unsigned int currentTextureId = 0;
  static unsigned int currentTexture2Id = 0;
  static unsigned int currentBumpId = 0;
  static unsigned int currentSpecularId = 0;

  Unit::Unit():
    _id(0),
    _width(0),
    _height(0),
    _renderWidth(0),
    _renderHeight(0),
    _format(GL_RGBA),
    _minFiltering(GL_LINEAR),
    _maxFiltering(GL_LINEAR),
    _maxAnisotropy(1.f),
    _horizontalTiling(GL_REPEAT),
    _verticalTiling(GL_REPEAT),
    _data(NULL),
    _unit(GL_TEXTURE0)
  {
  }

  Unit::~Unit()
  {
    if (_id != 0)
    {
      glDeleteTextures(1, &_id);
    }
  }

  void Unit::use(int unit) const
  {
    switch (unit)
      {
      case 0:
        if (currentTextureId != _id)
	{
	  currentTextureId = _id;
	  oglActiveTextureARB(GL_TEXTURE0);
	  glBindTexture(GL_TEXTURE_2D, _id);
	}
        break;
      case 1:
        if (currentTexture2Id != _id)
	{
	  currentTexture2Id = _id;
	  oglActiveTextureARB(GL_TEXTURE1);
	  glBindTexture(GL_TEXTURE_2D, _id);
	}
        break;
      case 2:
        if (currentBumpId != _id)
	{
	  currentBumpId = _id;
	  oglActiveTextureARB(GL_TEXTURE2);
	  glBindTexture(GL_TEXTURE_2D, _id);
	}
        break;
      case 3:
        if (currentSpecularId != _id)
	{
	  currentSpecularId = _id;
	  oglActiveTextureARB(GL_TEXTURE3);
	  glBindTexture(GL_TEXTURE_2D, _id);
	}
        break;
      }
  }

  void Unit::unUse()
  {
    currentTextureId = 0;
    currentTexture2Id = 0;
    currentBumpId = 0;
    currentSpecularId = 0;

    oglActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    oglActiveTextureARB(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);

    oglActiveTextureARB(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Unit::setRenderTexture(unsigned int width, unsigned int height,
                              GLenum format, GLenum internalFormat)
  {
    assert(width != 0);
    assert(height != 0);

    DBG("Texture de rendu demandée : %dx%d", width, height);

    glGenTextures(1, &_id);
    OGL_ERROR_CHECK("Génération de l'id");
    use(0);
    _renderWidth = width;
    _renderHeight = height;
    _format = internalFormat;

    // Trouver la puissance de 2 superieure
    _width = 1;
    while (_width < _renderWidth)
      _width <<= 1;
    while (_width < _renderHeight)
      _width <<= 1;
    _height = _width;

    DBG("Texture de rendu obtenue : %dx%d", _width, _height);

    //
    // FIXME : ce n'est pas forcément 4
    //
    const unsigned int size = _width * _height * 4;
    _data = (unsigned char *)msys_mallocAlloc(size * sizeof(unsigned char));

    setFiltering(GL_LINEAR, GL_LINEAR);
    setTiling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    assert(_width > 0 && _height > 0);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format,
		   GL_UNSIGNED_BYTE, _data);
    OGL_ERROR_CHECK("Création d'une texture de rendu");
    _activateFiltering();
    _activateTiling();
  }

  void Unit::setData(unsigned int width, unsigned int height, GLenum format,
		     const unsigned char * data)
  {
    assert(width != 0);
    assert(height != 0);
    assert(data != NULL);
    assert(_data == NULL);

    glGenTextures(1, &_id);
    _width = width;
    _height = height;
    _format = format;
    _data = data;
  }

  void Unit::setData(const Channel & alpha)
  {
    setData(alpha.Width(), alpha.Height(), GL_ALPHA, AlphaBuffer(alpha));
  }

  void Unit::setData(const Channel & r,
		     const Channel & g,
		     const Channel & b)
  {
    setData(r.Width(), r.Height(), GL_RGB, RGBBuffer(r, g, b));
  }

  void Unit::setBumpData(const Channel & r,
			 const Channel & g,
			 const Channel & b)
  {
    setData(r.Width(), r.Height(), GL_RGB, NormalizedRGBBuffer(r, g, b));
  }

  void Unit::setData(const Channel & r,
		     const Channel & g,
		     const Channel & b,
		     const Channel & alpha)
  {
    setData(r.Width(), r.Height(), GL_RGBA, RGBABuffer(r, g, b, alpha));
  }

  void Unit::load(bool freeData)
  {
    assert(_id != 0);
    assert(_width != 0);
    assert(_height != 0);
    assert(_data != NULL);

    use(0);

    if (GL_NEAREST == _minFiltering || GL_LINEAR == _minFiltering)
    {
      // Sans mipmap
      glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format,
		   GL_UNSIGNED_BYTE, _data);
    }
    else
    {
      // Avec mipmap
      gluBuild2DMipmaps(GL_TEXTURE_2D, _format, _width, _height, _format,
			GL_UNSIGNED_BYTE, _data);
    }
    //
    // FIXME - Distinction format entrée de format de sortie ?
    //

    _activateFiltering();
    _activateTiling();

    if (freeData)
    {
      msys_mallocFree((void *)_data);
      _data = NULL; // FIXME : libération du buffer ?
    }
  }

  void Unit::_activateFiltering() const
  {
    assert(_maxAnisotropy >= 1.f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFiltering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _maxFiltering);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _maxAnisotropy);
  }

  void Unit::_activateTiling() const
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _horizontalTiling);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _verticalTiling);
  }

  //
  // Récupère le contenu du buffer dans la texture courante
  //
  // void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
  // void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
  //
  void Unit::renderToTexture() const
  {
    renderToTexture(0, 0, 0, 0, _renderWidth, _renderHeight);
  }

  void Unit::renderToTexture(GLint xoffset, GLint yoffset,
			     GLint x, GLint y,
			     GLsizei width,
			     GLsizei height) const
  {
    assert(_id != 0);

    use(0);
    assert(width - xoffset != 0 && height - yoffset != 0);

    glFlush();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, x, y, width, height);
  }

  //
  // Récupère le contenu de la texture courante (inverse de load)
  //
  // void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * img)
  //
  void Unit::dumpFromGL(unsigned char * data) const
  {
    assert(_id != 0);
    assert(_data != NULL);

    use(0);
    glGetTexImage(GL_TEXTURE_2D, 0, _format, GL_UNSIGNED_BYTE, data);
  }
}
