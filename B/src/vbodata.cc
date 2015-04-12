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
    // FIXME : autre membres
  {
    oglGenBuffers(1, &_id);
    OGL_ERROR_CHECK("Génération d'id de VBO");
  }

  Element::~Element()
  {
    oglDeleteBuffers(1, &_id);
  }

  void Element::setupData(unsigned int size, void * data, int attribData) const
  {
    if (currentVboId != _id)
    {
      currentVboId = _id;
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
    }
    oglBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    _setBuffers(attribData);
  }

  void Element::setupData(const Array <vertex> & chunk, int attribData) const
  {
    setupData(chunk.size * sizeof(vertex), chunk.elt, attribData);
  }

  void Element::use(int attribData) const
  {
    if (currentVboId != _id)
    {
      currentVboId = _id;
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      _setBuffers(attribData); // Index pour l'attribut de sommet supplémentaire
    }
  }

  void Element::unUse()
  {
    currentVboId = 0;
    oglBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void Element::_setBuffers(int attribIndex) const
  {
    glVertexPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 0));
    glNormalPointer(GL_FLOAT, sizeof(vertex), (void *)(NULL + 3 * sizeof(float)));
    oglVertexAttribPointer(attribIndex, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + 6 * sizeof(float)));
    glColorPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + 9 * sizeof(float)));
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (void *)(NULL + 12 * sizeof(float)));
  }
}
