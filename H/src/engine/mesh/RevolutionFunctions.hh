#ifndef REVOLUTION_FUNCTIONS_HH
#define REVOLUTION_FUNCTIONS_HH

#include "engine/algebra/Vector3.hh"

namespace mesh
{
	Algebra::vector3f	pipeHFunc(float t, float theta);
	Algebra::vector3f	inPipeHFunc(float t, float theta);
	float				pipeRFunc(float t, float theta);
	Algebra::vector3f	wheelHFunc(float t, float theta);
	float				wheelRFunc(float t, float theta);
	
	Algebra::vector3f	sphereHFunc(float t, float theta);
	float				sphereRFunc(float t, float theta);
	Algebra::vector3f	torusHFunc(float t, float theta);
	float				torusRFunc(float t, float theta);
	
	Algebra::vector3f	hemisphereHFunc(float t, float theta);
	float				hemisphereRFunc(float t, float theta);
}

#endif // REVOLUTION_FUNCTIONS_HH
