//
// Définitions pour les meshs
//

#include "mesh.hh"

#include <stdio.h>

#include "algebra/vector3.hxx"
#include "array.hxx"
#include "cube.hh"
#include "interpolation.hxx"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "mesh/revolution.hh"
#include "mesh/revolutionfunctions.hh"
#include "randomness.hh"
#include "texgen/texture.hh"

namespace Mesh
{
  //
  // Vertex transformé par une matrice
  //
  void transformVertex(const matrix4 & transform, vertex & v)
  {
#if VERTEX_ATTR_POSITION
    applyMatrixToPoint(transform, v.p);
#endif

#if VERTEX_ATTR_NORMAL
    applyMatrixToVector(transform, v.n);
#endif

#if VERTEX_ATTR_TANGENT
    applyMatrixToVector(transform, v.t);
#endif
  }

  vertex transformedVertex(const matrix4 & transform, vertex v)
  {
    transformVertex(transform, v);
    return v;
  }

  void MeshStruct::clear()
  {
    vertices.empty();
  }

  //
  // Ajout à un tas de sommets d'un autre tas de sommets
  //
  void MeshStruct::add(const MeshStruct & mesh)
  {
    IFDBG(assert(vertices.max_size >= vertices.size + mesh.vertices.size));

    const int size = mesh.vertices.size;
    for (int i = 0; i < size; ++i)
    {
      vertices.add(mesh.vertices[i]);
    }
  }

  //
  // Ajout à un tas de sommets d'un autre tas de sommets transformé
  // par une matrice
  //
  void MeshStruct::add(const MeshStruct & mesh, const matrix4 & transform)
  {
    IFDBG(assert(vertices.max_size >= vertices.size + mesh.vertices.size));

    const int size = mesh.vertices.size;
    for (int i = 0; i < size; ++i)
    {
      vertices.add(transformedVertex(transform, mesh.vertices[i]));
    }
  }

  void MeshStruct::generate2DShape(const vector2f * points, int numberOfPoints,
    bool front, bool back)
  {
    if (front)
    {
      for (int i = 0; i < numberOfPoints; ++i)
      {
	vertex v = Cube::vertices[12];
#if VERTEX_ATTR_POSITION
	const int j = i;
	v.p.x = points[j].x;
	v.p.y = 0;
	v.p.z = points[j].y;
#endif
	vertices.add(v);
      }
    }
    if (back)
    {
      for (int i = 0; i < numberOfPoints; ++i)
      {
	vertex v = Cube::vertices[8];
	const int j = (numberOfPoints - 1 - i) % numberOfPoints;
#if VERTEX_ATTR_POSITION
	v.p.x = points[j].x;
	v.p.y = 0;
	v.p.z = points[j].y;
#endif
	vertices.add(v);
      }
    }
  }

  //
  // Crée un prisme à partir d'une base arbitraire.
  //
  /*
  void generatePrisme(Array<vertex> & dest, const Array<vector2f> & base, const vector3f& h)
  {
    // base n'a pas besoin d'être un chemin fermé : on fermera la boucle de toute façon
    assert(base.size >= 3);
    assert(dest.hasEnoughRoomFor((base.size + 1) * 4));

    Array<vertex> uniqueVertices((2 * base.size + 1) * 2);
    for (int i = 0; i <= base.size; ++i)
    {
      const vector3f bottom(base[i % base.size].x, 0, base[i % base.size].y);
      const vector3f top = bottom + h;
      uniqueVertices.add(bottom);
      uniqueVertices.add(top);
      uniqueVertices.add(bottom);
      uniqueVertices.add(top);
    }

    // On finit comme dans Revolution::generateMesh()
    // => déplacer ces deux fonctions dans Mesh
    _computeNormals(uniqueVertices, 1, thetaFaces);
    _makeQuadsFromVertices(uniqueVertices, vertices, 1, thetaFaces);

    // FIXME: on n'a que le tube, il faut fermer les bords
  }
  */

