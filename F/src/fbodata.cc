
#include "fbodata.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "fboid.hh"
#include "fbos.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"

namespace FBO
{
  static unsigned int currentFBOid = 0;

  Unit::Unit():
    _id(0),
    _width(0),
    _height(0)
  {
  }

  Unit::~Unit()
  {
  }

  void Unit::attachTexture(Texture::id texId, TextureUsage usage
			   DBGARG(char * description))
  {
    const GLenum params[] = {
      GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_COLOR_ATTACHMENT0_EXT,
      GL_RGBA,          GL_RGBA,          GL_DEPTH_COMPONENT, GL_RGB,
      GL_RGBA,          GL_RGBA,          GL_DEPTH_COMPONENT, GL_RGB32F_ARB,
      GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE, GL_UNSIGNED_BYTE,   GL_FLOAT,
    };
    const GLenum * attachment     = params;
    const GLenum * format         = params + numTextureUsages;
    const GLenum * internalFormat = params + 2 * numTextureUsages;
    const GLenum * dataType       = params + 3 * numTextureUsages;

    if (_id == 0)
      oglGenFramebuffersEXT(1, &_id);
    assert(_id != 0);

    float maxMaxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy);
    maxMaxAnisotropy = min(maxMaxAnisotropy, 8.f);

    assert(Texture::list[texId].id() == 0);
    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);
    Texture::list[texId].setRenderTexture(_width, _height, true, true,
					  format[usage], internalFormat[usage],
					  dataType[usage],
					  (usage == moments ? maxMaxAnisotropy : 1.f)
					  DBGARG(description));
    oglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment[usage],
			       GL_TEXTURE_2D, Texture::list[texId].id(), 0);
    _renderTex[(usage == moments ? color : usage)] = texId;

    unUse();

    OGL_FBO_ERROR_CHECK("Creation du FBO %d", texId);
  }

  //
  // void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
  //
  /*
  void Unit::attachCubeMapTexture(Texture::id texId)
  {
    if (_id == 0)
      oglGenFramebuffersEXT(1, &_id);
    assert(_id != 0);

    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);

    Texture::list[texId].setRenderCubeMapTexture(_width, _height);

    const GLuint id = Texture::list[texId].id();
    for (int i = 0; i < 6; ++i)
    {
      oglFramebufferTexture(GL_TEXTURE_CUBE_MAP,
			    GL_COLOR_ATTACHMENT0_EXT + i, id, i);
    }
    unUse();

    OGL_FBO_ERROR_CHECK("Creation du FBO %d", texId);
  }
  */

  void Unit::use() const
  {
    if (currentFBOid != _id)
    {
      oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);
      OGL_FBO_ERROR_CHECK("FBO::use() -> %d", _id);

      glViewport(0, 0, _width, _height);
      Shader::state.invResolution[0] = 1.f / (float)_width;
      Shader::state.invResolution[1] = 1.f / (float)_height;

      Texture::id textureId = _renderTex[0];
      if (textureId != none)
      {
	Texture::Unit & t = Texture::list[textureId];
	Shader::state.center[0] = 0.5f * (float)_width / t.width();
	Shader::state.center[1] = 0.5f * (float)_height / t.height();
      }

      currentFBOid = _id;
    }
  }

  void unUse()
  {
    currentFBOid = 0;
    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    glViewport(0, intro.yVP, intro.xres, intro.yresVP);
  }

  // Erreurs FBO possibles :
  //
  // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
  // All framebuffer attachment points are 'framebuffer attachment
  // complete'.
  //
  // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
  // There is at least one image attached to the framebuffer.
  //
  // GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
  // All attached images have the same width and height.
  //
  // GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
  // All images attached to the attachment points
  // COLOR_ATTACHMENT0_EXT through COLOR_ATTACHMENTn_EXT must have the
  // same internal format.
  //
  // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
  // The value of FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT must not be
  // NONE for any color attachment point(s) named by DRAW_BUFFERi.
  //
  // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
  // If READ_BUFFER is not NONE, then the value of
  // FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT must not be NONE for the
  // color attachment point named by READ_BUFFER.
  //
  // GL_FRAMEBUFFER_UNSUPPORTED_EXT:
  // The combination of internal formats of the attached images does
  // not violate an implementation-dependent set of restrictions.
  //
  // GL_FRAMEBUFFER_COMPLETE_EXT:
  // OK
}
