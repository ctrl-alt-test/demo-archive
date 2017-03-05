//
// Texture unit
//

#include "textureunit.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "sys/glext.h"

#include "interpolation.hh"
#include "matrix.hh"

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
  static unsigned int currentTextureIds[numberOfUsages];

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
    _data(NULL)
  {
#if DEBUG
    description = "Sans titre";
#endif // DEBUG
  }

  Unit::~Unit()
  {
    if (_id != 0)
    {
      glDeleteTextures(1, &_id);
    }
  }

  void Unit::use(usage u) const
  {
    if (currentTextureIds[u] != _id)
    {
      currentTextureIds[u] = _id;
      oglActiveTextureARB(GL_TEXTURE0 + u);
      glBindTexture(GL_TEXTURE_2D, _id);
    }
  }

  /*
  void Unit::useCubeMap(usage u) const
  {
    if (currentTextureIds[u] != _id)
    {
      currentTextureIds[u] = _id;
      oglActiveTextureARB(GL_TEXTURE0 + u);
      glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
    }
  }
  */

  void Unit::unUse()
  {
    // Sur ma machine ça dit que j'ai 8 unités de texture
    // Apparemment c'est un truc qui n'a pas changé pas depuis dix ans
    assert(GL_TEXTURE0 + numberOfUsages <= GL_TEXTURE8);

    for (unsigned int u = 0; u < numberOfUsages; ++u)
    {
      currentTextureIds[u] = 0;
      oglActiveTextureARB(GL_TEXTURE0 + u);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

  void Unit::setRenderTexture(unsigned int width, unsigned int height,
                              bool rectangular, bool nonPowerOfTwo,
                              GLenum format, GLenum internalFormat,
                              GLenum dataType,
                              float maxAnisotropy
                              DBGARG(char * description))
  {
    assert(width != 0);
    assert(height != 0);

    glGenTextures(1, &_id);
    OGL_ERROR_CHECK("Génération de l'id");
    use();
    _renderWidth = width;
    _renderHeight = height;
    _format = internalFormat;
    IFDBG(this->description = description);

    _width = width;
    _height = height;

    if (!nonPowerOfTwo)
    {
	// Trouver la puissance de 2 superieure
	_width = 1;
	while (_width < _renderWidth)
	    _width <<= 1;
	while (_width < _renderHeight)
	    _width <<= 1;
    }
    if (!rectangular)
    {
	_width = max(_width, _height);
	_height = _width;
    }

    DBG("Texture de rendu souhaitée : %dx%d : obtenue : %dx%d (%s)",
	_renderWidth, _renderHeight, _width, _height, description);

    assert(_width > 0 && _height > 0);

    setFiltering(GL_LINEAR, GL_LINEAR, maxAnisotropy);
    setTiling(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    _data = NULL;
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, format,
		 dataType, _data);
    OGL_ERROR_CHECK("Création d'une texture de rendu");
    _activateFiltering();
    _activateTiling();
  }

  /*
  void Unit::setRenderCubeMapTexture(unsigned int width, unsigned int height)
  {
    glGenTextures(1, &_id);
    OGL_ERROR_CHECK("Génération de l'id");
    useCubeMap();

    _renderWidth = width;
    _renderHeight = height;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Define all 6 faces
    for (int i = 0; i < 6; ++i)
    {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8,
		   _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  */

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

  static inline unsigned char floatToByte(float value)
  {
    return (unsigned char)msys_ifloorf(clamp(value) * 255.f);
  }

  void Unit::setData(const Channel & alpha, bool compressed)
  {
    const unsigned int width = alpha.Width();
    const unsigned int height = alpha.Height();
    unsigned char* data = (unsigned char*)msys_mallocAlloc(width * height * sizeof(unsigned char));

    // Y axis upside down for OpenGL way of handling texture coordinates
    for (unsigned int j = 0, k = height - 1; j < height; ++j, k--)
      for (unsigned int i = 0; i < width; ++i)
      {
	data[width * k + i] = floatToByte(alpha[width * j + i]);
      }

    setData(width, height, compressed ? GL_COMPRESSED_ALPHA : GL_ALPHA, data);
  }

  void Unit::setData(const Channel & r,
		     const Channel & g,
		     const Channel & b,
		     bool compressed)
  {
    assert(r.Width() == g.Width() && r.Height() == g.Height() &&
	   g.Width() == b.Width() && g.Height() == b.Height());

    const unsigned int width = r.Width();
    const unsigned int height = r.Height();
    unsigned char* data = (unsigned char*)msys_mallocAlloc(3 * width * height * sizeof(unsigned char));

    // Y axis upside down for OpenGL way of handling texture coordinates
    for (unsigned int j = 0, k = height - 1; j < height; ++j, k--)
      for (unsigned int i = 0; i < width; ++i)
      {
	const unsigned int aIndex = width * j + i;
 	const unsigned int dIndex = 3 * (width * k + i);
	data[dIndex] = floatToByte(r[aIndex]);
	data[dIndex + 1] = floatToByte(g[aIndex]);
	data[dIndex + 2] = floatToByte(b[aIndex]);
      }

    setData(width, height, compressed ? GL_COMPRESSED_RGB : GL_RGB, data);
  }

  void Unit::setData(const Channel & r,
		     const Channel & g,
		     const Channel & b,
		     const Channel & a,
		     bool compressed)
  {
    assert(r.Width() == g.Width() && r.Height() == g.Height() &&
	   g.Width() == b.Width() && g.Height() == b.Height() &&
	   b.Width() == a.Width() && b.Height() == a.Height());

    const unsigned int width = a.Width();
    const unsigned int height = a.Height();
    unsigned char* data = (unsigned char*)msys_mallocAlloc(4 * width * height * sizeof(unsigned char));

    for (unsigned int j = 0, k = height - 1; j < height; ++j, k--)
      for (unsigned int i = 0; i < width; ++i)
      {
	const unsigned int aIndex = width * j + i;
 	const unsigned int dIndex = 4* (width * k + i);
	data[dIndex] = floatToByte(r[aIndex]);
	data[dIndex + 1] = floatToByte(g[aIndex]);
	data[dIndex + 2] = floatToByte(b[aIndex]);
	data[dIndex + 3] = floatToByte(a[aIndex]);
      }

    setData(width, height, compressed ? GL_COMPRESSED_RGBA : GL_RGBA, data);
  }

  void Unit::load(bool freeData)
  {
    assert(_id != 0);
    assert(_width != 0);
    assert(_height != 0);
    assert(_data != NULL);

    use();

    GLenum format;
    switch (_format)
    {
    case GL_COMPRESSED_RGB:
      format = GL_RGB;
      break;
    case GL_COMPRESSED_RGBA:
      format = GL_RGBA;
      break;
    case GL_COMPRESSED_ALPHA:
      format = GL_ALPHA;
      break;
    default:
      format = _format;
    }

    if (GL_NEAREST == _minFiltering || GL_LINEAR == _minFiltering)
    {
      // Sans mipmap
      glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, format,
		   GL_UNSIGNED_BYTE, _data);
      OGL_ERROR_CHECK("Texture::Unit , glTexImage2D(%dx%d)", _width, _height);
    }
    else
    {
      // Avec mipmap
      gluBuild2DMipmaps(GL_TEXTURE_2D, _format, _width, _height, format,
			GL_UNSIGNED_BYTE, _data);
      OGL_ERROR_CHECK("Texture::Unit , gluBuild2DMipmaps(%dx%d)", _width, _height);
    }

#if DEBUG

    // Cas des textures compressées
    if (GL_COMPRESSED_ALPHA == _format ||
	GL_COMPRESSED_LUMINANCE == _format ||
	GL_COMPRESSED_LUMINANCE_ALPHA == _format ||
	GL_COMPRESSED_INTENSITY == _format ||
	GL_COMPRESSED_RGB == _format ||
	GL_COMPRESSED_RGBA == _format)
    {
      int width = 0;
      int height = 0;
      int depth = 0;
      int iFormat = 0;
      int border = 0;
      int redSize = 0;
      int greenSize = 0;
      int blueSize = 0;
      int alphaSize = 0;
      int luminanceSize = 0;
      int intensitySize = 0;
      int depthSize = 0;
      int compressed = 0;
      int compressedSize = 0;
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, width/height");
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &depth);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &iFormat);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BORDER, &border);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, depth/iFormat/border");
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, &redSize);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &greenSize);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &blueSize);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alphaSize);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, red/green/blue/alphaSize");
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_LUMINANCE_SIZE, &luminanceSize);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTENSITY_SIZE, &intensitySize);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, luminance/intensitySize");
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, &depthSize);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, depthSize");
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
      OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, compressed");
	  if (compressed != 0)
	  {
	      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressedSize);
		  OGL_ERROR_CHECK("Texture::Unit, compressed, glGetTexLevelParameteriv, compressedSize");
	  }
	  else
	  {
		  DBG("Texture non compressée !");
	  }

      DBG("Texture chargée : %dx%d, depth %d, border %d, red %d, green %d, blue %d, alpha %d, luminance %d, intensity %d, depth %d, %s, size %d",
	  width, height, depth, border,
	  redSize, greenSize, blueSize, alphaSize, luminanceSize, intensitySize, depthSize,
	  (compressedSize == GL_FALSE ? "non compressée" : "compressée"), compressedSize);
    }