  static vertex interpolate3(const vertex & v1, const vertex & v2, const vertex & v3, float x, float y)
  {
    assert(x + y <= 1.f); // sinon, ça sort du triangle (v1, v2, v3)

    vertex v = v1;
    // FIXME: couleurs et normales
#if VERTEX_ATTR_POSITION
    v.p = v1.p + (v2.p - v1.p) * x + (v3.p - v1.p) * y;
#endif

#if VERTEX_ATTR_TEX_COORD
    v.u = v1.u + (v2.u - v1.u) * x + (v3.u - v1.u) * y;
    v.v = v1.v + (v2.v - v1.v) * x + (v3.v - v1.v) * y;
    v.w = v1.w + (v2.w - v1.w) * x + (v3.w - v1.w) * y;
    v.q = v1.q + (v2.q - v1.q) * x + (v3.q - v1.q) * y;
#endif

#if VERTEX_ATTR_BARY_COORD
    v.d0 = v1.d0 + (v2.d0 - v1.d0) * x + (v3.d0 - v1.d0) * y;
    v.d1 = v1.d1 + (v2.d1 - v1.d1) * x + (v3.d1 - v1.d1) * y;
    v.d2 = v1.d2 + (v2.d2 - v1.d2) * x + (v3.d2 - v1.d2) * y;
    v.d3 = v1.d3 + (v2.d3 - v1.d3) * x + (v3.d3 - v1.d3) * y;
#endif

    return v;
  }

  static vertex interpolate4(const vertex* vertices, float x, float y)
  {
    if (x + y < 1.f)
      return interpolate3(vertices[0], vertices[1], vertices[3], x, y);
    else
      return interpolate3(vertices[2], vertices[1], vertices[3], 1.f - y, 1.f - x);
  }

  void MeshStruct::removeFace(int index)
  {
    assert(index % 4 == 0);
    for (int i = 3; i >= 0; i--)
    {
      vertices.remove(index + i);
    }
  }

  // Pivoter les tangentes de 90°
  void MeshStruct::rotateTangents()
  {
#if VERTEX_ATTR_NORMAL && VERTEX_ATTR_TANGENT
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].t = cross(vertices[i].n, vertices[i].t);
    }
