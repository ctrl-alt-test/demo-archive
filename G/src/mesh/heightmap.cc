//
// Topologie "place"
//

#include "heightmap.hh"

#include "algebra/vector3.hh"
#include "interpolation.hh"
#include "spline.hh"

#include "sys/msys_debug.h"
#include "sys/msys_libc.h"

namespace Mesh
{
  static vector3f computeTangent(const vector3f & n,
				 const vector3f & OA)
  {
    const vector3f bt = cross(n, OA);

    vector3f t = cross(bt, n);
    normalize(t);

    return t;
  }

  void Heightmap::_computePositions(Array<vertex> & vertices,
				     float xScale, float yScale,
				     int xFaces, int yFaces)
  {
    for (int j = 0; j <= yFaces; ++j)
    {
      const float y = float(j) / yFaces;

      for (int i = 0; i <= xFaces; ++i)
      {
	  const float x = float(i) / yFaces;
	  const float z = _heightFunc(x, y);

	  const vector3f pos = vector3f(xScale * x, yScale * y, z);

	  const vertex p = {
#if VERTEX_ATTR_POSITION
	    pos, // Position
#endif
#if VERTEX_ATTR_NORMAL
	    vector3f(0, 0, 0), // Normale (plus tard)
#endif
#if VERTEX_ATTR_TANGENT
	    vector3f(0, 0, 0), // Tangente (plus tard)
#endif
#if VERTEX_ATTR_COLOR
	    0.f, 1.f, 1.f, // Couleur
#endif
#if VERTEX_ATTR_TEX_COORD
	    x, y, 0, 1.f, // Coordonnée de texture
#endif
#if VERTEX_ATTR_BARY_COORD
	    0, 0, 0, 0, // Coordonnées barycentriques (plus tard)
#endif
#if VERTEX_ATTR_ID
	    0,
#endif
	  };

	  vertices.add(p);
      }
    }
  }

  static
  vector3f computeNormalWithIndices(const Array<vertex> & vertices,
				    int stride,
				    int i0, int i1, int i2,
				    int j0, int j1, int j2)
  {
#if VERTEX_ATTR_POSITION
    const vector3f & _O = vertices[i1 + j1 * stride].p;
    const vector3f & _A = vertices[i2 + j1 * stride].p;
    const vector3f & _B = vertices[i1 + j2 * stride].p;
    const vector3f & _C = vertices[i0 + j1 * stride].p;
    const vector3f & _D = vertices[i1 + j0 * stride].p;

    return Mesh::computeNormal(_O, _A, _B, _C, _D);
#else
    assert(false);
    return vector3f();
#endif
  }

  vector3f generalPurposeComputeNormal(const Array<vertex> & vertices,
				       int xFaces, int yFaces,
				       int i1, int j1)
  {
    const int j0 = msys_max(j1 - 1, 0);
    const int j2 = msys_min(j1 + 1, yFaces);

    const int i0 = msys_max(i1 - 1, 0);
    const int i2 = msys_min(i1 + 1, xFaces);

    return Mesh::computeNormalWithIndices(vertices, xFaces + 1, i0, i1, i2, j0, j1, j2);
  }

  void Heightmap::_computeNormals(Array<vertex> & vertices,
				  int xFaces, int yFaces)
  {
#if VERTEX_ATTR_POSITION
    for (int j = 0; j <= yFaces; ++j)
      for (int i = 0; i <= xFaces; ++i)
      {
	vertex & p = vertices[j * (xFaces + 1) + i];

	const int i2 = (i == xFaces ? 1 : i + 1);
	const vertex & _A = vertices[i2 + j  * (xFaces + 1)];
	const vector3f & O = p.p;
	const vector3f & A = _A.p;
	const vector3f OA = A - O;

#if VERTEX_ATTR_NORMAL
	p.n = _computeNormalFunc(vertices, xFaces, yFaces, i, j);
#if VERTEX_ATTR_TANGENT
 	p.t = computeTangent(p.n, OA);
#endif
#endif
      }
#endif
  }

  void Heightmap::_makeQuadsFromVertices(Array<vertex> & uniqueVertices,
					 Array<vertex> & vertices,
					 int xFaces, int yFaces)
  {
    for (int j = 0; j < yFaces; ++j)
      for (int i = 0; i < xFaces; ++i)
      {
	vertex A = uniqueVertices[ i      +  j      * (xFaces + 1)];
	vertex B = uniqueVertices[ i      + (j + 1) * (xFaces + 1)];
	vertex C = uniqueVertices[(i + 1) + (j + 1) * (xFaces + 1)];
	vertex D = uniqueVertices[(i + 1) +  j      * (xFaces + 1)];

#if VERTEX_ATTR_BARY_COORD
	A.d0 = 1; A.d1 = 1; A.d2 = 0; A.d3 = 0;
	B.d0 = 0; B.d1 = 1; B.d2 = 1; B.d3 = 0;
	C.d0 = 0; C.d1 = 0; C.d2 = 1; C.d3 = 1;
	D.d0 = 1; D.d1 = 0; D.d2 = 0; D.d3 = 1;
#endif

	vertices.add(A);
	vertices.add(B);
	vertices.add(C);
	vertices.add(D);
      }
  }

  void Heightmap::generateMesh(Array<vertex> & vertices,
			       float xScale, float yScale,
			       int xFaces, int yFaces)
  {
    assert(xFaces >= 1);
    assert(yFaces >= 1);

    Array<vertex> uniqueVertices((xFaces + 1) * (yFaces + 1));
    IFDBG(assert(vertices.max_size >= 4 * xFaces * yFaces));

    _computePositions(uniqueVertices, xScale, yScale, xFaces, yFaces);
    _computeNormals(uniqueVertices, xFaces, yFaces);
    _makeQuadsFromVertices(uniqueVertices, vertices, xFaces, yFaces);
  }
}