#endif // DEBUG

    //
    // FIXME - Distinction format entrée de format de sortie ?
    //

    _activateFiltering();
    _activateTiling();

    if (freeData)
    {
      msys_mallocFree((void *)_data);
    }
    _data = NULL; // FIXME : libération du buffer ?
  }

  void Unit::_activateFiltering() const
  {
    assert(_maxAnisotropy >= 1.f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFiltering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _maxFiltering);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, _maxAnisotropy);
    OGL_ERROR_CHECK("Texture::Unit , _activateFiltering");
  }

  void Unit::_activateTiling() const
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _horizontalTiling);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _verticalTiling);
    OGL_ERROR_CHECK("Texture::Unit , _activateTiling");
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
			     GLsizei width, GLsizei height) const
  {
    assert(_id != 0);
    assert(width > 0 && height > 0);
    assert(width + xoffset <= (int)_width && height + yoffset <= (int)_height);

    use();

    glFlush();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
			xoffset, yoffset, x, y, width, height);
  }

  //
  // Récupère le contenu de la texture courante (inverse de load)
  //
  // void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * img)
  //
  void Unit::dumpFromGL(unsigned char ** r,
			unsigned char ** g,
			unsigned char ** b,
			unsigned char ** a) const
  {
    assert(_id != 0);

    unsigned char * data = (unsigned char *)msys_mallocAlloc(4 * _width * _height * sizeof(unsigned char));
    *r = (unsigned char *)msys_mallocAlloc(_width * _height * sizeof(unsigned char));
    *g = (unsigned char *)msys_mallocAlloc(_width * _height * sizeof(unsigned char));
    *b = (unsigned char *)msys_mallocAlloc(_width * _height * sizeof(unsigned char));
    *a = (unsigned char *)msys_mallocAlloc(_width * _height * sizeof(unsigned char));

    use();
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    for (unsigned int i = 0; i < _width * _height; ++i)
      (*r)[i] = data[4 * i];

    for (unsigned int i = 0; i < _width * _height; ++i)
      (*g)[i] = data[4 * i + 1];

    for (unsigned int i = 0; i < _width * _height; ++i)
      (*b)[i] = data[4 * i + 2];

    for (unsigned int i = 0; i < _width * _height; ++i)
      (*a)[i] = data[4 * i + 3];

    msys_mallocFree(data);
  }
}