#endif
  }

  inline
  float distancePointLine(const vector3f & p, const vector3f & a, const vector3f & b)
  {
    const vector3f PA = a - p;
    const vector3f AB = b - a;
    float dotABAB = dot(AB, AB);

    if (dotABAB == 0) return norm(PA);

    const vector3f PH = PA - AB * dot(PA, AB) / dotABAB;
    return norm(PH);
  }

  void computeFaceBaryCoord(vertex & A, vertex & B, vertex & C, vertex & D)
  {
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_BARY_COORD
    // Distance entre chaque sommet et les deux faces opposées
    const float dABC = distancePointLine(A.p, B.p, C.p);
    const float dACD = distancePointLine(A.p, C.p, D.p);

    const float dBCD = distancePointLine(B.p, C.p, D.p);
    const float dBDA = distancePointLine(B.p, D.p, A.p);

    const float dCDA = distancePointLine(C.p, D.p, A.p);
    const float dCAB = distancePointLine(C.p, A.p, B.p);

    const float dDAB = distancePointLine(D.p, A.p, B.p);
    const float dDBC = distancePointLine(D.p, B.p, C.p);

    A.d0 = dABC; A.d1 = dACD; A.d2 =    0; A.d3 =    0;
    B.d0 =    0; B.d1 = dBCD; B.d2 = dBDA; B.d3 =    0;
    C.d0 =    0; C.d1 =    0; C.d2 = dCDA; C.d3 = dCAB;
    D.d0 = dDBC; D.d1 =    0; D.d2 =    0; D.d3 = dDAB;
#endif
  }

  void MeshStruct::computeBaryCoord()
  {
    for (int i = 0; i < vertices.size; i += 4)
    {
      computeFaceBaryCoord(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3]);
    }
  }

  // Split a face in x*y faces
  void MeshStruct::splitFace(const vertex* oldVertices, int x, int y)
  {
    assert(x > 0 && y > 0);

    for (int i = 0; i < x; i++)
    {
      for (int j = 0; j < y; j++)
      {
        vertex A = interpolate4(oldVertices, (float) i / x,     (float) j / y);
        vertex B = interpolate4(oldVertices, (float) (i+1) / x, (float) j / y);
        vertex C = interpolate4(oldVertices, (float) (i+1) / x, (float) (j+1) / y);
        vertex D = interpolate4(oldVertices, (float) i / x,     (float) (j+1) / y);
        vertices.add(A);
        vertices.add(B);
        vertices.add(C);
        vertices.add(D);
      }
    }
  }

  void MeshStruct::splitAllFaces(int x)
  {
    MeshStruct originalMesh(vertices.size);
    originalMesh.add(*this);

    clear();
    for (int i = 0; i < originalMesh.vertices.size; i += 4)
    {
      splitFace(&originalMesh.vertices[i], x, x);
    }
  }

  void MeshStruct::splitAllFacesDownToMaxSide(float maxSide)
  {
#if VERTEX_ATTR_POSITION
    MeshStruct originalMesh(vertices.size);
    originalMesh.add(*this);

    clear();
    for (int i = 0; i < originalMesh.vertices.size; i += 4)
    {
      const vector3f AB = originalMesh.vertices[i + 1].p - originalMesh.vertices[i    ].p;
      const vector3f BC = originalMesh.vertices[i + 2].p - originalMesh.vertices[i + 1].p;
      const vector3f CD = originalMesh.vertices[i + 3].p - originalMesh.vertices[i + 2].p;
      const vector3f DA = originalMesh.vertices[i    ].p - originalMesh.vertices[i + 3].p;

      int xSplit = 1 + msys_ifloorf(msys_max(norm(AB), norm(CD)) / maxSide);
      int ySplit = 1 + msys_ifloorf(msys_max(norm(BC), norm(DA)) / maxSide);

      splitFace(&originalMesh.vertices[i], xSplit, ySplit);
    }
#endif
  }

  void MeshStruct::addNoise(float coef)
  {
#if VERTEX_ATTR_POSITION
    for (int i = 0; i < vertices.size; i++)
    {
      float seed = 10.f * norm(vertices[i].p) + 2.f * vertices[i].p.x + vertices[i].p.y;
      Rand rand((int)(100.f * seed));
      vertices[i].p += vector3f(rand.sfgen(), rand.sfgen(), rand.sfgen()) * coef;
      // TODO: normals
    }
#endif
  }

  // ========================================================================
  // Normales

  vector3f computeNormal(const vector3f & O,
			 const vector3f & A,
			 const vector3f & B,
			 const vector3f & C,
			 const vector3f & D)
  {
    const vector3f OA = A - O;
    const vector3f OB = B - O;
    const vector3f OC = C - O;
    const vector3f OD = D - O;

    const vector3f n1 = cross(OB, OA);
    const vector3f n2 = cross(OC, OB);
    const vector3f n3 = cross(OD, OC);
    const vector3f n4 = cross(OA, OD);

    vector3f n = n1 + n2 + n3 + n4;
    normalize(n);

    return n;
  }

  void computeFaceNormal(vertex & A, vertex & B, vertex & C, vertex & D)
  {
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_NORMAL
    const vector3f AC = C.p - A.p;
    const vector3f BD = D.p - B.p;

    vector3f n = cross(AC, BD);
    normalize(n);

    A.n = n;
    B.n = n;
    C.n = n;
    D.n = n;
#endif
  }

  void MeshStruct::computeNormals()
  {
    for (int i = 0; i < vertices.size; i += 4)
    {
      computeFaceNormal(vertices[i], vertices[i+1], vertices[i+2], vertices[i+3]);
    }
  }

  // ========================================================================
  // Color

  void MeshStruct::setColor(const vector3f & color)
  {
#if VERTEX_ATTR_ID
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].r = color.x;
      vertices[i].g = color.y;
      vertices[i].b = color.z;
    }
#endif
  }

  void MeshStruct::setColorPerFace(const vector3f * colors, int size)
  {
#if VERTEX_ATTR_ID
    Rand rand;
    int index = 0;
    for (int i = 0; i < vertices.size; ++i)
    {
      if (i % 4 == 0) index = rand.igen() % size;

      vertices[i].r = colors[index].x;
      vertices[i].g = colors[index].y;
      vertices[i].b = colors[index].z;
    }
#endif
  }

  // ========================================================================
  // Identifiant de sommet

  void MeshStruct::setId(float id)
  {
#if VERTEX_ATTR_ID
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].id = id;
    }
#endif
  }

  void MeshStruct::setIdPerFace()
  {
#if VERTEX_ATTR_ID
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].id = float(i / 4);
    }
#endif
  }

  // ========================================================================
  // Coordonnées de texture

  void MeshStruct::reprojectTexture(const vector3f & U, const vector3f & V)
  {
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_TEX_COORD
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].u = dot(U, vertices[i].p);
      vertices[i].v = dot(V, vertices[i].p);
    }
