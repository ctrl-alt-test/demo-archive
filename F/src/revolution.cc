//
// Forme de révolution
//

#include "revolution.hh"

#include "interpolation.hh"
#include "spline.h"
#include "vector.hh"

namespace Mesh
{
  //
  // Etapes
  //
  // - avoir une orientation radiale
  // - brancher avec un LSystem
  //

  vector3f Revolution::_splineHFunc(float t, float theta)
  {
    float ret[2];
    spline(_splineData, _splineLen / 3, 2, t, ret);
    return vector3f(0.f, ret[0] / 100.f, 0.f);
  }

  float Revolution::_splineRFunc(float t, float theta)
  {
    float ret[2];
    spline(_splineData, _splineLen / 3, 2, t, ret);
    return ret[1] / 100.f + 0.000001f;
    // Le epsilon sert à éviter d'avoir un rayon nul,
    // qui pose problème pour les tangeantes
  }

  static vector3f computeTangent(const vector3f & n,
				 const vector3f & OA)
  {
    const vector3f bt = n ^ OA;

    vector3f t = bt ^ n;
    normalize(t);

    return t;
  }

  vector3f Revolution::_getCenterPosition(float t, float thetaLevel)
  {
    if (_hFunc)
      return _hFunc(t, thetaLevel);
    return _splineHFunc(t, thetaLevel);
  }

  // Texture enroulée autour de la forme
  inline
  static vector3f _computeTexCoord(float t, float thetaLevel, float radius)
  {
    return vector3f(t, thetaLevel, radius == 0 ? 1.f : radius);
  }

  void Revolution::_computePositions(Array<vertex> & vertices,
				     float hScale, float rScale,
				     int tFaces, int thetaFaces)
  {
    //
    // FIXME : si au lieu de mettre le code dans la boucle, on génère
    // une liste de points pour la passer à une fonction, on peut
    // appeler cette fonction avec une liste de point préparée, pour
    // avoir des arrêtes, etc.
    //

    vector3f X(1.f, 0, 0);
    vector3f Y(0, 1.f, 0);

    int thetaStrongEdgePass = 0;
    if (thetaFaces < 0)
    {
      thetaFaces = -thetaFaces;
      thetaStrongEdgePass = 1;
    }

    for (int j = 0; j <= tFaces; ++j)
    {
      const float t = float(j) / tFaces;

      const vector3f p1 = (_getCenterPosition(float(j) / float(tFaces), 0) +
			   _getCenterPosition(float(j) / float(tFaces), 0.5f * PI) +
			   _getCenterPosition(float(j) / float(tFaces), PI) +
			   _getCenterPosition(float(j) / float(tFaces), 1.5f * PI));
      const vector3f p2 = (_getCenterPosition((float(j) + 0.5f) / float(tFaces), 0) +
			   _getCenterPosition((float(j) + 0.5f) / float(tFaces), 0.5f * PI) +
			   _getCenterPosition((float(j) + 0.5f) / float(tFaces), PI) +
			   _getCenterPosition((float(j) + 0.5f) / float(tFaces), 1.5f * PI));

      vector3f dp = p2 - p1;

      if (dp * Y < 0) { dp = -dp; }
      if (norm(dp) > 0.0001f)
      {
	normalize(dp);
	Y = dp;
      }

      vector3f Z = (_absoluteOrientation ? vector3f(1.f, 0, 0) : X) ^ Y;
      normalize(Z);
      X = Y ^ Z; normalize(X);
      Z = X ^ Y;

      matrix4 m = rotationMatrix(X, Y, Z);


      for (int i = 0; i <= thetaFaces; ++i)
      {
	for (int thetaEdge = 0; thetaEdge == 0 || (thetaEdge <= thetaStrongEdgePass && i < thetaFaces); ++thetaEdge)
	{
	  const float thetaLevel = float(i) / thetaFaces + 0.0001f * float(thetaEdge);
	  const float theta = 2.f * PI * thetaLevel;

	  const float radius =
	    (_rFunc ? _rFunc(t, theta) : _splineRFunc(t, theta));

	  const float x = radius * msys_cosf(theta);
	  const float z = radius * msys_sinf(theta);
	  vector3f revolutionCenter = _getCenterPosition(t, theta);

	  vector3f nPos = vector3f(x, revolutionCenter.y, z);
	  vector3f pos = vector3f(rScale * x, 0, rScale * z);

	  applyMatrixToVector(m, pos);

	  // translation
	  pos += revolutionCenter * hScale;

	  const vector3f texCoord = _computeTexCoord(t, thetaLevel, radius);

	  const vertex p = {pos, // Position
			    vector3f(0, 0, 0), // Normale (plus tard)
			    vector3f(0, 0, 0), // Tangente (plus tard)
// 			    1.f, 1.f, 1.f, // Couleur
			    texCoord.x, texCoord.y, 0, texCoord.z}; // Coordonnée de texture

	  vertices.add(p);
	}
      }
    }
  }

  void Revolution::_computeNormals(Array<vertex> & vertices,
				   int tFaces, int thetaFaces)
  {
    for (int j = 0; j <= tFaces; ++j)
      for (int i = 0; i <= thetaFaces; ++i)
      {
	vertex & p = vertices[j * (thetaFaces + 1) + i];

	const int i2 = (i == thetaFaces ? 1 : i + 1);
	const vertex & _A = vertices[i2 + j  * (thetaFaces + 1)];
	const vector3f & O = p.p;
	const vector3f & A = _A.p;
	const vector3f OA = A - O;

	p.n = _computeNormalFunc(vertices, tFaces, thetaFaces, i, j);
	p.t = computeTangent(p.n, OA);
      }
  }

  void Revolution::_makeQuadsFromVertices(Array<vertex> & uniqueVertices,
					  Array<vertex> & vertices,
					  int tFaces, int thetaFaces)
  {
    for (int j = 0; j < tFaces; ++j)
      for (int i = 0; i < thetaFaces; ++i)
      {
	vertices.add(uniqueVertices[ i      +  j      * (thetaFaces + 1)]);
	vertices.add(uniqueVertices[ i      + (j + 1) * (thetaFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) + (j + 1) * (thetaFaces + 1)]);
	vertices.add(uniqueVertices[(i + 1) +  j      * (thetaFaces + 1)]);
      }
  }

  void Revolution::generateMesh(Array<vertex> & vertices,
				float hScale, float rScale,
				int tFaces, int thetaFaces)
  {
    int signedThetaFaces = thetaFaces;
    if (thetaFaces < 0)
    {
      thetaFaces = -2 * thetaFaces;
    }

    assert(tFaces >= 1);
    assert(thetaFaces >= 3);

    Array<vertex> uniqueVertices((tFaces + 1) * (thetaFaces + 1));
    IFDBG(assert(vertices.max_size >= 4 * tFaces * thetaFaces));

    _computePositions(uniqueVertices, hScale, rScale, tFaces, signedThetaFaces);
    _computeNormals(uniqueVertices, tFaces, thetaFaces);
    _makeQuadsFromVertices(uniqueVertices, vertices, tFaces, thetaFaces);
  }
}
