//
// Définition des types pour les meshs
//

#include "mesh.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "cube.hh"
#include "revolution.hh"
#include "revolutionfunctions.hh"
#include "textures.hh"
#include "tweakval.h"
#include "vector.hh"

#if DEBUG

void immediateArrayDraw(const Array<vertex> & vertices, int vTanIndex)
{
  glBegin(GL_QUADS);
  for (int i = 0; i < vertices.size; ++i)
  {
    const vertex & v = vertices[i];
    glNormal3fv((float*)&(v.n));
    oglVertexAttrib3f(vTanIndex, v.t.x, v.t.y, v.t.z);
//     glColor3f(v.r, v.g, v.b);
    glTexCoord4f(v.u, v.v, v.w, v.q);
    glVertex3fv((float*)&(v.p));
  }
  glEnd();
}

void immediateMeshDraw(const mesh & m, int vTanIndex)
{
  glBegin(GL_QUADS);
  for (int i = 0; i < m.indices.size; ++i)
  {
    const vertex & v = m.vertices[m.indices[i]];
    glNormal3fv((float*)&(v.n));
    //oglVertexAttrib3f(vTanIndex, v.t.x, v.t.y, v.t.z);
//     glColor3f(v.r, v.g, v.b);
    glTexCoord4f(v.u, v.v, v.w, v.q);
    glVertex3fv((float*)&(v.p));
  }
  glEnd();
}

#endif // DEBUG

namespace Mesh
{
  Array<vertex> pool[];
  const int poolSize = sizeof(pool) / sizeof(pool[0]);
  int mid = 0;

  void initPool()
  {
    for (int i = 0; i < poolSize; ++i)
      pool[i].init(16384);
    mid = 0;
  }

  void clearPool()
  {
    mid = 0;
  }

  Array<vertex> & getTemp()
  {
    assert(mid < poolSize);
    pool[mid].empty();
    return pool[mid++];
  }

  void destroyPool()
  {
    for (int i = 0; i < poolSize; ++i)
    {
      msys_mallocFree(pool[i].elt);
      IFDBG(pool[i].max_size = 0;)
      IFDBG(pool[i].elt = 0;)
    }
    mid = 0;
  }

  //
  // Vertex transformé par une matrice
  //
  vertex transformedVertex(const matrix4 & transform, const vertex & v)
  {
    vertex transformed = v;

    applyMatrixToPoint(transform, transformed.p);
    applyMatrixToVector(transform, transformed.n);
    applyMatrixToVector(transform, transformed.t);

    return transformed;
  }

  //
  // Ajout à un tas de sommets d'un autre tas de sommets transformé
  // par une matrice
  //
  void addVertices(Array<vertex> & vertices,
		   const Array<vertex> & moreVertices,
		   const matrix4 & transform)
  {
    IFDBG(assert(vertices.max_size >= vertices.size + moreVertices.size));
    int size = moreVertices.size;
    for (int i = 0; i < size; ++i)
    {
      vertices.add(transformedVertex(transform, moreVertices[i]));
    }
  }

  //
  // Ajout à un tas de sommets d'un autre tas de sommets transformé
  // par la matrice MODELVIEW
  //
  void addVerticesUsingCurrentProjection(Array<vertex> & vertices,
					 const Array<vertex> & moreVertices)
  {
    matrix4 transform;
    getFromModelView(transform);
    addVertices(vertices, moreVertices, transform);
  }

