//
// Forme de révolution
//

#ifndef		REVOLUTION_HH
# define	REVOLUTION_HH

#include "algebra/vector2.hh"
#include "algebra/vector3.hh"
#include "mesh/revolutionfunctions.hh"
#include "spline.hh"
#include "variable.hh"

namespace Mesh
{
  struct MeshStruct;

  void generateTestMeshes();

  class Revolution
  {
  public:
    typedef float (*fttheta)(float t, float thetaLevel);
    typedef vector2f (*ftsection)(float t, float thetaLevel);
    typedef vector3f (*fttheta_center)(float t, float thetaLevel);
    typedef vector3f (*normalCompute)(const Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);

    // hFunc : fonction qui donne la hauteur en fonction de t et theta
    // rFunc : fonction qui donne le rayon en fonction de t et theta
    Revolution(fttheta_center hFunc, fttheta rFunc, ftsection secFunc = NULL, Variable * var = NULL):
      _hFunc(hFunc),
      _rFunc(rFunc),
      _secFunc(secFunc),
      _spline(var),
      _computeNormalFunc(generalPurposeComputeNormal),
      _absoluteOrientation(false)//absoluteOrientation)
    {
      assert(rFunc != NULL || secFunc != NULL || var != NULL);
      assert(hFunc != NULL || var != NULL);
    }

    inline
    void setComputeNormalFunc(normalCompute computeNormalFunc)
    {
      _computeNormalFunc = computeNormalFunc;
    };

    // tFaces : nombres de faces suivant t
    // thetaFaces : nombre de faces suivant theta
    void generateMesh(MeshStruct & mesh,
		      float hScale, float rScale,
		      int tFaces, int thetaFaces);

  private:
    vector3f _splineHFunc(float t, float theta);
    float _splineRFunc(float t, float theta);
    vector2f _revolve(float t, float theta);

    vector3f _getCenterPosition(float t, float thetaLevel);
    void _computePositions(Array<vertex> & vertices,
			   float hScale, float rScale,
			   int tFaces, int thetaFaces);
    void _computeNormals(Array<vertex> & vertices,
			 int tFaces, int thetaFaces);
    void _makeQuadsFromVertices(Array<vertex> & uniqueVertices,
				Array<vertex> & vertices,
				int tFaces, int thetaFaces);

    fttheta_center	_hFunc;
    fttheta		_rFunc;
    ftsection		_secFunc;
    Variable *          _spline;
    normalCompute	_computeNormalFunc;
//     int			_hRepeat;
//     int			_rRepeat;
    bool		_absoluteOrientation;
  };

#if DEBUG
  Mesh::Revolution loadSplineFromFile(const char *file);
#endif

}

#endif // REVOLUTION_HH
