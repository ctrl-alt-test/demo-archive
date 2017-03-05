//
// Données pour un VBO
//

#include "vbodata.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "shaderid.hh"

//
// glGenBuffersARB(GLsizei n, GLuint* ids)
// glBindBufferARB(GLenum target, GLuint id)
// glBufferDataARB(GLenum target, GLsizei size, const void* data, GLenum usage)
//
// FIXME : usage des pointeurs
//

namespace VBO
{
  static unsigned int currentVboId = 0;

  Element::Element():
    _id(0)
    // FIXME : autres membres
  {
    oglGenBuffers(1, &_id);
    OGL_ERROR_CHECK("Génération d'id de VBO");
  }

  Element::~Element()
  {
    oglDeleteBuffers(1, &_id);
  }

  void Element::setupData(int size, void * data) const
  {
    DBG("%d bytes", size);
    if (currentVboId != _id)
    {
      currentVboId = _id;
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      OGL_ERROR_CHECK("VBO::setupData, glBindBuffer(%d)", _id);
    }
    oglBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    OGL_ERROR_CHECK("VBO::setupData, glBufferData(%d, ...)", size);
  }

  void Element::setupData(const Array <vertex> & chunk
			  DBGARG(char * description)) const
  {
    DBG("Creating a VBO of %d elements (%s)", chunk.size, description);
    setupData(chunk.size * sizeof(vertex), chunk.elt);
  }

  static void _setBuffers()
  {
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 0));
    OGL_ERROR_CHECK("VBO::_setBuffers, glVertexPointer()");

    glNormalPointer(GL_FLOAT, sizeof(vertex), (void *)(NULL + 3 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glNormalPointer()");

    oglVertexAttribPointer(Shader::vTan, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + 6 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glVertexAttribPointer(%d) (max : %d)", Shader::vTan, GL_MAX_VERTEX_ATTRIBS);

//     glColorPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 9 * sizeof(float)));
//     OGL_ERROR_CHECK("VBO::_setBuffers, glColorPointer()");

    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (void *)(NULL + 9 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glTexCoordPointer()");
  }

  void Element::use() const
  {
    if (currentVboId != _id)
    {
      currentVboId = _id;
      OGL_ERROR_CHECK("???? VBO::use");
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      OGL_ERROR_CHECK("VBO::use, glBindBuffer(%d)", _id);
      _setBuffers();
    }
  }

  void Element::unUse()
  {
    currentVboId = 0;
    oglBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}