  //
  // Applique la matrice courante à un mesh
  //
  void applyCurrentProjection(Array<vertex> & vertices)
  {
    matrix4 transform;
    getFromModelView(transform);
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i] = transformedVertex(transform, vertices[i]);
    }
  }

  void generatePave(Array<vertex> & dest, float x, float y, float z)
  {
    IFDBG(assert(dest.max_size >= dest.size + int(Cube::numberOfVertices));)

    for (unsigned int i = 0; i < Cube::numberOfVertices; i++)
    {
      vertex v = Cube::vertices[i];
      v.p.x *= x;
      v.p.y *= y;
      v.p.z *= z;
      dest.add(v);
    }
  }

  static vertex interpolate3(const vertex & v1, const vertex & v2, const vertex & v3, float x, float y)
  {
    assert(x + y <= 1.f); // sinon, ça sort du triangle (v1, v2, v3)

    vertex v = v1;
    // FIXME: couleurs et normales
    v.p = v1.p + (v2.p - v1.p) * x + (v3.p - v1.p) * y;
    v.u = v1.u + (v2.u - v1.u) * x + (v3.u - v1.u) * y;
    v.v = v1.v + (v2.v - v1.v) * x + (v3.v - v1.v) * y;
    v.w = v1.w + (v2.w - v1.w) * x + (v3.w - v1.w) * y;
    v.q = v1.q + (v2.q - v1.q) * x + (v3.q - v1.q) * y;
    return v;
  }

  static vertex interpolate4(const vertex* vertices, float x, float y)
  {
    if (x + y < 1.f)
      return interpolate3(vertices[0], vertices[1], vertices[3], x, y);
    else
      return interpolate3(vertices[2], vertices[1], vertices[3], 1.f - y, 1.f - x);
  }

  void removeFace(Array<vertex> & mesh, int index)
  {
    assert(index % 4 == 0);
    for (int i = 3; i >= 0; i--)
    {
      mesh.remove(index + i);
    }
  }

  // Pivoter les tangentes de 90°
  void rotateTangents(Array<vertex> & mesh)
  {
    for (int i = 0; i < mesh.size; ++i)
    {
      mesh[i].t = mesh[i].n ^ mesh[i].t;
    }
  }

  // Split a face in x*y faces
  void splitFace(Array<vertex> & mesh, const vertex* vertices, int x, int y)
  {
    assert(x > 0 && y > 0);
    for (int i = 0; i < x; i++)
    {
      for (int j = 0; j < y; j++)
      {
        mesh.add(interpolate4(vertices, (float) i / x,     (float) j / y));
        mesh.add(interpolate4(vertices, (float) (i+1) / x, (float) j / y));
        mesh.add(interpolate4(vertices, (float) (i+1) / x, (float) (j+1) / y));
        mesh.add(interpolate4(vertices, (float) i / x,     (float) (j+1) / y));
      }
    }
  }

  void splitAllFaces(Array<vertex> & mesh, int x)
  {
    int size = mesh.size;
    for (int i = 0; i < size; i += 4)
    {
      splitFace(mesh, &mesh[i], x, x);
      removeFace(mesh, i);
    }
  }

  void addNoise(Array<vertex> & mesh, float coef)
  {
    for (int i = 0; i < mesh.size; i++)
    {
      float seed = 10.f * norm(mesh[i].p) + 2.f * mesh[i].p.x + mesh[i].p.y;
      msys_srand((int)(100.f * seed));
      mesh[i].p += vector3f(msys_sfrand(), msys_sfrand(), msys_sfrand()) * coef;
      // FIXME: normales
    }
  }

  // ========================================================================
  // Coordonnées de texture

  void reprojectTexture(Array<vertex> & mesh, vector3f U, vector3f V)
  {
    for (int i = 0; i < mesh.size; ++i)
    {
      mesh[i].u = U * mesh[i].p;
      mesh[i].v = V * mesh[i].p;
    }
  }

  void reprojectTextureXZPlane(Array<vertex> & mesh, float scale)
  {
    const vector3f u(scale, 0, 0);
    const vector3f v(0, 0, scale);
    reprojectTexture(mesh, u, v);
  }

  void reprojectTextureXYPlane(Array<vertex> & mesh, float scale)
  {
    const vector3f u(scale, 0, 0);
    const vector3f v(0, scale, 0);
    reprojectTexture(mesh, u, v);
  }

  void reprojectTextureZYPlane(Array<vertex> & mesh, float scale)
  {
    const vector3f u(0, 0, scale);
    const vector3f v(0, scale, 0);
    reprojectTexture(mesh, u, v);
  }

  void addTexCoordNoise(Array<vertex> & mesh, float coef)
  {
    for (int i = 0; i < mesh.size; ++i)
    {
      float seed = 10.f * norm(mesh[i].p) + 2.f * mesh[i].p.x + mesh[i].p.y;
      msys_srand((int)(100.f * seed));
      mesh[i].u += msys_sfrand() * coef;
      mesh[i].v += msys_sfrand() * coef;
      mesh[i].w += msys_sfrand() * coef;
      mesh[i].q += msys_sfrand() * coef;
    }
  }

  void scaleTexture(Array<vertex> & mesh, float uFactor, float vFactor)
  {
    for (int i = 0; i < mesh.size; ++i)
    {
      mesh[i].u *= uFactor;
      mesh[i].v *= vFactor;
    }
  }

  void translateTexture(Array<vertex> & mesh, float uOffset, float vOffset)
  {
    for (int i = 0; i < mesh.size; ++i)
    {
      mesh[i].u += uOffset;
      mesh[i].v += vOffset;
    }
  }

  // =========================================================================


  static vector3f vectorFromHeightMap(const Texture::Channel & tex, float x, float z)
  {
    return vector3f(x, tex.Bilinear(tex.Width() * x, tex.Height() * z), z);
  }