#endif
  }

  void MeshStruct::reprojectTextureXZPlane(float scale)
  {
    const vector3f u(scale, 0, 0);
    const vector3f v(0, 0, scale);
    reprojectTexture(u, v);
  }

  void MeshStruct::reprojectTextureXYPlane(float scale)
  {
    const vector3f u(scale, 0, 0);
    const vector3f v(0, scale, 0);
    reprojectTexture(u, v);
  }

  void MeshStruct::reprojectTextureZYPlane(float scale)
  {
    const vector3f u(0, 0, scale);
    const vector3f v(0, scale, 0);
    reprojectTexture(u, v);
  }

  void MeshStruct::addTexCoordNoise(float coef)
  {
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_TEX_COORD
    for (int i = 0; i < vertices.size; ++i)
    {
      float seed = 10.f * norm(vertices[i].p) + 2.f * vertices[i].p.x + vertices[i].p.y;
      Rand rand((int)(100.f * seed));
      vertices[i].u += rand.sfgen() * coef;
      vertices[i].v += rand.sfgen() * coef;
      vertices[i].w += rand.sfgen() * coef;
      vertices[i].q += rand.sfgen() * coef;
    }
#endif
  }

  void MeshStruct::scaleTexture(float uFactor, float vFactor)
  {
#if VERTEX_ATTR_TEX_COORD
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].u *= uFactor;
      vertices[i].v *= vFactor;
    }
#endif
  }

  void MeshStruct::translateTexture(float uOffset, float vOffset)
  {
#if VERTEX_ATTR_TEX_COORD
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i].u += uOffset;
      vertices[i].v += vOffset;
    }
#endif
  }

  // =========================================================================


  static vector3f vectorFromHeightMap(const Texture::Channel & tex, float x, float z)
  {
    return vector3f(x, tex.Bilinear(tex.Width() * x, tex.Height() * z), z);
  }

#if VERTEX_ATTR_POSITION
# define POS(x, y, z) vector3f(x, y, z),
#else
# define POS(x, y, z)
#endif

#if VERTEX_ATTR_NORMAL
# define NOR(x, y, z) vector3f(x, y, z),
#else
# define NOR(x, y, z)
#endif

#if VERTEX_ATTR_TANGENT
# define TAN(x, y, z) vector3f(x, y, z),
#else
# define TAN(x, y, z)
#endif

#if VERTEX_ATTR_COLOR
# define COL(r, g, b) r, g, b,
#else
# define COL(r, g, b)
#endif

#if VERTEX_ATTR_TEX_COORD
# define UV(u, v, w, q) u, v, w, q,
#else
# define UV(u, v, w, q)
#endif

#if VERTEX_ATTR_BARY_COORD
# define BAR(d0, d1, d2, d3) d0, d1, d2, d3,
#else
# define BAR(d0, d1, d2, d3)
#endif

