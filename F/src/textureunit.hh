//
// Texture unit
//

#ifndef		TEXTURE_UNIT_HH
# define	TEXTURE_UNIT_HH

#include "sys/msys.h"
#include <GL/gl.h>

#include "texgen/texture.hh"

namespace Texture
{
  enum usage
    {
      // Nommage pour le forward rendering
      albedo   = 0,
      normal   = 1,
      material = 2,
      albedo2  = 3,
      shadow   = 4,
//       ambient  = 5,

      // Nommage pour le post processing
      color    = 0,
      depth    = 1,
      glow     = 2,
      velocity = 3,
      grain    = 4,

      numberOfUsages // Ne pas dépasser 8 (question de hardware)
    };

  class Unit
  {
  public:

    Unit();
    ~Unit();

    unsigned int id() const;
    void use(usage u = albedo) const;

    static void unUse();

    GLuint width() const;
    GLuint height() const;
    GLuint renderWidth() const;
    GLuint renderHeight() const;
    unsigned char operator [] (unsigned int i) const;
    const unsigned char * data() const;

    void setRenderTexture(unsigned int width, unsigned int height,
						  bool rectangular, bool nonPowerOfTwo,
			  GLenum format, GLenum internalFormat, GLenum dataType,
			  float maxAnisotropy
			  DBGARG(char * description));
    void setData(unsigned int width, unsigned int height, GLenum format,
		 const unsigned char * data);

    void setData(const Channel & alpha, bool compressed);
    void setData(const Channel & r, const Channel & g, const Channel & b,
		 bool compressed);
    void setData(const Channel & r, const Channel & g, const Channel & b,
		 const Channel & a, bool compressed);

    void setData4(const Channel & r1, const Channel & g1, const Channel & b1,
		  const Channel & r2, const Channel & g2, const Channel & b2,
		  const Channel & r3, const Channel & g3, const Channel & b3,
		  const Channel & r4, const Channel & g4, const Channel & b4);



    void setFiltering(GLint min, GLint max, GLfloat maxAnisotropy = 1.f);
    void setTiling(GLint horizontal, GLint vertical);

    void setFormat(GLenum format);

    void load(bool freeData = true);
    void renderToTexture() const;
    void renderToTexture(GLint xoffset, GLint yoffset,
			 GLint x, GLint y,
			 GLsizei width,
			 GLsizei height) const;
    void dumpFromGL(unsigned char ** r,
		    unsigned char ** g,
		    unsigned char ** b,
		    unsigned char ** a) const;

  private:
    void _activateFiltering() const;
    void _activateTiling() const;

    GLuint	_id;

    GLuint	_width;
    GLuint	_height;
//     GLenum	_internalFormat;
    GLenum	_format;

    // Taille de rendu pour le render to texture
    // (width, height) etant dans ce cas a la puissance de 2 superieure
    GLuint	_renderWidth;
    GLuint	_renderHeight;

    GLint	_minFiltering;
    GLint	_maxFiltering;
    GLfloat	_maxAnisotropy;
    GLint	_horizontalTiling;
    GLint	_verticalTiling;

    const unsigned char * _data;

#if DEBUG
  public:
    const char * description;
#endif // DEBUG

  };
}


#include "textureunit.hxx"

#endif		// TEXTURE_UNIT_HH
