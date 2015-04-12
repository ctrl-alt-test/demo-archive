//
// Données pour un VBO
//

#include "vbodata.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

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

  void Element::setupData(int size, void * data/*, int attribLocation*/) const
  {
    DBG("%d", size);
    if (currentVboId != _id)
    {
      currentVboId = _id;
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      OGL_ERROR_CHECK("VBO::setupData, glBindBuffer(%d)", _id);
    }
    oglBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    OGL_ERROR_CHECK("VBO::setupData, glBufferData(%d, ...)", size);

//     _setBuffers(attribLocation);
  }

  void Element::setupData(const Array <vertex> & chunk/*, int attribLocation*/) const
  {
    setupData(chunk.size * sizeof(vertex), chunk.elt/*, attribLocation*/);
  }

  void Element::use(int attribLocation) const
  {
    if (currentVboId != _id)
    {
      currentVboId = _id;
      OGL_ERROR_CHECK("???? VBO::use");
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      OGL_ERROR_CHECK("VBO::use, glBindBuffer(%d)", _id);
      _setBuffers(attribLocation); // Index pour l'attribut de sommet supplémentaire
    }
  }

  void Element::unUse()
  {
    currentVboId = 0;
    oglBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void Element::_setBuffers(int attribLocation) const
  {
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 0));
    OGL_ERROR_CHECK("VBO::_setBuffers, glVertexPointer()");

    glNormalPointer(GL_FLOAT, sizeof(vertex), (void *)(NULL + 3 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glNormalPointer()");

    if (attribLocation >= 0)
    {
      oglVertexAttribPointer(attribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + 6 * sizeof(float)));
      OGL_ERROR_CHECK("VBO::_setBuffers, glVertexAttribPointer(%d) (max : %d)", attribLocation, GL_MAX_VERTEX_ATTRIBS);
    }

    glColorPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 9 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glColorPointer()");

    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (void *)(NULL + 12 * sizeof(float)));
    OGL_ERROR_CHECK("VBO::_setBuffers, glTexCoordPointer()");
  }
}
