//
// Définition d'un sommet
//

#ifndef VERTEX_HH
#define	VERTEX_HH

#include "engine/algebra/Vector3.hh"

#define VERTEX_ATTR_POSITION   1
#define VERTEX_ATTR_NORMAL     1
#define VERTEX_ATTR_TANGENT    1
#define VERTEX_ATTR_COLOR      0
#define VERTEX_ATTR_TEX_COORD  1
#define VERTEX_ATTR_ID         0

namespace mesh
{
	struct vertex
	{
#if VERTEX_ATTR_POSITION
		Algebra::vector3f p; // Vertex position.
#endif

#if VERTEX_ATTR_NORMAL
		Algebra::vector3f n; // Vertex normal.
#endif

#if VERTEX_ATTR_TANGENT
		Algebra::vector3f t; // Vertex tangent.
		float tw; // Tangent space handedness.
#endif

#if VERTEX_ATTR_COLOR
		float r; // Vertex color,
		float g;
		float b;
#endif

#if VERTEX_ATTR_TEX_COORD
		float u; // Vertex texture coordinate.
		float v; //
#endif

#if VERTEX_ATTR_ID
		float id; // Vertex id.
#endif
	};
}

#endif // VERTEX_HH