#if VERTEX_ATTR_ID
# define ID0 0,
#else
# define ID0
#endif

  static vertex rectangle[] =
  {
    {POS(0, 0, 0) NOR(0, 1, 0) TAN(1, 0, 0) COL(1, 1, 1) UV(0, 0, 0, 1) BAR(1, 1, 0, 0) ID0},
    {POS(1, 0, 0) NOR(0, 1, 0) TAN(1, 0, 0) COL(1, 1, 1) UV(1, 0, 0, 1) BAR(0, 1, 1, 0) ID0},
    {POS(1, 0, 1) NOR(0, 1, 0) TAN(1, 0, 0) COL(1, 1, 1) UV(1, 1, 0, 1) BAR(0, 0, 1, 1) ID0},
    {POS(0, 0, 1) NOR(0, 1, 0) TAN(1, 0, 0) COL(1, 1, 1) UV(0, 1, 0, 1) BAR(1, 0, 0, 1) ID0},
  };

  void MeshStruct::generateFromHeightMap(const Texture::Channel & tex, int resx, int resy)
  {
#if VERTEX_ATTR_POSITION
    assert(vertices.size == 0);

    const vertex * source = rectangle;
    splitFace(source, resx, resy);

    for (int i = 0; i < vertices.size; i++)
    {
      vertex & v = vertices[i];
      v.p = vectorFromHeightMap(tex, v.p.x, v.p.z);
    }

    // Suppression des faces où y=0
    for (int i = 0; i < vertices.size; i += 4)
    {
      float maxy = msys_max(msys_max(vertices[i].p.y, vertices[i+1].p.y),
                       msys_max(vertices[i+2].p.y, vertices[i+3].p.y));
      if (maxy <= 0.f) removeFace(i);
    }

#if VERTEX_ATTR_NORMAL
    // Calcul des normales
    const float incx = 1.f / resx;
    const float incz = 1.f / resy;
    for (int i = 0; i < vertices.size; i++)
    {
      vertex & v = vertices[i];
      vector3f A = vectorFromHeightMap(tex, v.p.x + incx, v.p.z);
      vector3f B = vectorFromHeightMap(tex, v.p.x, v.p.z + incz);
      vector3f C = vectorFromHeightMap(tex, v.p.x - incx, v.p.z);
      vector3f D = vectorFromHeightMap(tex, v.p.x, v.p.z - incz);
      v.n = computeNormal(v.p, A, B, C, D);
      // FIXME: tangente
    }
#endif

    // Symétrie
    int size = vertices.size;
    for (int i = 0; i < size; i++)
    {
      vertex v = vertices[i];
      v.p.y *= -1.f;
      // FIXME: normale et tangente ?
      vertices.add(v);
    }
#endif
  }

  void MeshStruct::generateCubicTorus(float outerSize, float innerSize, float height)
  {
    const float width = 0.5f * (outerSize - innerSize);
    const float position = 0.5f * (innerSize + width);
    assert(width > 0.f);
    MeshStruct paveA(Cube::numberOfVertices);
    MeshStruct paveB(Cube::numberOfVertices);
    Pave(width, height, outerSize).generateMesh(paveA);
    Pave(width, height, innerSize).generateMesh(paveB);
    paveB.rotate(DEG_TO_RAD * 90.f, 0, 1.f, 0);

    add(paveA, matrix4::translation(vector3f(position, 0.f, 0.f)));
    add(paveA, matrix4::translation(vector3f(-position, 0.f, 0.f)));
    add(paveB, matrix4::translation(vector3f(0.f, 0.f, position)));
    add(paveB, matrix4::translation(vector3f(0.f, 0.f, -position)));
  }

  // Génère un cube de 1 de côté, troué. Le trou va de (x1, y1) à (x2, y2)
  //
  //  +-------+
  //  |       |
  //  |  +--+ |
  //  |  +--+ |
  //  +-------+
  void MeshStruct::generateCustomCubicTorus(float x1, float y1, float x2, float y2)
  {
    assert(x1 < x2);
    assert(y1 < y2);
    assert(x1 < 1.f && x2 < 1.f && y1 < 1.f && y2 < 1.f);

    // génère un cube unitaire centré en 0,5
    MeshStruct & cube = Mesh::getTempMesh();
    Pave(1.f, 1.f, 1.f).generateMesh(cube);
    cube.translate(0.5f, 0.5f, 0.5f);

    // pavé gauche
    add(cube, matrix4::scaling(vector3f(x1, 1.f, 1.f)));

    // pavé bas
    add(cube, matrix4::translation(vector3f(x1, 0.f, 0.f)).scale(vector3f(x2 - x1, y1, 1.f)));

    // pavé haut
    add(cube, matrix4::translation(vector3f(x1, y2, 0.f)).scale(vector3f(x2 - x1, 1.f - y2, 1.f)));

    // pavé droite
    add(cube, matrix4::translation(vector3f(x2, 0, 0.f)).scale(vector3f(1.f - x2, 1.f, 1.f)));
  }

  // FIXME: Attention à la globale : ce n'est pas thread safe !
  float gThicknessRatio;
  static vector3f couronneHFunc(float t, float theta) { return vector3f(0, 0, 0); }
  static float couronneRFunc(float t, float theta) { return (1.f - gThicknessRatio + gThicknessRatio * t); }

  // cylindre creu avec une épaisseur
  // thickness_ratio est l'épaisseur relative (entre 0 et 1)
  void MeshStruct::generateTorusCylinder(float height, float diameter,
                             float thicknessRatio, int thetaFaces)
  {
    gThicknessRatio = thicknessRatio;
    Revolution outPipe(pipeHFunc, pipeRFunc);
    Revolution inPipe(inPipeHFunc, pipeRFunc);
    Revolution couronne(couronneHFunc, couronneRFunc);

    MeshStruct & bellOut = getTempMesh();
    MeshStruct & bellIn = getTempMesh();
    MeshStruct & bellCouronne = getTempMesh();

    outPipe.generateMesh(bellOut, height, 0.5f * diameter, 1, thetaFaces);
    inPipe.generateMesh(bellIn, height, 0.5f * diameter * (1.f - thicknessRatio), 1, thetaFaces);
    couronne.generateMesh(bellCouronne, 1.f, 0.5f * diameter, 1, thetaFaces);

    add(bellOut);
    add(bellIn);
    add(bellCouronne);
    add(bellCouronne, matrix4::translation(vector3f(0, height, 0)).rotate(DEG_TO_RAD * 180.f, vector3f::ux));
  }

  //
  // Éloigne les sommets, faisant ainsi apparaitre un pavé central
  // utile pour faire un cube arrondi
  //
  // La taille augmente de 2x, 2y, 2z
  //
  void MeshStruct::expandPave(float x, float y, float z)
  {
#if VERTEX_ATTR_POSITION
    for (int i = 0; i < vertices.size; i++)
    {
      vertices[i].p.x += sign(vertices[i].p.x) * x;
      vertices[i].p.y += sign(vertices[i].p.y) * y;
      vertices[i].p.z += sign(vertices[i].p.z) * z;
    }
#endif
  }

  void MeshStruct::transform(const matrix4 & mat)
  {
    for (int i = 0; i < vertices.size; ++i)
    {
      vertices[i] = transformedVertex(mat, vertices[i]);
    }
  }

  void MeshStruct::translate(float x, float y, float z)
  {
    const matrix4 mat = matrix4::translation(vector3f(x, y, z));
    transform(mat);
  }

  void MeshStruct::rotate(float angle, float x, float y, float z)
  {
    const matrix4 mat = matrix4::rotation(angle, vector3f(x, y, z));
    transform(mat);
  }

  void MeshStruct::scale(float x, float y, float z)
  {
    const matrix4 mat = matrix4::scaling(vector3f(x, y, z));
    transform(mat);
  }

