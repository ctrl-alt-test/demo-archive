//
// Définition des types pour les meshs
//

#ifndef		MESH_HH
# define	MESH_HH

#include "array.hh"

#include "matrix.hh"

struct vertex
{
  vector3f p; // Position du sommet

  vector3f n; // Normale au sommet

  vector3f t; // Tangente au sommet

//   float r; // Couleur du sommet // FIXME : tester avec un unsigned int RGBA
//   float g;
//   float b;

  float u; // Coordonnée de texture du sommet
  float v;
  float w;
  float q;
};

struct mesh
{
  Array<vertex> vertices;
  Array<int> indices; // 4 par 4, on affiche par quads

  mesh(int nbVertices, int nbIndices): vertices(nbVertices), indices(nbIndices) {}
};

#if DEBUG

void immediateArrayDraw(const Array<vertex> & vertices, int vTanIndex);
void immediateMeshDraw(const mesh & m, int vTanIndex);

#endif // DEBUG

namespace Mesh
{
  extern Array<vertex> pool[24];
  extern int mid;

  void initPool();
  void clearPool();
  Array<vertex> & getTemp();
  void destroyPool();

  vertex transformedVertex(const matrix4 & transform, const vertex & v);

  void addVertices(Array<vertex> & vertices,
		   const Array<vertex> & moreVertices,
		   const matrix4 & transform);

  void addVerticesUsingCurrentProjection(Array<vertex> & vertices,
					 const Array<vertex> & moreVertices);
  void applyCurrentProjection(Array<vertex> & vertices);
  void generatePave(Array<vertex> & dest, float x, float y, float z);
  void generateCubicTorus(Array<vertex> & dest, float outerSize, float innerSize, float width);
  void generateCustomCubicTorus(Array<vertex> & dest, float x1, float y1, float x2, float y2);
  void rotateTangents(Array<vertex> & mesh);
  void splitFace(Array<vertex> & mesh, const vertex* vertices, int x, int y);
  void splitAllFaces(Array<vertex> & mesh, int x);
  void removeFace(Array<vertex> & mesh, int index);
  void addNoise(Array<vertex> & mesh, float coef);

  void reprojectTexture(Array<vertex> & m, vector3f U, vector3f V);
  void reprojectTextureXZPlane(Array<vertex> & mesh, float scale);
  void reprojectTextureXYPlane(Array<vertex> & mesh, float scale);
  void reprojectTextureZYPlane(Array<vertex> & mesh, float scale);
  void addTexCoordNoise(Array<vertex> & mesh, float coef);
  void scaleTexture(Array<vertex> & mesh, float uFactor, float vFactor);
  void translateTexture(Array<vertex> & mesh, float uOffset, float vOffset);

  void generateFromHeightMap(Array<vertex> & mesh, const Texture::Channel & tex, int resx, int resy);
  void generateTorusCylinder(Array<vertex> & mesh, float height, float diameter,
                             float thicknessRatio, int thetaFaces);
  void expandPave(Array<vertex> & m, float x, float y, float z);
  void translate(Array<vertex> & m, float x, float y, float z);
  void rotate(Array<vertex> & m, float angle, float x, float y, float z);
  void scale(Array<vertex> & m, float x, float y, float z);
}

#endif		// MESH_HH
