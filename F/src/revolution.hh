//
// Forme de révolution
//

#ifndef		REVOLUTION_HH
# define	REVOLUTION_HH

#include "array.hh"
#include "mesh.hh"
#include "revolutionfunctions.hh"
#include "spline.h"
#include "vector.hh"

namespace Mesh
{
  void generateTestMeshes();

  class Revolution
  {
  public:
    typedef float (*fttheta)(float t, float thetaLevel);
    typedef vector3f (*fttheta_center)(float t, float thetaLevel);
    typedef vector3f (*normalCompute)(const Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);

    // hFunc : fonction qui donne la hauteur en fonction de t et theta
    // rFunc : fonction qui donne le rayon en fonction de t et theta
    Revolution(fttheta_center hFunc, fttheta rFunc,
	       float * splineData = NULL, int splineLen = 0,
	       bool absoluteOrientation = false):
      _hFunc(hFunc),
      _rFunc(rFunc),
      _splineData(splineData),
      _splineLen(splineLen),
      _computeNormalFunc(generalPurposeComputeNormal),
      _absoluteOrientation(absoluteOrientation)
    {
      assert((rFunc != NULL) || (splineData != NULL && splineLen != 0));
      assert((hFunc != NULL) || (splineData != NULL && splineLen != 0));

      if (splineData!= NULL)
      {
	initSplineData(splineData, splineLen);
      }
    }

    inline
    void setComputeNormalFunc(normalCompute computeNormalFunc)
    {
      _computeNormalFunc = computeNormalFunc;
    };

    // tFaces : nombres de faces suivant t
    // thetaFaces : nombre de faces suivant theta
    void generateMesh(Array<vertex> & vertices,
		      float hScale, float rScale,
		      int tFaces, int thetaFaces);

  private:
    vector3f _splineHFunc(float t, float theta);
    float _splineRFunc(float t, float theta);

    vector3f Revolution::_getCenterPosition(float t, float thetaLevel);
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
    float *		_splineData;
    int			_splineLen;
    normalCompute	_computeNormalFunc;
    int			_hRepeat;
    int			_rRepeat;
    bool		_absoluteOrientation;
  };

#if DEBUG
  Mesh::Revolution loadSplineFromFile(const char *file);
#endif

}

#endif		// REVOLUTION_HH