#define V3(x, y, z) vector3f(x, y, z)
  static vertex rectangle[] =
  {
    {V3(0, 0, 0), V3(0, 0, 0),  V3(1.f, 0.f, 0.f),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},
    {V3(1, 0, 0), V3(0, 0, 0),  V3(1.f, 0.f, 0.f),  /* 1.f, 1.f, 1.f, */  1, 0, 0, 1.f},
    {V3(1, 0, 1), V3(0, 0, 0),  V3(1.f, 0.f, 0.f),  /* 1.f, 1.f, 1.f, */  1, 1, 0, 1.f},
    {V3(0, 0, 1), V3(0, 0, 0),  V3(1.f, 0.f, 0.f),  /* 1.f, 1.f, 1.f, */  0, 1, 0, 1.f},
  };

  void generateFromHeightMap(Array<vertex> & mesh, const Texture::Channel & tex, int resx, int resy)
  {
    assert(mesh.size == 0);

    const vertex * source = rectangle;
    splitFace(mesh, source, resx, resy);

    for (int i = 0; i < mesh.size; i++)
    {
      vertex & v = mesh[i];
      v.p = vectorFromHeightMap(tex, v.p.x, v.p.z);
    }

    // Suppression des faces où y=0
    for (int i = 0; i < mesh.size; i += 4)
    {
      float maxy = max(max(mesh[i].p.y, mesh[i+1].p.y),
                       max(mesh[i+2].p.y, mesh[i+3].p.y));
      if (maxy <= 0.f) removeFace(mesh, i);
    }

    // Calcul des normales
    const float incx = 1.f / resx;
    const float incz = 1.f / resy;
    for (int i = 0; i < mesh.size; i++)
    {
      vertex & v = mesh[i];
      vector3f A = vectorFromHeightMap(tex, v.p.x + incx, v.p.z);
      vector3f B = vectorFromHeightMap(tex, v.p.x, v.p.z + incz);
      vector3f C = vectorFromHeightMap(tex, v.p.x - incx, v.p.z);
      vector3f D = vectorFromHeightMap(tex, v.p.x, v.p.z - incz);
      v.n = computeNormal(v.p, A, B, C, D);
      // FIXME: tangente
    }

    // Symmétrie
    int size = mesh.size;
    for (int i = 0; i < size; i++)
    {
      vertex v = mesh[i];
      v.p.y *= -1.f;
      // FIXME: normale et tangente ?
      mesh.add(v);
    }
  }

  void generateCubicTorus(Array<vertex> & dest, float outerSize, float innerSize, float height)
  {
    const float width = 0.5f * (outerSize - innerSize);
    const float position = 0.5f * (innerSize + width);
    assert(width > 0.f);
    Array<vertex> paveA(Cube::numberOfVertices);
    Array<vertex> paveB(Cube::numberOfVertices);
    generatePave(paveA, width, height, outerSize);
    generatePave(paveB, width, height, innerSize);
	rotate(paveB, 90.f, 0, 1.f, 0);

	glPushMatrix();
	glLoadIdentity();

	glTranslatef(position, 0.f, 0.f);
    addVerticesUsingCurrentProjection(dest, paveA);

	glTranslatef(-2.f * position, 0.f, 0.f);
    addVerticesUsingCurrentProjection(dest, paveA);

	glTranslatef(position, 0.f, position);
    addVerticesUsingCurrentProjection(dest, paveB);

	glTranslatef(0, 0.f, -2.f * position);
    addVerticesUsingCurrentProjection(dest, paveB);

	glPopMatrix();
  }

  // Génère un cube de 1 de côté, troué. Le trou va de (x1, y1) à (x2, y2)
  //
  //  +-------+
  //  |       |
  //  |  +--+ |
  //  |  +--+ |
  //  +-------+
  void generateCustomCubicTorus(Array<vertex> & dest, float x1, float y1, float x2, float y2)
  {
    assert(x1 < x2);
    assert(y1 < y2);
    assert(x1 < 1.f && x2 < 1.f && y1 < 1.f && y2 < 1.f);

    glPushMatrix();
    glLoadIdentity();

    // génère un cube unitaire centré en 0,5
    Array<vertex> & cube = Mesh::getTemp();
    Mesh::generatePave(cube, 1.f, 1.f, 1.f);
    glTranslatef(0.5f, 0.5f, 0.5f);
    applyCurrentProjection(cube);

    // pavé gauche
    glLoadIdentity();
    glScalef(x1, 1.f, 1.f);
    addVerticesUsingCurrentProjection(dest, cube);

    // pavé bas
    glLoadIdentity();
    glTranslatef(x1, 0.f, 0.f);
    glScalef(x2 - x1, y1, 1.f);
    addVerticesUsingCurrentProjection(dest, cube);

    // pavé haut
    glLoadIdentity();
    glTranslatef(x1, y2, 0.f);
    glScalef(x2 - x1, 1.f - y2, 1.f);
    addVerticesUsingCurrentProjection(dest, cube);

    // pavé droite
    glLoadIdentity();
    glTranslatef(x2, 0, 0.f);
    glScalef(1.f - x2, 1.f, 1.f);
    addVerticesUsingCurrentProjection(dest, cube);

    glPopMatrix();
  }

  // FIXME: Attention à la globale : ce n'est pas thread safe !
  float gThicknessRatio;
  static vector3f couronneHFunc(float t, float theta) { return vector3f(0, 0, 0); }
  static float couronneRFunc(float t, float theta) { return (1.f - gThicknessRatio + gThicknessRatio * t); }

  // cylindre creu avec une épaisseur
  // thickness_ratio est l'épaisseur relative (entre 0 et 1)
  void generateTorusCylinder(Array<vertex> & mesh, float height, float diameter,
                             float thicknessRatio, int thetaFaces)
  {
    gThicknessRatio = thicknessRatio;
    Revolution outPipe(pipeHFunc, pipeRFunc);
    Revolution inPipe(inPipeHFunc, pipeRFunc);
    Revolution couronne(couronneHFunc, couronneRFunc);

    Array<vertex> & bellOut = getTemp();
    Array<vertex> & bellIn = getTemp();
    Array<vertex> & bellCouronne = getTemp();

    outPipe.generateMesh(bellOut, height, 0.5f * diameter, 1, thetaFaces);
    inPipe.generateMesh(bellIn, height, 0.5f * diameter * (1.f - thicknessRatio), 1, thetaFaces);
    couronne.generateMesh(bellCouronne, 1.f, 0.5f * diameter, 1, thetaFaces);

    addVerticesUsingCurrentProjection(mesh, bellOut);
    addVerticesUsingCurrentProjection(mesh, bellIn);
    addVerticesUsingCurrentProjection(mesh, bellCouronne);
    glPushMatrix();
    glTranslatef(0, height, 0);
    glRotatef(180.f, 1.f, 0, 0);
    addVerticesUsingCurrentProjection(mesh, bellCouronne);
    glPopMatrix();
  }

  // FIXME: déplacer ça autre part
  static float sign(float x)
  {
    if (msys_fabsf(x) < _TV(0.01f)) return x;
    return 1.f - 2.f * float(x < 0);
  }

  //
  // Éloigne les sommets, faisant ainsi apparaitre un pavé central
  // utile pour faire un cube arrondi
  //
  // La taille augmente de 2x, 2y, 2z
  //
  void expandPave(Array<vertex> & m, float x, float y, float z)
  {
    for (int i = 0; i < m.size; i++)
    {
      m[i].p.x += sign(m[i].p.x) * x;
      m[i].p.y += sign(m[i].p.y) * y;
      m[i].p.z += sign(m[i].p.z) * z;
    }
  }

  void translate(Array<vertex> & m, float x, float y, float z)
  {
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, z);
    applyCurrentProjection(m);
    glPopMatrix();
  }

  void rotate(Array<vertex> & m, float angle, float x, float y, float z)
  {
    glPushMatrix();
    glLoadIdentity();
    glRotatef(angle, x, y, z);
    applyCurrentProjection(m);
    glPopMatrix();
  }

  void scale(Array<vertex> & m, float x, float y, float z)
  {
    glPushMatrix();
    glLoadIdentity();
    glScalef(x, y, z);
    applyCurrentProjection(m);
    glPopMatrix();
  }
}
