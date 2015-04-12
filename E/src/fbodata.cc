
#include "fbodata.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "fboid.hh"
#include "fbos.hh"
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

  void Unit::attachTexture(Texture::id texId, TextureUsage usage)
  {
    const GLenum attachment[numTextureUsages] =
    {
      GL_COLOR_ATTACHMENT0_EXT,
      GL_COLOR_ATTACHMENT1_EXT,
      GL_DEPTH_ATTACHMENT_EXT,
    };
    const GLenum format[numTextureUsages] =
    {
      GL_RGBA,
      GL_RGBA,
      GL_DEPTH_COMPONENT,
    };
    const GLenum internalFormat[numTextureUsages] =
    {
      GL_RGBA,
      GL_RGBA,
      GL_DEPTH_COMPONENT24,
    };
    if (_id == 0)
      oglGenFramebuffersEXT(1, &_id);
    assert(_id != 0);

    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);
    Texture::list[texId].setRenderTexture(_width, _height, format[usage], internalFormat[usage]);
    oglFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment[usage],
                               GL_TEXTURE_2D, Texture::list[texId].id(), 0);

    _renderTex[usage] = texId;
    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  }

  void Unit::use() const
  {
    if (currentFBOid != _id)
    {
      oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _id);
      OGL_FBO_ERROR_CHECK("FBO::use() -> %d", _id);

      glViewport(0, 0, _width, _height);
      currentFBOid = _id;
    }
  }

  void unUse()
  {
    currentFBOid = 0;
    oglBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
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
