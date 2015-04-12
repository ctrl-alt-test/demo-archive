//
// Forme de révolution
//

#ifndef		CYLINDER_HH
# define	CYLINDER_HH

#include "array.hh"
#include "mesh.hh"
#include "vector.hh"

namespace Mesh
{
  extern Array<vertex> * mesh1;
  extern Array<vertex> * mesh2;
  extern Array<vertex> * mesh3;
  extern Array<vertex> * mesh4;
  extern Array<vertex> * mesh5;
  extern Array<vertex> * mesh6;
  extern Array<vertex> * mesh7;
  void generateTestMeshes();

  void addCylinder(Array<vertex> & vertices,
		   float (*heightForAxial)(float axial),
		   float (*radiusForAxial)(float axial),
		   vector3f (*computeNormal)(const Array<vertex> & uniqueVertices,
					     int radialFaces, int axialFaces,
					     int i, int j),
		   float diameter,
		   float length,
		   int radialFaces,
		   int axialFaces);
}

#endif		// CYLINDER_HH
