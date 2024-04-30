//
// Données pour un VBO
//

#include "vbodata.hh"

#include "mesh/vertex.hh"
#include "shaderid.hh"

#include "sys/msys_glext.h"

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

#if DEBUG
    description = "Sans titre";
#endif // DEBUG
  }

  Element::~Element()
  {
    oglDeleteBuffers(1, &_id);
  }

  void Element::setupData(int size, void * data) const
  {
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
			  DBGARG(char * description))
  {
    DBG("Creating a VBO of %8d elements / %8d bytes (%s)", chunk.size, chunk.size * sizeof(vertex), description);
    setupData(chunk.size * sizeof(vertex), chunk.elt);

    IFDBG(this->description = description);
    IFDBG(this->numberOfVertices = chunk.size);
  }

  void Element::use() const
  {
    if (currentVboId != _id)
    {
      currentVboId = _id;
      OGL_ERROR_CHECK("???? VBO::use");
      oglBindBuffer(GL_ARRAY_BUFFER, _id);
      OGL_ERROR_CHECK("VBO::use, glBindBuffer(%d)", _id);
      vertex::setAttributePointers(NULL);
    }
  }

  void Element::unUse()
  {
    currentVboId = 0;
    oglBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}
