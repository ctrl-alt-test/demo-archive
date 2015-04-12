//
// Définition des types pour les meshs
//

#include "mesh.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "vector.hh"

#if DEBUG

void immediateArrayDraw(const Array<vertex> & vertices, int vTanIndex)
{
  glBegin(GL_QUADS);
  for (int i = 0; i < vertices.size; ++i)
  {
    const vertex & v = vertices[i];
    glNormal3f(v.nx, v.ny, v.nz);
    oglVertexAttrib3f(vTanIndex, v.tx, v.ty, v.tz);
    glColor3f(v.r, v.g, v.b);
    glTexCoord4f(v.u, v.v, v.w, v.q);
    glVertex3f(v.x, v.y, v.z);
  }
  glEnd();
}

#endif // DEBUG

//
// Vertex transformé par une matrice
//
vertex transformedVertex(const float m[16], const vertex & v)
{
  vertex transformed = v;

  vector3f p = {v.x, v.y, v.z};
  vector3f n = {v.nx, v.ny, v.nz};
  vector3f t = {v.tx, v.ty, v.tz};

  computeMatrixPointProduct(m, p);
  computeMatrixVectorProduct(m, n);
  computeMatrixVectorProduct(m, t);

  transformed.x = p.x;
  transformed.y = p.y;
  transformed.z = p.z;
  transformed.nx = n.x;
  transformed.ny = n.y;
  transformed.nz = n.z;
  transformed.tx = t.x;
  transformed.ty = t.y;
  transformed.tz = t.z;

  return transformed;
}
