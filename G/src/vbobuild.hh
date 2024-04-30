//
// Construction de VBO
//

#ifndef		VBO_BUILD_HH
# define	VBO_BUILD_HH

#include "vbodata.hh"

#include "array.hh"
#include "matrix.hh"
#include "mesh.hh"
#include "node.hh"
#include "renderlist.hh"
#include "vector.hh"

namespace VBO
{
  // Versions pour les hommes, où il faut gérer les indices

  unsigned int addCubeToChunk(vertex * dest, const vector3f & p,
			      float size = 1.f,
			      bool x1face = true, bool x2face = true,
			      bool y1face = true, bool y2face = true,
			      bool z1face = true, bool z2face = true);
  unsigned int addCubeToChunk(vertex * dest, const vector3f & p,
			      const vector3f & color,
			      bool x1face = true, bool x2face = true,
			      bool y1face = true, bool y2face = true,
			      bool z1face = true, bool z2face = true);
  unsigned int addCubeToChunk(vertex * dest, const vector3f &p,
			      const vector3f &x1Color,
			      const vector3f &x2Color,
			      const vector3f &y1Color,
			      const vector3f &y2Color,
			      const vector3f &z1Color,
			      const vector3f &z2Color,
			      bool x1face = true, bool x2face = true,
			      bool y1face = true, bool y2face = true,
			      bool z1face = true, bool z2face = true);
  unsigned int addCubeToChunk(vertex * dest, const matrix4 & transform,
			      const vector3f & color,
			      bool x1face = true, bool x2face = true,
			      bool y1face = true, bool y2face = true,
			      bool z1face = true, bool z2face = true);

  // Versions équivalentes, à base de tableau

  void addCube(Array<vertex> &vbo, const vector3f & pos, float size = 1.f,
	       bool x1face = true, bool x2face = true,
	       bool y1face = true, bool y2face = true,
	       bool z1face = true, bool z2face = true);
  void addCube(Array<vertex> &vbo, const vector3f & pos,
	       const vector3f & color,
	       bool x1face = true, bool x2face = true,
	       bool y1face = true, bool y2face = true,
	       bool z1face = true, bool z2face = true);
  void addCube(Array<vertex> &vbo, const vector3f & pos,
	       const vector3f & x1Color,
	       const vector3f & x2Color,
	       const vector3f & y1Color,
	       const vector3f & y2Color,
	       const vector3f & z1Color,
	       const vector3f & z2Color,
	       bool x1face = true, bool x2face = true,
	       bool y1face = true, bool y2face = true,
	       bool z1face = true, bool z2face = true);
  void addCube(Array<vertex> &vbo, const matrix4 & transform,
	       const vector3f & color,
	       bool x1face = true, bool x2face = true,
	       bool y1face = true, bool y2face = true,
	       bool z1face = true, bool z2face = true);

  // Version last minute VBO service

  void addElements(Array<vertex> & vbo,
		   const Node * tree,
		   bool x1face = true, bool x2face = true,
		   bool y1face = true, bool y2face = true,
		   bool z1face = true, bool z2face = true);
}

#endif		// VBO_BUILD_HH
