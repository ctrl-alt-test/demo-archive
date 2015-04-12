//
// Intérieur de l'usine
//

#ifndef		FACTORY_HH
# define	FACTORY_HH

#include "renderlist.hh"
#include "vbodata.hh"

namespace Factory
{
  extern unsigned int numberOfZFightCubeVertices;
  extern Array<VBO::vertex> * frameVertices;
  extern VBO::vertex * disjoinedCubeVertices;
  extern VBO::vertex * shuffledCubeVertices;
  extern VBO::vertex * resizedCubeVertices;
  extern VBO::vertex * shearedCubeVertices;
  extern VBO::vertex * zFightCubeVertices;
  extern VBO::vertex * rubiksCubeVertices;

  extern const date testTreadmillCycleDuration;
  extern const date arrivalTreadmillCycleDuration;
  extern const date onTestTreadmillDuration;
  extern const unsigned int pitLength;
  extern const unsigned int pitWidth;
  extern const date onPitTreadmillDuration;
  extern const date validatorCycleDuration;
  extern const float validatorSize;
  extern const date dance1AnimDuration;
  extern const date dance2AnimDuration;

  void generateMeshes();

  void create(RenderList & renderList);

  void update(RenderList & renderList, date renderDate);

  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position);
  int notSoRandom(int seed);
}

#endif		// FACTORY_HH