#if DEBUG
  void MeshStruct::saveToJSON(char *file)
  {
    // https://github.com/mrdoob/three.js/wiki/JSON-Model-format-3
    // TODO: export material
    // TODO: export normals
    FILE * fp = fopen(file, "w");
    assert(fp != NULL);
    char* header =
      "{\n"
      "\"metadata\": { \"formatVersion\" : 3 },\n"
      "\n"
      "\"materials\": [{\n"
      "  \"colorAmbient\" : [0.8, 0.8, 0.8],\n"
      "  \"colorDiffuse\" : [0.03, 0.3, 0.06],\n"
      "  \"colorSpecular\" : [0.5, 0.5, 0.5],\n"
      "  \"shading\" : \"Lambert\",\n"
      "  \"specularCoef\" : 10\n"
      "  }],\n"
      "\n"
      "\"vertices\": [\n";
    fprintf(fp, "%s", header);
    for (int i = 0; i < vertices.size; i++)
    {
      fprintf(fp, "%f,%f,%f", vertices[i].p.x, vertices[i].p.y, vertices[i].p.z);
      if (i + 1 < vertices.size) fprintf(fp, ",\n");
    }
    fprintf(fp, "],\n");
    fprintf(fp, "\"normals\": [\n");
    for (int i = 0; i < vertices.size; i++)
    {
      fprintf(fp, "%f,%f,%f", vertices[i].n.x, vertices[i].n.y, vertices[i].n.z);
      if (i + 1 < vertices.size) fprintf(fp, ",\n");
    }
    fprintf(fp, "],\n");
    fprintf(fp, "\"faces\": [\n");
    for (int i = 0; i < vertices.size; i += 4)
    {
      int kind = 1/*quad*/ | 32/*normals*/;
      fprintf(fp, "%d, %d,%d,%d,%d, %d,%d,%d,%d", kind, i, i+1, i+2, i+3, i, i+1, i+2, i+3);
      if (i + 4 < vertices.size) fprintf(fp, ",\n");
    }
    fprintf(fp, "]}\n");
    fclose(fp);
  }
#endif
}
