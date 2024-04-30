//
// Topologie "plane"
//

#ifndef		HEIGHTMAP_HH
# define	HEIGHTMAP_HH

#include "algebra/vector3.hh"
// #include "heightmapfunctions.hh"
#include "mesh.hh"
#include "spline.hh"
#include "variable.hh"

namespace Mesh
{
  void generateTestMeshes();
  vector3f generalPurposeComputeNormal(const Array<vertex> & vertices,
				       int xFaces, int yFaces,
				       int i1, int j1);

  class Heightmap
  {
  public:
    typedef float (*fxy)(float x, float y);
    typedef vector3f (*normalCompute)(const Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);

    // heightFunc : fonction qui donne la hauteur en fonction de x et y
    Heightmap(fxy heightFunc, Variable * var = NULL):
      _heightFunc(heightFunc),
      _spline(var),
      _computeNormalFunc(generalPurposeComputeNormal)
    {
      assert(heightFunc != NULL || var != NULL);
    }

    inline
    void setComputeNormalFunc(normalCompute computeNormalFunc)
    {
      _computeNormalFunc = computeNormalFunc;
    };

    void generateMesh(Array<vertex> & vertices,
		      float xScale, float yScale,
		      int xFaces, int yFaces);

  private:
    void _computePositions(Array<vertex> & vertices,
			   float hScale, float rScale,
			   int tFaces, int thetaFaces);
    void _computeNormals(Array<vertex> & vertices,
			 int xFaces, int yFaces);
    void _makeQuadsFromVertices(Array<vertex> & uniqueVertices,
				Array<vertex> & vertices,
				int xFaces, int yFaces);

    fxy			_heightFunc;
    Variable *          _spline;
    normalCompute	_computeNormalFunc;
  };
}

#endif		// HEIGHTMAP_HH
