//
// Fonctions génératrices pour les formes de révolution
//

#ifndef		REVOLUTION_FUNCTIONS_HH
# define	REVOLUTION_FUNCTIONS_HH

#include "algebra/vector3.hh"
#include "array.hh"
#include "vertex.hh"

namespace Mesh
{
  vector3f pipeHFunc(float t, float theta);
  vector3f inPipeHFunc(float t, float theta);
  float pipeRFunc(float t, float theta);
  vector3f wheelHFunc(float t, float theta);
  float wheelRFunc(float t, float theta);

  vector3f sphereHFunc(float t, float theta);
  float sphereRFunc(float t, float theta);
  vector3f torusHFunc(float t, float theta);
  float torusRFunc(float t, float theta);

  vector3f hemisphereHFunc(float t, float theta);
  float hemisphereRFunc(float t, float theta);

  vector3f generalPurposeComputeNormal(const Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);
  vector3f sphereComputeNormal(const Array<vertex> & uniqueVertices, int tFaces, int thetaFaces, int i, int j);
  vector3f generalPurposeComputeNormalWithBottoms(const Array<vertex> & uniqueVertices, int tFaces, int thetaFaces, int i, int j);
  vector3f torusComputeNormal(const Array<vertex> & vertices, int tFaces, int thetaFaces, int i, int j);
}

#endif		// REVOLUTION_FUNCTIONS_HH
