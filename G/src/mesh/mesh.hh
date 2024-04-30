//
// Définitions pour les meshs
//

#ifndef		MESH_HH
# define	MESH_HH

#include "array.hxx"
#include "algebra/matrix.hh"
#include "algebra/vector2.hh"
#include "array.hh"
#include "vertex.hh"

namespace Texture
{
  class Channel;
}

// struct mesh
// {
//   Array<vertex> vertices;
//   Array<int> indices; // 4 par 4, on affiche par quads

//   mesh(int nbVertices, int nbIndices): vertices(nbVertices), indices(nbIndices) {}
// };

namespace Mesh
{
  struct MeshStruct
  {
    Array<vertex> vertices;

    MeshStruct(): vertices() {}
    MeshStruct(int capacity): vertices(capacity) {}

    void clear();
    void add(const MeshStruct & mesh);
    void add(const MeshStruct & mesh, const matrix4 & transform);
    void removeFace(int index);

    /*
    // Pour le reste :
    typedef vertex *(filterOperator)(const vertex& vertex);
    typedef vertex *(combineOperator)(const vertex& lhs, const vertex& rhs);
    void filter(filterOperator op);
    void combine(const MeshStruct & mesh, combineOperator op);
    */

    void transform(const matrix4 & mat);
    void translate(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);
    void scale(float x, float y, float z);

    void computeNormals();
    void computeBaryCoord();
    void rotateTangents();

    void setColor(const vector3f & color);
    void setColorPerFace(const vector3f * colors, int size);

    void setId(float id);
    void setIdPerFace();

    void reprojectTexture(const vector3f & U, const vector3f & V);
    void reprojectTextureXZPlane(float scale);
    void reprojectTextureXYPlane(float scale);
    void reprojectTextureZYPlane(float scale);
    void addTexCoordNoise(float coef);
    void scaleTexture(float uFactor, float vFactor);
    void translateTexture(float uOffset, float vOffset);

    void generateFromHeightMap(const Texture::Channel & tex, int resx, int resy);
    void generateTorusCylinder(float height, float diameter,
                               float thicknessRatio, int thetaFaces);
    void generatePrisme(const Array<vector2f> & base, const vector3f& h);
    void generateCubicTorus(float outerSize, float innerSize, float width);
    void generateCustomCubicTorus(float x1, float y1, float x2, float y2);
    void generate2DShape(const vector2f * points, int numberOfPoints,
      bool front, bool back);

    void splitFace(const vertex* vertices, int x, int y);
    void splitAllFaces(int x);
    void splitAllFacesDownToMaxSide(float maxSide);

    void expandPave(float x, float y, float z);
    void addNoise(float coef);


#if DEBUG
    void saveToJSON(char *file);
#endif
  };

  void transformVertex(const matrix4 & transform, vertex & v);
  vertex transformedVertex(const matrix4 & transform, vertex v);

  void computeFaceBaryCoord(vertex & A, vertex & B, vertex & C, vertex & D);

  vector3f computeNormal(const vector3f & O,
			 const vector3f & A,
			 const vector3f & B,
			 const vector3f & C,
			 const vector3f & D);
  void computeFaceNormal(vertex & A, vertex & B, vertex & C, vertex & D);
}

#endif		// MESH_HH
