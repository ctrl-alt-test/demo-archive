//
// Définition des types pour les meshs
//

#include "vertex.hh"

#include "shaderprogram.hh"

#include "sys/msys_glext.h"

#if DEBUG

void vertex::immediateDraw() const
{
#if VERTEX_ATTR_NORMAL
  glNormal3fv((float*)&n);
#endif

#if VERTEX_ATTR_TANGENT
  {
    int index = attributeLocation(vertex::tangent);
    if (index >= 0)
      oglVertexAttrib3f(index, t.x, t.y, t.z);
  }
#endif

#if VERTEX_ATTR_COLOR
  glColor3f(r, g, b);
#endif

#if VERTEX_ATTR_TEX_COORD
  glTexCoord4f(u, v, w, q);
#endif

#if VERTEX_ATTR_BARY_COORD
  {
    int index = attributeLocation(vertex::barycentricCoord);
    if (index >= 0)
      oglVertexAttrib4f(index, d0, d1, d2, d3);
  }
#endif

#if VERTEX_ATTR_ID
  {
    int index = attributeLocation(vertex::identifier);
    if (index >= 0)
      oglVertexAttrib1f(index, (float)id); // En attendant une fonction qui prend un int
  }
#endif

#if VERTEX_ATTR_POSITION
  glVertex3fv((float*)&p);
#endif
}

#endif // DEBUG


// Contournement d'un bug particulièrement pénible des pilotes NVidia
#define NVIDIA_ALIASING_WORKAROUND 3

int vertex::attributeLocation(vertex::attribute attribute)
{
  return attribute + NVIDIA_ALIASING_WORKAROUND;
}

void vertex::setAttributePointers(const void * pointer)
{
  int offset = 0;

#if VERTEX_ATTR_POSITION
  glVertexPointer(3, GL_FLOAT, sizeof(vertex), (void *)(NULL + offset));
  OGL_ERROR_CHECK("vertex::setAttributePointers(), vertex");
  offset += 3 * sizeof(float);
#endif
  
#if VERTEX_ATTR_NORMAL
  glNormalPointer(GL_FLOAT, sizeof(vertex), (void *)(NULL + offset));
  OGL_ERROR_CHECK("vertex::setAttributePointers(), normal");
  offset += 3 * sizeof(float);
#endif

#if VERTEX_ATTR_TANGENT
  {
    int index = attributeLocation(vertex::tangent);
    if (index >= 0)
      oglVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + offset));
    OGL_ERROR_CHECK("vertex::setAttributePointers(), tangent (attrib: %d)", index);
  }
  offset += 3 * sizeof(float);
#endif

#if VERTEX_ATTR_COLOR
  {
    int index = attributeLocation(vertex::color);
    if (index >= 0)
      oglVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + offset));
    OGL_ERROR_CHECK("vertex::setAttributePointers(), color");
  }
  offset += 3 * sizeof(float);
#endif

#if VERTEX_ATTR_TEX_COORD
  glTexCoordPointer(4, GL_FLOAT, sizeof(vertex), (void *)(NULL + offset));
  OGL_ERROR_CHECK("vertex::setAttributePointers(), texture coordinates");
  offset += 4 * sizeof(float);
#endif

#if VERTEX_ATTR_BARY_COORD
  {
    int index = attributeLocation(vertex::barycentricCoord);
    if (index >= 0)
      oglVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + offset));
    OGL_ERROR_CHECK("vertex::setAttributePointers(), barycentric coordinates (attrib: %d)", index);
  }
  offset += 4 * sizeof(float);
#endif

#if VERTEX_ATTR_ID
  {
    int index = attributeLocation(vertex::identifier);
    if (index >= 0)
      oglVertexAttribPointer(index, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(NULL + offset));
    OGL_ERROR_CHECK("vertex::setAttributePointers(), id (attrib: %d)", index);
  }
  offset += 1 * sizeof(int);
#endif
}

static void enableVertexAttribArray(vertex::attribute attribute)
{
  int index = vertex::attributeLocation(attribute);
  if (index >= 0)
    oglEnableVertexAttribArray(index);
}

static void disableVertexAttribArray(vertex::attribute attribute)
{
  int index = vertex::attributeLocation(attribute);
  if (index >= 0)
    oglDisableVertexAttribArray(index);
}

void vertex::activateRenderStates()
{
#if VERTEX_ATTR_POSITION
  glEnableClientState(GL_VERTEX_ARRAY);
#endif

#if VERTEX_ATTR_NORMAL
  glEnableClientState(GL_NORMAL_ARRAY);
#endif

#if VERTEX_ATTR_TANGENT
  enableVertexAttribArray(vertex::tangent);
#endif

#if VERTEX_ATTR_COLOR
  enableVertexAttribArray(vertex::color);
#endif

#if VERTEX_ATTR_TEX_COORD
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

#if VERTEX_ATTR_BARY_COORD
  enableVertexAttribArray(vertex::barycentricCoord);
#endif

#if VERTEX_ATTR_ID
  enableVertexAttribArray(vertex::identifier);
#endif
}

void vertex::deactivateRenderStates()
{
#if VERTEX_ATTR_POSITION
  glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if VERTEX_ATTR_NORMAL
  glDisableClientState(GL_NORMAL_ARRAY);
#endif

#if VERTEX_ATTR_TANGENT
  disableVertexAttribArray(vertex::tangent);
#endif

#if VERTEX_ATTR_COLOR
  disableVertexAttribArray(vertex::color);
#endif

#if VERTEX_ATTR_TEX_COORD
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

#if VERTEX_ATTR_BARY_COORD
  disableVertexAttribArray(vertex::barycentricCoord);
#endif

#if VERTEX_ATTR_ID
  disableVertexAttribArray(vertex::identifier);
#endif
}
