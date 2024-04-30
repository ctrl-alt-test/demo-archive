
#include "fbodata.hh"

#include "fboid.hh"
#include "fbos.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"
#include "textureunit.hxx"

#include "sys/msys_debug.h"
#include "sys/msys_glext.h"

namespace FBO
{
  static unsigned int currentFBOid = 0;

  Unit::Unit():
    _id(0),
    _width(0),
    _height(0),
    _colorBuffers(3) // Quand on aura plus de 3 MRT on en reparle
  {
  }

  Unit::~Unit()
  {
  }

  void Unit::attachTexture(Texture::id texId, TextureUsage usage, GLenum clamping
			   DBGARG(char * description))
  {
    // FIXME: rassembler tout ça dans une classe ou struct TextureUsage
    const GLenum params[] = {
      GL_COLOR_ATTACHMENT0_EXT,        GL_COLOR_ATTACHMENT1_EXT,        GL_DEPTH_ATTACHMENT_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT0_EXT,
      GL_RGB,                          GL_RGB,                          GL_DEPTH_COMPONENT,      GL_RGB,                   GL_RG,                    GL_RGBA,
      GL_R11F_G11F_B10F,               GL_R11F_G11F_B10F,               GL_DEPTH_COMPONENT,      GL_RGB32F_ARB,            GL_RG16F,                 GL_RGBA32F,
      GL_UNSIGNED_INT_10F_11F_11F_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_UNSIGNED_BYTE,        GL_FLOAT,                 GL_FLOAT,                 GL_FLOAT,
    };
    const GLenum * attachments     = params;
    const GLenum * formats         = params + numTextureUsages;
    const GLenum * internalFormats = params + 2 * numTextureUsages;
    const GLenum * dataTypes       = params + 3 * numTextureUsages;

    if (_id == 0)
      oglGenFramebuffers(1, &_id);
    assert(_id != 0);

    float maxMaxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxMaxAnisotropy);
    maxMaxAnisotropy = msys_min(maxMaxAnisotropy, 8.f);

    assert(Texture::list[texId].id() == 0);
    oglBindFramebuffer(GL_FRAMEBUFFER_EXT, _id);
    Texture::list[texId].setRenderTexture(_width, _height, true, true,
					  formats[usage], internalFormats[usage],
					  dataTypes[usage],
					  clamping, (usage == moments ? maxMaxAnisotropy : 1.f)
					  DBGARG(description));
    oglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, attachments[usage],
			       GL_TEXTURE_2D, Texture::list[texId].id(), 0);
    _renderTex[(usage == moments || usage == compute ? color : usage)] = texId;

    if (internalFormats[usage] != GL_DEPTH_COMPONENT
	// OpenGL >= 3
#ifdef GL_DEPTH_STENCIL
	&& internalFormats[usage] != GL_DEPTH_STENCIL
#endif
	)
      _colorBuffers.add(attachments[usage]);
    oglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
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
  }
  */

  void Unit::use(bool writeOnlyFirstBuffer) const
  {
    if (currentFBOid != _id)
    {
      oglBindFramebuffer(GL_FRAMEBUFFER_EXT, _id);
      OGL_FBO_ERROR_CHECK("FBO::use");

      glViewport(0, 0, _width, _height);
      Shader::uniforms[Shader::resolution].set(vector2f((float)_width, (float)_height));
      Shader::uniforms[Shader::invResolution].set(vector2f(1.f / (float)_width, 1.f / (float)_height));

      Texture::id textureId = _renderTex[0];
      if (textureId != none)
      {
	Texture::Unit & t = Texture::list[textureId];
	Shader::uniforms[Shader::center].set(0.5f * vector2f((float)_width / t.width(), (float)_height / t.height()));
      }

      oglDrawBuffers(writeOnlyFirstBuffer ? 1 : _colorBuffers.size, _colorBuffers.elt);
      OGL_FBO_ERROR_CHECK("FBO::use - drawbuffers");

      currentFBOid = _id;
    }
  }

  void unUse()
  {
    currentFBOid = 0;
    oglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
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
